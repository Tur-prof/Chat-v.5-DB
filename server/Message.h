#pragma once
#include <string>

class Message
{
    std::string _from;
    std::string _to;
    std::string _text;
    std::string _time;

public:
    Message();
    Message(const std::string& from, const std::string& to, const std::string& text, const std::string& _time);

    const std::string getFrom() const;
    const std::string getTo() const;
    const std::string getText() const;
    const std::string getTime() const;

    ~Message() = default;
};


