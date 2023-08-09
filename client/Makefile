SRC = Client.h Client.cpp main.cpp

client : $(SRC)
	g++ -std=c++17 -o client $(SRC)

install:
	install ./client /usr/local/bin

uninstall:
	rm - rf /usr/local/bin/client
