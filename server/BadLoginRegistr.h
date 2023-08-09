#pragma once
#include <exception>

//Не правильный логин при регистрации
class BadLoginRegistr : public std::exception
{
public:
	virtual const char* what() const noexcept override
	{
		return "\033[31m""This username is taken, please try another one";
	}
};