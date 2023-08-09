#pragma once
#include <string>

class User
{
    //int _id;
    std::string _login;
    std::string _password;

public:
    User();
    User(const std::string& login, const std::string& password);

    const std::string getUserLogin() const;
    const std::string getUserPassword() const;

    ~User() = default;
};

