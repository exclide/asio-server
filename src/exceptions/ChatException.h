//
// Created by w on 08.12.23.
//

#ifndef SERVER_CHATEXCEPTION_H
#define SERVER_CHATEXCEPTION_H

#include <stdexcept>

class ChatException : public std::runtime_error {
public:
    ChatException(const std::string& msg) : std::runtime_error(msg) {}
};

#endif //SERVER_CHATEXCEPTION_H
