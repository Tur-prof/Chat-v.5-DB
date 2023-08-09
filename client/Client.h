#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstring>
#ifdef _WIN64
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#elif __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define MESSAGE_LENGTH 1024 // Максимальный размер буфера данных

class Client
{
public:
	Client();
	bool createSock();
	void connectServer();
	void communication();

	std::fstream setting;

	std::string IP;
	uint16_t PORT;

	~Client();
private:
#ifdef _WIN64
	WSADATA wsaData;
	SOCKET connection;
	int check_status, connection_status;
#elif __linux__
	int socket_file_descriptor, connection;
#endif
	struct sockaddr_in serveraddress, clientaddress;
	char message[MESSAGE_LENGTH];
};