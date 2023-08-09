#include "User.h"

//Создание пользователя с заданными параметрами
User::User(const std::string& login, const std::string& password) : _login(login), _password(password)
{
}

//Возврат логина пользователя
const std::string User::getUserLogin() const
{
    return _login;
}

//Возврат пароля пользователя
const std::string User::getUserPassword() const
{
    return _password;
}
