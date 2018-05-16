#include <iostream>
#include <string>
#include <tuple>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <graphene/chain/database.hpp>
#include <graphene/chain/balance_object.hpp>
#include <graphene/chain/asset_object.hpp>
#include <graphene/chain/witness_object.hpp>
#include <graphene/chain/committee_member_object.hpp>
#include <graphene/chain/proposal_object.hpp>
#include <graphene/utilities/key_conversion.hpp>
#include <fc/crypto/base36.hpp>

std::string RandomSeed() {
    std::string seed;
    for (int i = 0; i < 8; i++) {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        seed += boost::uuids::to_string(uuid);
        if (i == 7) {
            continue;
        }
        seed += ":";
    }
    return seed;
}

std::tuple<std::string, std::string, std::string> GenerateKeys(const std::string &seed) {
    auto key = fc::ecc::private_key::regenerate(fc::sha256::hash(seed));
    auto pub_key = key.get_public_key();
    auto bts_pub_key = graphene::chain::public_key_type(pub_key);
    
    auto key_data = pub_key.serialize();
    auto base36 = fc::to_base36(key_data.data, key_data.size());
    auto name = "n" + base36;

    auto pri_key = graphene::utilities::key_to_wif(key);

    return std::tuple<std::string, std::string, std::string>(name, bts_pub_key, pri_key);
}

void PrintHelp() {
    // pi_sign_tool key_gen
    // pi_sign_tool sign pri_key 'unsigned_tx_in_json_form'
    std::cerr << "Usage:\n" 
        << "\tpi_sign_tool key_gen\n" 
        << "\tpi_sign_tool sign pri_key \'unsigned_tx_in_json_form\'" 
        << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        PrintHelp();
        return -1;
    }
    std::string op = argv[1];
    if (argc == 2 && op == "key_gen") {
        std::string seed = RandomSeed();
        // std::cout << seed << std::endl;
        auto keys = GenerateKeys(seed);
        // std::cout << std::get<0>(keys) << std::endl;
        std::cout << std::get<1>(keys) << std::endl;
        std::cout << std::get<2>(keys) << std::endl;
    } else if (argc == 4 && op == "sign") {
        try {
            std::string pri_key_str = argv[2];
            auto pri_key = graphene::utilities::wif_to_key(pri_key_str);
            if (!pri_key) {
                throw "invalide private key.";
            }
            std::string tx_str = argv[3];
            graphene::chain::signed_transaction tx;
            auto tx_json = fc::json::from_string(tx_str);
            fc::from_variant(tx_json, tx);
            tx.sign(
                *pri_key, 
                graphene::chain::chain_id_type("ae471be89b3509bf7474710dda6bf35d893387bae70402b54b616d72b83bc5a4")
            );
            fc::variant signature;
            fc::to_variant(tx.signatures[0], signature);
            std::cout << signature.as_string() << std::endl;
        } catch (...) {
            std::cerr << "Invalide params" << std::endl;
            return -2;
        }
    } else {
        PrintHelp();
        return -1;
    }
    return 0;
}
