pi_sign_tool: main.o
	g++ -o pi_sign_tool main.o \
	-std=c++11 \
	-L/path/to/pi/lib \
	-L/path/to/pi/lib/cryptonomex \
	-lboost_thread -lboost_coroutine -lboost_filesystem \
	-lboost_date_time -lboost_system -lboost_chrono -lboost_context \
	-lssl -lcrypto \
	-lgraphene_chain \
	-lgraphene_utilities \
	-lgraphene_db \
	-lfc \
	-lsecp256k1	

main.o: main.cpp
	g++ -c main.cpp -std=c++11 \
	-I/path/to/pi/include \

clean: main.o pi_sign_tool
	rm -rf main.o pi_sign_tool