#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <cstring>
#include "Chat.h"
#include "BadLoginRegistr.h"
#include "BadLoginMessage.h"

#if defined(_WIN64)
#define PLATFORM_NAME "windows 64-bit" // Windows 64-bit
#elif defined(_WIN32) 
#define PLATFORM_NAME "windows 32-bit" // Windows 32-bit
#elif defined(__ANDROID__)
#define PLATFORM_NAME "android"   // Android 
#elif defined(__linux__)
#define PLATFORM_NAME "linux"     // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos и другие
#elif TARGET_OS_IPHONE == 1
#define PLATFORM_NAME "ios"       // Apple iOS
#elif TARGET_OS_MAC == 1
#define PLATFORM_NAME "osx"       // Apple OSX
#else
#define PLATFORM_NAME "OS not detected" // ОС не распознана
#endif

namespace fs = std::filesystem;

Chat::Chat() : _database(std::make_shared <Database>())
{
    std::string _port;
    setting_port.open("setting_server.txt", std::fstream::in | std::fstream::out | std::fstream::app);
    fs::permissions("setting_server.txt", fs::perms::owner_all);
    if (!setting_port.is_open())
    {
        std::cout << "File openning error!" << std::endl;
    }
    else
    {
        fs::permissions("setting_server.txt", fs::perms::owner_all);
        while (getline(setting_port, _port, ' '))
        {
        }
    }
    setting_port.close();
    PORT = static_cast<uint16_t>(std::stoi(_port));
}

//Функция установки включения программы
void Chat::start()
{
    _chatOn = true;
    _users = _database->write_vector_users();
    _messages = _database->write_vector_messagesPublic();
    _messages = _database->write_vector_messagesPrivate();
    _active_user = _database->write_map_users_online();
    std::cout << getDateTime() << std::endl;
    std::cout << "\033[1;31m" << PLATFORM_NAME << std::endl;
}

//Функция возврата включенного состояния программы
bool Chat::chatOn() const
{
    return _chatOn;
}

//Функция установки авторизации пользователя
void Chat::loginOn()
{
    _checkLogin = true;
}

//Функция возврата режима авторизованного пользователя
bool Chat::checkLogin() const
{
    return _checkLogin;
}

//Функция вывода главного меню
void Chat::showLoginMenu()
{
    char operation;

    while (_chatOn && !_checkLogin)
    {
        std::cout << "\033[1;32m" << "(1) Login" << std::endl;
        std::cout << "(2) Regisration" << std::endl;
        std::cout << "(0) Exit" << std::endl;
        std::cout << ">> " << std::endl;
        std::cin >> operation;

        switch (operation)
        {
        case '1':
            if (login())
            {
                _checkLogin = false;
            }
            break;
        case '2':
            registrUser();
            break;
        case '0':
            _chatOn = false;
            break;
        default:
            std::cout << "\033[31m" << "Choose 1 or 2..." << std::endl;
            break;
        }
    }
}

//Функция авторизации пользователя
bool Chat::login()
{
    std::string login;
    std::string password;
    std::string status;
    bool is_found = false;
    char operation;

    while (!is_found)
    {
        std::cout << "login: ";
        std::cin >> login;
        std::cout << "password: ";
        std::cin >> password;
        for (const User& u: _users)
        {
            if (u.getUserLogin() == login && u.getUserPassword() == password)
            {
                std::cout << "\033[31m" << "LOGIN SUCCESS" << std::endl;
                _admin_online = login;
                status = "1";
                _active_user[_admin_online] = true;
                _database->changeStatusByUserDB(_admin_online, status);
                loginOn();
                return is_found;
            }
        }
        if (!is_found)
        {
            std::cout << "\033[31m" << "LOGIN OR PASSWORD FAILED" << std::endl;
            std::cout << "\033[32m" << "Press (0) for exit or press any button to try again" << std::endl;
            std::cin >> operation;
            if (operation == '0')
            {
                break;
            }
        }
    }
    return is_found;
}

//Функция регистрации пользователя
void Chat::registrUser()
{
    std::string login;
    std::string password;
    std::string status;
    std::cout << "new login: ";
    std::cin.ignore();
    std::cin >> login;
    if (_users.empty())
    {
        try
        {
            if (login == "All")
            {
                throw BadLoginRegistr();
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return;
        }
    }
    else
    {
        try
        {
            for (const auto& u : _users)
            {
                if (u.getUserLogin() == login)
                {
                    throw BadLoginRegistr();
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return;
        }
    }
    std::cout << "password: ";
    std::cin.ignore();
    std::cin >> password;
    status = "0";
    _database->addUserDB(login, password, status);
    User user(login, password);
    _users.emplace_back(user);
    _active_user[login] = false;
}

//Функция вывода меню пользователя
void Chat::showUserMenu()
{
    char operation;
    std::string status;
    std::cout << "\033[32m" << "Hi, " << "\033[37m" << _admin_online << std::endl;

    while (_checkLogin) {
        std::cout << "\033[36m" << "Menu: (#) SERVER MODE | (1) Chat Public |" << "\033[35m" << " (2) Chat Private |" << "\033[37m" << " (3) Users |" << "\033[32m" << " (0) Logout";

        std::cout << std::endl
            << "\033[37m" << ">> ";
        std::cin >> operation;

        switch (operation)
        {
        case '#':
            serverOn();
            break;
        case '1':
            showMenuChatPublic();
            break;
        case '2':
            showMenuChatPrivate();
            break;
        case '3':
            showUsersList();
            break;
        case '0':
            status = "0";
            _database->changeStatusByUserDB(_admin_online, status);
            _active_user.at(_admin_online) = false;
            _checkLogin = false;
            break;
        default:
            std::cout << "\033[31m" << "Choose 1 or 2 or 3..." << std::endl;
            break;
        }
    }
    std::cout << "\033[36m" << "--------------" << std::endl;
}


//Функция вывода меню публичного чата
void Chat::showMenuChatPublic()
{
    bool chat_public_on = true;
    char operation;
    std::cout << "\033[36m" << "--- Chat Public ---" << std::endl;
    while (chat_public_on) {
        std::cout << "\033[36m" << "Menu:  (1) ShowChat |  (2) Add Message |  (0) Back"
            << std::endl
            << ">> ";
        std::cin >> operation;

        switch (operation)
        {
        case '1':
            showChatPublic();
            break;
        case '2':
            addMessagePublic();
            break;
        case '0':
            chat_public_on = false;
            break;
        default:
            std::cout << "\033[31m" << "Choose 1 or 2..." << std::endl;
            break;
        }
    }
    std::cout << "\033[36m" << "--------------" << std::endl;
}

//Функция вывода меню приватного чата
void Chat::showMenuChatPrivate()
{
    bool chat_private_on = true;
    char operation;

    while (chat_private_on) {
        std::cout << "\033[35m" << "--- Chat Private ---" << std::endl;
        std::cout << "Menu: (1) ShowChat | (2) Add Message | (0) Back"
            << std::endl
            << ">> ";
        std::cin >> operation;

        switch (operation)
        {
        case '1':
            showChatPrivate();
            break;
        case '2':
            addMessagePrivate();
            break;
        case '0':
            chat_private_on = false;
            break;
        default:
            std::cout << "\033[31m" << "Choose 1 or 2..." << std::endl;
            break;
        }
    }
    std::cout << "\033[35m" << "--------------" << std::endl;
}

//Функция вывода списка пользователей
void Chat::showUsersList()
{
    std::cout << "\033[37m" << "--- Users ---" << std::endl;
    for (const auto& u : _active_user)
    {
        if (u.first == _admin_online) {
            std::cout << "\033[37m" << u.first << "\033[32m" << " online" << "\033[37m" << " (you)" << std::endl;
        }
        else if (u.second == true) {
            std::cout << "\033[37m" << u.first << "\033[32m" << " online" << std::endl;
        }
        else {
            std::cout << "\033[37m" << u.first << "\033[31m" << " offline" << std::endl;
        }
    }
    std::cout << "\033[37m" << "--------------" << std::endl;
}

//Функция вывода публичного чата
void Chat::showChatPublic()
{
    for (const auto& m : _messages)
    {
        if (m.getTo() == "All")
        {
            std::cout << m.getTime() << " || From " << m.getFrom() << " To " << m.getTo() << " : " << m.getText();
            std::cout << std::endl;
        }
    }
}

//Функция добавления публичных сообщений
void Chat::addMessagePublic() {
    std::string from = _admin_online;
    std::string to = "All";
    std::string text;
    std::string time;
    std::cout << "Text: ";
    std::cin.ignore();
    std::getline(std::cin, text);
    time = getDateTime();
    _database->addMessagePublicDB(from, text, time);
    _messages.push_back({ from, to, text, time });
}

//Функция вывода приватного чата
void Chat::showChatPrivate()
{
    for (const auto& m : _messages) {
        if (m.getTo() == _admin_online || m.getFrom() == _admin_online) {
            std::cout << m.getTime() << " || From " << m.getFrom() << " To " << m.getTo() << " : " << m.getText();
            std::cout << std::endl;
        }
    }
}

//Функция добавления приватных сообщений
void Chat::addMessagePrivate() {
    int _found = 0;
    std::string from = _admin_online;
    std::string to;
    std::string text;
    std::string time;
    std::cout << "To: ";
    std::cin >> to;
    try
    {
        for (const auto& u : _users)
        {
            if (u.getUserLogin() == to)
            {
                _found = 1;
            }
        }
        if (_found == 0) {
            throw BadLoginMessage();
        }
        else {
            std::cout << "Text: ";
            std::cin.ignore();
            std::getline(std::cin, text);
            time = getDateTime();
            _database->addMessagePrivateDB(from, to, text, time);
            _messages.push_back({ from, to, text, time });
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

std::string Chat::getDateTime()
{
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);

    char buffer[26];
    strftime(buffer, sizeof(buffer), "%m-%d-%Y %X", now);
    return buffer;
}

// Далее идут функции работы СЕРВЕРА - SERVER MODE (SM) для Windows
#ifdef _WIN64
void Chat::sm_send(SOCKET client, char* i_message)
{
    size_t bytes = send(client, i_message, MESSAGE_LENGTH, 0);
    // Если передали >= 0  байт, значит пересылка прошла успешно
    if (bytes >= 0)
    {
        //std::cout << "Data successfully sent to the client.!" << std::endl;
    }
    //WSACleanup();
}

void Chat::sm_get(SOCKET client, char* i_message)
{
    recv(client, i_message, MESSAGE_LENGTH, 0);
    if (strncmp("QQQ", i_message, 3) == 0) {
        std::cout << "Client Exited." << std::endl;
        std::cout << "Server is Exiting..!" << std::endl;
        return;
    }
    //std::cout << "Data received from client: " << i_message << std::endl;
    //WSACleanup();
}

void Chat::serverOn()
{
    int check_status, bind_status, connection_status;
    char msg[MESSAGE_LENGTH];
    std::string buff_mess;
    check_status = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (check_status != 0)
    {
        std::cout << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "WinSock successfully initialized" << std::endl;
    }

    // Создадаем сокет
    server = socket(AF_INET, SOCK_STREAM, 0);
    // если не удалось 
    if (server == -1) {
        std::cout << "Socket creation failed!" << std::endl;
        closesocket(server);
        WSACleanup();
        exit(1);
    }
    // содержит IP-адрес хоста
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // Зададим номер порта для связи
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;

    // Привяжем сокет
    bind_status = bind(server, (sockaddr*)&serveraddress, sizeof(serveraddress));
    // если не удалось
    if (bind_status != 0) {
        std::cout << "Socket binding failed!" << std::endl;
        closesocket(server);
        WSACleanup();
        exit(1);
    }
    else
    {
        std::cout << "Server socket successfully initialized" << std::endl;
    }

    // сокет переводится в режим ожидания запросов со стороны клиентов
    connection_status = listen(server, SOMAXCONN);
    // если не удалось
    if (connection_status != 0) {
        std::cout << "Socket is unable to listen for new connections!" << std::endl;
        closesocket(server);
        WSACleanup();
        exit(1);
    }
    else {
        std::cout << "Server is listening for new connection..." << std::endl;
    }

    ZeroMemory(&clientaddress, sizeof(clientaddress));
    int client_size = sizeof(clientaddress);
    client = accept(server, (sockaddr*)&clientaddress, &client_size);
    if (client == INVALID_SOCKET)
    {
        std::cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << std::endl;
        closesocket(server);
        closesocket(client);
        WSACleanup();
        exit(1);
    }
    else
    {
        std::cout << "Server: got connection from " << inet_ntoa(clientaddress.sin_addr) << " port " << ntohs(clientaddress.sin_port) << std::endl;
    }
    
    server_online = true;
    
    while (server_online)
    {
        char operation;
        buff_mess = "(1) Login\n(2) Regisration\n(0) Exit\n>>";
        strcpy(msg, buff_mess.c_str());
        sm_send(client, msg);
        
        sm_get(client, msg);
        operation = msg[0];
        switch(operation)
        {
        case '1':
            checkLoginSM(client);
            break;
        case '2':
            registrUserSM(client);
            break;
        case '0':
        buff_mess = "end";
        strcpy(msg, buff_mess.c_str());
        sm_send(client, msg);

        sm_get(client, msg);
        std::cout << msg << std::endl;
         
	    server_online = false;
        closesocket(client);
        WSACleanup();
        return;
        default:
            buff_mess = "Bad choise...\nPress any button to repeat";
            strcpy(msg, buff_mess.c_str());
            sm_send(client, msg);

            sm_get(client, msg); 
            break;
        }
    }
}

void Chat::checkLoginSM(SOCKET client)
{
    checkLog = true;
    std::string login, password, status, buff_mess;
    char msg[MESSAGE_LENGTH];

    while (checkLog)
    {
        std::string check_login, check_password;
        buff_mess = "login:";
        strcpy(msg, buff_mess.c_str());
        sm_send(client, msg);

        sm_get(client, msg);
        check_login = msg;

        buff_mess = "password:";
        strcpy(msg, buff_mess.c_str());
        sm_send(client, msg);

        sm_get(client, msg);
        check_password = msg;

        for (const User& u : _users)
        {
            if (u.getUserLogin() == check_login && u.getUserPassword() == check_password)
            {
                status = "1";
                _database->changeStatusByUserDB(check_login, status);
                _user_online = check_login;
                _active_user[check_login] = true;
                _clientLogin = true;
                showUserMenuSM(client);
                break;
            }
        }
        if (!_clientLogin)
        {
            buff_mess = "LOGIN OR PASSWORD FAILED\nPress any button to repeat";
            strcpy(msg, buff_mess.c_str());
            sm_send(client, msg);

            sm_get(client, msg);
            break;
        }
    }
}

void Chat::registrUserSM(SOCKET client)
{
    std::string new_login, password, status, buff_mess;
    char msg[MESSAGE_LENGTH];
    bool flag = false;
    
    buff_mess = "new login: ";
    strcpy(msg, buff_mess.c_str());
    sm_send(client, msg);

    sm_get(client, msg);
    new_login = msg;
    
    buff_mess = "password: ";
    strcpy(msg, buff_mess.c_str());
    sm_send(client, msg);
    
    sm_get(client, msg);
    password = msg;

    for (const auto& u : _active_user)
    {
        if (u.first == new_login)
        {
            buff_mess = "This username is taken, please try another one\nPress any button to repeat";
            strcpy(msg, buff_mess.c_str());
            sm_send(client, msg);
            flag = true;

            sm_get(client, msg);
            break;
        }
    }
    if (!flag)
    {
        status = "0";
        _database->addUserDB(new_login, password, status);
        User user(new_login, password);
        _users.emplace_back(user);
        _active_user[new_login] = false;
    }
}

void Chat::showUserMenuSM(SOCKET client)
{
    bool _menu_on = true;
    std::string login, password, status, buff_mess;
    char operation;
    char msg[MESSAGE_LENGTH];

    while (_menu_on) 
    {
        buff_mess = "LOGIN SUCCESS\nMenu: (1) Chat Public | (2) Chat Private | (3) Users |(0) Logout\nHi!, " + _user_online + "\n>>";
        strcpy(msg, buff_mess.c_str());
        sm_send(client, msg);

        sm_get(client, msg);
        operation = msg[0];
        
        switch (operation)
        {
        case '1':
            showMenuChatPublicSM(client);
            break;
        case '2':
            showMenuChatPrivateSM(client);
            break;
        case '3':
            showUsersListSM(client);
            break;
        case '0':
            status = "0";
            _database->changeStatusByUserDB(_admin_online, status);
            _active_user.at(_user_online) = false;
            _menu_on = false;
            checkLog = false;
            break;
        default:
            buff_mess = "Choose 1 or 2 or 3...\n--------------\nPress any button to repeat";
            strcpy(msg, buff_mess.c_str());
            sm_send(client, msg);

            sm_get(client, msg);
            break;
        }
    }
}

void Chat::showMenuChatPublicSM(SOCKET client)
{
    bool chat_public_on = true;
    std::string buff_mess1;
    char operation;
    char msg[MESSAGE_LENGTH];
    while (chat_public_on) {
        buff_mess1 = "--- Chat Public ---\nMenu:  (1) ShowChat | (2) Add Message | (0) Back\n>>";
        strcpy(msg, buff_mess1.c_str());
        sm_send(client, msg);

        sm_get(client, msg);
        operation = msg[0];

        switch (operation)
        {
        case '1':
        
            showChatPublicSM(client);
            break;
        case '2':
            addMessagePublicSM(client);
            break;
        case '0':
            chat_public_on = false;
            break;
        default:
            buff_mess1 = "Choose 1 or 2...\n--------------\nPress any button to repeat";
            strcpy(msg, buff_mess1.c_str());
            sm_send(client, msg);

            sm_get(client, msg);
            break;
        }
    }
}

void Chat::showMenuChatPrivateSM(SOCKET client)
{
    bool chat_private_on = true;
    std::string buff_mess;
    char operation;
    char msg[MESSAGE_LENGTH];

    while (chat_private_on) {
        buff_mess = "--- Chat Private ---\nMenu:  (1) ShowChat | (2) Add Message | (0) Back\n>>";
        strcpy(msg, buff_mess.c_str());
        sm_send(client, msg);

        sm_get(client, msg);
        operation = msg[0];

        switch (operation)
        {
        case '1':
            showChatPrivateSM(client);
            break;
        case '2':
            addMessagePrivateSM(client);
            break;
        case '0':
            chat_private_on = false;
            break;
        default:
            buff_mess = "Choose 1 or 2...";
            strcpy(msg, buff_mess.c_str());
            sm_send(client, msg);

            sm_get(client, msg);
        }
    }
}

void Chat::showChatPublicSM(SOCKET client)
{
    std::string buff_mess;
    char msg[MESSAGE_LENGTH];
    for (const auto& m : _messages) 
    {
        if (m.getTo() == "All")
        {
            buff_mess += m.getTime() + " || From " + m.getFrom() + " To " + m.getTo() + " : " + m.getText() + "\n";
        }
    }
    buff_mess += "--------------\nPress any button to continue";
    strcpy(msg, buff_mess.c_str());
    sm_send(client, msg);

    sm_get(client, msg);
}

void Chat::addMessagePublicSM(SOCKET client)
{
    bool go_chat_public = true;
    std::string buff_mess;
    char msg[MESSAGE_LENGTH];
    std::cout << "User "<< _user_online << " has entered a public chat! Let's talk to him" << std::endl;
    buff_mess = "HI, " + _user_online + "\n(QQQ) - Exit\nPress any button to start";
    strcpy(msg, buff_mess.c_str());
    sm_send(client, msg);

    sm_get(client, msg);

    while(go_chat_public)
    {
        std::string from = _admin_online;
        std::string to = "All";
        std::string text;
        std::string time;
        std::getline(std::cin, text);
        if (text == "QQQ")
        {
            go_chat_public = false;
            break;
        }
        time = getDateTime();
        _database->addMessagePublicDB(from, text, time);
        Message message(from, to, text, time);
        _messages.emplace_back(message);

        buff_mess = time + " || " + from + " : " + text;
        strcpy(msg, buff_mess.c_str());
        sm_send(client, msg);

        sm_get(client, msg);
        buff_mess = msg;
        from = _user_online;
        time = getDateTime();
        std::cout << time << " || " << _user_online << " : " << msg << std::endl;
        if (buff_mess == "QQQ")
            {
                go_chat_public = false;
                break;
            }    
        _database->addMessagePublicDB(from, text, time);
        Message msg(from, to, text, time);
        _messages.emplace_back(msg);
    }  
}

void Chat::showChatPrivateSM(SOCKET client)
{
    std::string buff_mess;
    char msg[MESSAGE_LENGTH];
    for (const auto& m : _messages) 
    {
        if (m.getTo() == _user_online || m.getFrom() == _user_online && m.getTo() != "All") 
        {
            buff_mess += m.getTime() + " || From " + m.getFrom() + " To " + m.getTo() + " : " + m.getText() + "\n";
        }
    }
    buff_mess += "--------------\nPress any button to continue";
    strcpy(msg, buff_mess.c_str());
    sm_send(client, msg);

    sm_get(client, msg);
}

void Chat::addMessagePrivateSM(SOCKET client) {
    
    std::string buff_mess, to, text, time;
    std::string from = _user_online;
    char msg[MESSAGE_LENGTH];
    int _found = 1; 
    
    buff_mess = "To: ";
    strcpy(msg, buff_mess.c_str());
    sm_send(client, msg);
        
    sm_get(client, msg);
    to = msg;
    for (const auto& u : _active_user)
    {
        if (u.first == to)
        {
            _found = 0;
        }
    }
    if (_found == 1) 
    {
        buff_mess = "User with this login does not exist\n--------------\nPress any button to continue";
        strcpy(msg, buff_mess.c_str());
        sm_send(client, msg);

        sm_get(client, msg);
        return;
    }
    else 
    {
        buff_mess = "Text: ";
        strcpy(msg, buff_mess.c_str());
        sm_send(client, msg);

        sm_get(client, msg);
        text = msg;
        time = getDateTime();
        _database->addMessagePrivateDB(from, to, text, time);
        Message msg(from, to, text, time);
        _messages.emplace_back(msg);
    }
}

void Chat::showUsersListSM(SOCKET client)
{
    std::string buff_mess;
    char msg[MESSAGE_LENGTH];
    std::string login;
    std::string password;
    for (const auto& u : _active_user)
    {
        if (u.second == true)
        {
            buff_mess += u.first + " online\n";
        }
        else 
        {
            buff_mess += u.first + " offline\n";
        }
    }
    buff_mess += "--------------\nPress any button to continue";
    strcpy(msg, buff_mess.c_str());
    sm_send(client, msg);

    sm_get(client, msg);
}

// Далее идут функции работы СЕРВЕРА - SERVER MODE (SM) для __linux__

#else __linux__
void Chat::sm_send(int connection, char* i_message)
{
    ssize_t bytes = write(connection, i_message, MESSAGE_LENGTH);
    // Если передали >= 0  байт, значит пересылка прошла успешно
    if (bytes >= 0)
    {
        //std::cout << "Data successfully sent to the client.!" << std::endl;
    }

}

void Chat::sm_get(int connection, char* i_message)
{
    read(connection, i_message, MESSAGE_LENGTH);
    if (strncmp("QQQ", i_message, 3) == 0) {
        std::cout << "Client Exited." << std::endl;
        std::cout << "Server is Exiting..!" << std::endl;
        return;
    }
    //std::cout << "Data received from client: " << i_message << std::endl;
}

void Chat::serverOn()
{
    int socket_file_descriptor, connection, bind_status, connection_status;
    char msg[MESSAGE_LENGTH];
    std::string buff_mess;
    bzero(msg, MESSAGE_LENGTH);
    // Создадаем сокет
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    // если не удалось 
    if (socket_file_descriptor == -1) {
        std::cout << "Socket creation failed!" << std::endl;
        exit(1);
    }
    // содержит IP-адрес хоста
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // Зададим номер порта для связи
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;

    // Привяжем сокет
    bind_status = bind(socket_file_descriptor, (struct sockaddr*)&serveraddress,
        sizeof(serveraddress));
    // если не удалось
    if (bind_status == -1) {
        std::cout << "Socket binding failed!" << std::endl;
        exit(1);
    }


    // сокет переводится в режим ожидания запросов со стороны клиентов
    connection_status = listen(socket_file_descriptor, 5);
    // если не удалось
    if (connection_status == -1) {
        std::cout << "Socket is unable to listen for new connections!" << std::endl;
        exit(1);
    }
    else {
        std::cout << "Server is listening for new connection..." << std::endl;
    }

    length = sizeof(client);
    // создаём для общения с клиентом новый сокет и возвращаем его дескриптор
    connection = accept(socket_file_descriptor, (struct sockaddr*)&client, &length);
    // если не удалось
    if (connection == -1) {
        std::cout << "Server is unable to accept the data from client!" << std::endl;
        exit(1);
    }

    std::cout << "Server: got connection from " << inet_ntoa(client.sin_addr) << " port " << ntohs(client.sin_port) << std::endl;

    server_online = true;

    while (server_online)
    {
        char operation;
        bzero(msg, MESSAGE_LENGTH);
        buff_mess = "(1) Login\n(2) Regisration\n(0) Exit\n>>";
        strcpy(msg, buff_mess.c_str());
        sm_send(connection, msg);

        bzero(msg, MESSAGE_LENGTH);
        sm_get(connection, msg);
        operation = msg[0];

        switch (operation)
        {
        case '1':
            checkLoginSM(connection);
            break;
        case '2':
            registrUserSM(connection);
            break;
        case '0':
            bzero(msg, MESSAGE_LENGTH);
            buff_mess = "end";
            strcpy(msg, buff_mess.c_str());
            sm_send(connection, msg);

            bzero(msg, MESSAGE_LENGTH);
            sm_get(connection, msg);

            server_online = false;
            close(socket_file_descriptor);
            return;
        default:
            bzero(msg, MESSAGE_LENGTH);
            buff_mess = "Bad choise...\nPress any button to repeat";
            strcpy(msg, buff_mess.c_str());
            sm_send(connection, msg);

            bzero(msg, MESSAGE_LENGTH);
            sm_get(connection, msg);
            break;
        }
    }
}

void Chat::checkLoginSM(int connection)
{
    checkLog = true;
    std::string login, password, status, buff_mess;
    char operation;
    char msg[MESSAGE_LENGTH];

    while (checkLog)
    {
        std::string check_login, check_password;
        bzero(msg, MESSAGE_LENGTH);
        buff_mess = "login:";
        strcpy(msg, buff_mess.c_str());
        sm_send(connection, msg);

        bzero(msg, MESSAGE_LENGTH);
        sm_get(connection, msg);
        check_login = msg;

        bzero(msg, MESSAGE_LENGTH);
        buff_mess = "password:";
        strcpy(msg, buff_mess.c_str());
        sm_send(connection, msg);

        bzero(msg, MESSAGE_LENGTH);
        sm_get(connection, msg);
        check_password = msg;

        for (const User& u : _users)
        {
            if (u.getUserLogin() == check_login && u.getUserPassword() == check_password)
            {
                status = "1";
                _database->changeStatusByUserDB(_admin_online, status);
                _user_online = check_login;
                _active_user[check_login] = true;
                _clientLogin = true;
                showUserMenuSM(connection);
                break;
            }
        }
        if (!_clientLogin)
        {
            bzero(msg, MESSAGE_LENGTH);
            buff_mess = "LOGIN OR PASSWORD FAILED\nPress any button to repeat";
            strcpy(msg, buff_mess.c_str());
            sm_send(connection, msg);

            bzero(msg, MESSAGE_LENGTH);
            sm_get(connection, msg);
            break;
        }
    }
}

void Chat::registrUserSM(int connection)
{
    std::string new_login, password, status, buff_mess;
    char msg[MESSAGE_LENGTH];
    bool flag = false;

    bzero(msg, MESSAGE_LENGTH);
    buff_mess = "new login: ";
    strcpy(msg, buff_mess.c_str());
    sm_send(connection, msg);

    bzero(msg, MESSAGE_LENGTH);
    sm_get(connection, msg);
    new_login = msg;

    bzero(msg, MESSAGE_LENGTH);
    buff_mess = "password: ";
    strcpy(msg, buff_mess.c_str());
    sm_send(connection, msg);

    bzero(msg, MESSAGE_LENGTH);
    sm_get(connection, msg);
    password = msg;

    for (const auto& u : _active_user)
    {
        if (u.first == new_login)
        {
            bzero(msg, MESSAGE_LENGTH);
            buff_mess = "This username is taken, please try another one\nPress any button to repeat";
            strcpy(msg, buff_mess.c_str());
            sm_send(connection, msg);
            flag = true;

            bzero(msg, MESSAGE_LENGTH);
            sm_get(connection, msg);
            break;
        }
    }
    if (!flag)
    {
        status = "0";
        _database->addUserDB(new_login, password, status);
        User user(new_login, password);
        _users.emplace_back(user);
        _active_user[new_login] = false;
    }
}

void Chat::showUserMenuSM(int connection)
{
    bool _menu_on = true;
    std::string login, password, status, buff_mess;
    char operation;
    char msg[MESSAGE_LENGTH];

    while (_menu_on)
    {
        bzero(msg, MESSAGE_LENGTH);
        buff_mess = "LOGIN SUCCESS\nMenu: (1) Chat Public | (2) Chat Private | (3) Users |(0) Logout\nHi!, " + _user_online + "\n>>";
        strcpy(msg, buff_mess.c_str());
        sm_send(connection, msg);

        bzero(msg, MESSAGE_LENGTH);
        sm_get(connection, msg);
        operation = msg[0];

        switch (operation)
        {
        case '1':
            showMenuChatPublicSM(connection);
            break;
        case '2':
            showMenuChatPrivateSM(connection);
            break;
        case '3':
            showUsersListSM(connection);
            break;
        case '0':
            status = "0";
            _database->changeStatusByUserDB(_admin_online, status);
            _active_user.at(_user_online) = false;
            _menu_on = false;
            checkLog = false;
            break;
        default:
            bzero(msg, MESSAGE_LENGTH);
            buff_mess = "Choose 1 or 2 or 3...\n--------------\nPress any button to repeat";
            strcpy(msg, buff_mess.c_str());
            sm_send(connection, msg);

            bzero(msg, MESSAGE_LENGTH);
            sm_get(connection, msg);
            break;
        }
    }
}

void Chat::showMenuChatPublicSM(int connection)
{
    bool chat_public_on = true;
    std::string buff_mess;
    char operation;
    char msg[MESSAGE_LENGTH];
    while (chat_public_on) {
        bzero(msg, MESSAGE_LENGTH);
        buff_mess = "--- Chat Public ---\nMenu:  (1) ShowChat | (2) Add Message | (0) Back\n>>";
        strcpy(msg, buff_mess.c_str());
        sm_send(connection, msg);

        bzero(msg, MESSAGE_LENGTH);
        sm_get(connection, msg);
        operation = msg[0];

        switch (operation)
        {
        case '1':

            showChatPublicSM(connection);
            break;
        case '2':
            addMessagePublicSM(connection);
            break;
        case '0':
            chat_public_on = false;
            break;
        default:
            bzero(msg, MESSAGE_LENGTH);
            buff_mess = "Choose 1 or 2...\n--------------\nPress any button to repeat";
            strcpy(msg, buff_mess.c_str());
            sm_send(connection, msg);

            bzero(msg, MESSAGE_LENGTH);
            sm_get(connection, msg);
            break;
        }
    }
}

void Chat::showMenuChatPrivateSM(int connection)
{
    bool chat_private_on = true;
    std::string buff_mess;
    char operation;
    char msg[MESSAGE_LENGTH];

    while (chat_private_on) {
        bzero(msg, MESSAGE_LENGTH);
        buff_mess = "--- Chat Private ---\nMenu:  (1) ShowChat | (2) Add Message | (0) Back\n>>";
        strcpy(msg, buff_mess.c_str());
        sm_send(connection, msg);

        bzero(msg, MESSAGE_LENGTH);
        sm_get(connection, msg);
        operation = msg[0];

        switch (operation)
        {
        case '1':
            showChatPrivateSM(connection);
            break;
        case '2':
            addMessagePrivateSM(connection);
            break;
        case '0':
            chat_private_on = false;
            break;
        default:
            bzero(msg, MESSAGE_LENGTH);
            buff_mess = "Choose 1 or 2...";
            strcpy(msg, buff_mess.c_str());
            sm_send(connection, msg);

            bzero(msg, MESSAGE_LENGTH);
            sm_get(connection, msg);
        }
    }
}

void Chat::showChatPublicSM(int connection)
{
    std::string buff_mess;
    char msg[MESSAGE_LENGTH];
    for (const auto& m : _messages)
    {
        if (m.getTo() == "All")
        {
            bzero(msg, MESSAGE_LENGTH);
            buff_mess += m.getTime() + " || From " + m.getFrom() + " To " + m.getTo() + " : " + m.getText() + "\n";
        }
    }
    buff_mess += "--------------\nPress any button to continue";
    strcpy(msg, buff_mess.c_str());
    sm_send(connection, msg);

    bzero(msg, MESSAGE_LENGTH);
    sm_get(connection, msg);
}

void Chat::addMessagePublicSM(int connection)
{
    bool go_chat_public = true;
    std::string buff_mess;
    char msg[MESSAGE_LENGTH];
    std::cout << "User " << _user_online << " has entered a public chat! Let's talk to him" << std::endl;
    bzero(msg, MESSAGE_LENGTH);
    buff_mess = "HI, " + _user_online + "\n(QQQ) - Exit\nPress any button to start";
    strcpy(msg, buff_mess.c_str());
    sm_send(connection, msg);

    bzero(msg, MESSAGE_LENGTH);
    sm_get(connection, msg);

    while (go_chat_public)
    {
        std::string from = _admin_online;
        std::string to = "All";
        std::string text;
        std::string time;
        std::getline(std::cin, text);
        if (text == "QQQ")
        {
            go_chat_public = false;
            break;
        }
        time = getDateTime();
        _database->addMessagePublicDB(from, text, time);
        Message message(from, to, text, time);
        _messages.emplace_back(message);

        bzero(msg, MESSAGE_LENGTH);
        buff_mess = time + " || " + from + " : " + text;
        strcpy(msg, buff_mess.c_str());
        sm_send(connection, msg);

        bzero(msg, MESSAGE_LENGTH);
        sm_get(connection, msg);
        from = _user_online;
        time = getDateTime();
        buff_mess = msg;
        std::cout << time << " || " << from << " : " << msg << std::endl;
        if (buff_mess == "QQQ")
        {
            go_chat_public = false;
            break;
        }
        _database->addMessagePublicDB(from, text, time);
        Message msg(from, to, buff_mess, time);
        _messages.emplace_back(msg);
    }
}

void Chat::showChatPrivateSM(int connection)
{
    std::string buff_mess;
    char msg[MESSAGE_LENGTH];
    for (const auto& m : _messages)
    {
        if (m.getTo() == _user_online || m.getFrom() == _user_online && m.getTo() != "All")
        {
            bzero(msg, MESSAGE_LENGTH);
            buff_mess += m.getTime() + " || From " + m.getFrom() + " To " + m.getTo() + " : " + m.getText() + "\n";
        }
    }
    buff_mess += "--------------\nPress any button to continue";
    strcpy(msg, buff_mess.c_str());
    sm_send(connection, msg);

    bzero(msg, MESSAGE_LENGTH);
    sm_get(connection, msg);
}

void Chat::addMessagePrivateSM(int connection) {

    std::string buff_mess, to, text, time;
    std::string from = _user_online;
    char msg[MESSAGE_LENGTH];
    int _found = 1;

    bzero(msg, MESSAGE_LENGTH);
    buff_mess = "To: ";
    strcpy(msg, buff_mess.c_str());
    sm_send(connection, msg);

    bzero(msg, MESSAGE_LENGTH);
    sm_get(connection, msg);
    to = msg;
    for (const auto& u : _active_user)
    {
        if (u.first == to)
        {
            _found = 0;
        }
    }
    if (_found == 1)
    {
        bzero(msg, MESSAGE_LENGTH);
        buff_mess = "User with this login does not exist\n--------------\nPress any button to continue";
        strcpy(msg, buff_mess.c_str());
        sm_send(connection, msg);

        bzero(msg, MESSAGE_LENGTH);
        sm_get(connection, msg);
        return;
    }
    else
    {
        bzero(msg, MESSAGE_LENGTH);
        buff_mess = "Text: ";
        strcpy(msg, buff_mess.c_str());
        sm_send(connection, msg);

        bzero(msg, MESSAGE_LENGTH);
        sm_get(connection, msg);
        text = msg;
        time = getDateTime();
        _database->addMessagePrivateDB(from, to, text, time);
        Message msg(from, to, text, time);
        _messages.emplace_back(msg);
    }
}

void Chat::showUsersListSM(int connection)
{
    std::string buff_mess;
    char msg[MESSAGE_LENGTH];
    bzero(msg, MESSAGE_LENGTH);
    std::string login;
    std::string password;
    for (const auto& u : _active_user)
    {
        if (u.second == true)
        {
            buff_mess += u.first + " online\n";
        }
        else
        {
            buff_mess += u.first + " offline\n";
        }
    }
    buff_mess += "--------------\nPress any button to continue";
    strcpy(msg, buff_mess.c_str());
    sm_send(connection, msg);

    bzero(msg, MESSAGE_LENGTH);
    sm_get(connection, msg);
}
#endif
