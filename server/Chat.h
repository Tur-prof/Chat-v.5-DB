#pragma once
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <filesystem>
#include <ctime>
#ifdef _WIN64
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "Message.h"
#include "User.h"
#include "Database.h"


#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных

class Chat {
public:
    Chat();

    uint16_t PORT;

    void start();
    bool chatOn() const;
    void loginOn();
    bool checkLogin() const;
    void showLoginMenu();
    void showUserMenu();

    bool login();
    void registrUser();
    void showMenuChatPrivate();
    void showMenuChatPublic();
    void showUsersList();
    void showChatPublic();
    void addMessagePublic();
    void showChatPrivate();
    void addMessagePrivate();
    std::string getDateTime();

    // Далее идут функции работы СЕРВЕРА - SERVER MODE (SM)
#ifdef _WIN64
    void sm_send(SOCKET, char[]);
    void sm_get(SOCKET, char[]);
    void serverOn();
    void checkLoginSM(SOCKET);
    void showUserMenuSM(SOCKET);
    void registrUserSM(SOCKET);
    void showMenuChatPublicSM(SOCKET);
    void showMenuChatPrivateSM(SOCKET);
    void showUsersListSM(SOCKET);
    void showChatPublicSM(SOCKET);
    void addMessagePublicSM(SOCKET);
    void showChatPrivateSM(SOCKET);
    void addMessagePrivateSM(SOCKET);
#elif __linux__
    void sm_send(int, char[]);
    void sm_get(int, char[]);
    void serverOn();
    void checkLoginSM(int);
    void showUserMenuSM(int);
    void registrUserSM(int);
    void showMenuChatPublicSM(int);
    void showMenuChatPrivateSM(int);
    void showUsersListSM(int);
    void showChatPublicSM(int);
    void addMessagePublicSM(int);
    void showChatPrivateSM(int);
    void addMessagePrivateSM(int);
#endif
    ~Chat() = default;

    std::fstream setting_port;

private:

    bool _chatOn = false;
    bool _checkLogin = false;
    bool _clientLogin = false;
    bool server_online = false;
    bool checkLog = false;
    std::string _admin_online;
    std::string _user_online;

    std::vector<std::string> _logins;
    std::vector<User> _users;
    std::vector<Message> _messages;
    std::map<std::string, bool> _active_user;

    std::shared_ptr<Database> _database;

#ifdef _WIN64
    WSADATA wsaData;
    SOCKET server, client;
    struct sockaddr_in serveraddress, clientaddress;
#elif __linux__
    int connection;
    struct sockaddr_in serveraddress, client;
#endif
    socklen_t length;
};
