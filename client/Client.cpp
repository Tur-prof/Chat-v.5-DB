#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include "Client.h"

namespace fs = std::filesystem;

#if defined(_WIN64)
#define PLATFORM_NAME "windows 64-bit" // Windows 64-bit
#elif defined(__ANDROID__)
#define PLATFORM_NAME "android"   // Android 
#elif defined(__linux__)
#define PLATFORM_NAME "linux"     // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos и другие
#elif TARGET_OS_IPHONE == 1
#define PLATFORM_NAME "ios"       // Apple iOS
#elif TARGET_OS_MAC == 1
#define PLATFORM_NAME "osx"       // Apple OSX
#else
#define PLATFORM_NAME  "OS not detected" // ОС не распознана
#endif

Client::Client()
{
    std::string _ip;
    std::string _port;
    setting.open("setting_client.txt", std::fstream::in | std::fstream::out | std::fstream::app);
    fs::permissions("setting_client.txt", fs::perms::owner_all);
    if (!setting.is_open())
    {
        std::cout << "File openning error!" << std::endl;
    }
    else
    {
        fs::permissions("setting_client.txt", fs::perms::owner_all);
        while (getline(setting, _ip, ' ') && getline(setting, _port, ' '))
        {
        }
    }
    setting.close();
    IP = _ip;
    PORT = static_cast<uint16_t>(std::stoi(_port));
}

//Функция установки включения программы
    
bool Client::createSock()
{
    std::cout << "\033[1;31m" << PLATFORM_NAME << "\033[32m" << std::endl;
    // Создадим сокет
#ifdef _WIN64
    check_status = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (check_status != 0)
    {
        std::cout << "Error WinSock version initializaion #" << WSAGetLastError() << std::endl;
        return false;
    }
    connection = socket(AF_INET, SOCK_STREAM, 0);
    if (connection == INVALID_SOCKET) {
        std::cout << "Creation of Socket failed with error: " << WSAGetLastError() << std::endl;
        closesocket(connection);
        WSACleanup();
        return false;
    }
    ZeroMemory(&serveraddress, sizeof(serveraddress));
#elif __linux__
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file_descriptor == -1) {
        std::cout << "Creation of Socket failed!" << std::endl;
        return false;
    }
#endif
    serveraddress.sin_addr.s_addr = inet_addr(IP.c_str());
    // Зададим номер порта
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;
    return true;
}

void Client::connectServer()
{
    // Установим соединение с сервером
#ifdef _WIN64
    connection_status = connect(connection, (sockaddr*)&serveraddress, sizeof(serveraddress));
    if (connection_status != 0) {
        std::cout << "Connection with the server failed!" << std::endl;
        closesocket(connection);
        WSACleanup();
        return;
    }
#elif __linux__
    connection = connect(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
    if (connection == -1) {
        std::cout << "Connection with the server failed!" << std::endl;
        close(socket_file_descriptor);
        exit(1);
        }
#endif
    else {
        std::cout << "Connection with the server is Ok..." << std::endl;
    }
    std::cout << "Hello!" << std::endl;
    communication();
}

void Client::communication()
{
    // Взаимодействие с сервером
    while (1) {
#ifdef _WIN64
        // чтение сообщения от сервера
        recv(connection, message, sizeof(message), 0);
        std::cout /*<< "Message from server: "*/ << message << std::endl;
        if (message == "end")
        {
            closesocket(connection);
            break;
        }

        // отправка сообщения серверу
        std::cin.getline(message, MESSAGE_LENGTH);
        send(connection, message, sizeof(message), 0);

#else
        // чтение сообщения от сервера
        bzero(message, sizeof(message));
        // Ждем ответа от сервера
        read(socket_file_descriptor, message, sizeof(message));
        std::cout /*<< "Message from server: "*/ << message << std::endl;
        if (message == "end")
        {
            close(socket_file_descriptor);
            break;
        }

        // отправка сообщения серверу
        bzero(message, sizeof(message));
        //std::cout << "Message from client: ";
        std::cin.getline(message, MESSAGE_LENGTH);

        ssize_t bytes = write(socket_file_descriptor, message, sizeof(message));
        // Если передали >= 0  байт, значит пересылка прошла успешно
        if (bytes < 0) {
            std::cout << "Error!" << std::endl;
        }
#endif
    }
}

Client::~Client()
{
    // закрываем сокет, завершаем соединение    
#ifdef _WIN64
    closesocket(connection);
    WSACleanup();
#else
    close(socket_file_descriptor);
#endif
}






