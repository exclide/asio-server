//
// Created by w on 08.12.23.
//

#ifndef SERVER_PARSEEXCEPTION_H
#define SERVER_PARSEEXCEPTION_H

#include <stdexcept>
#include <string>

class ParseException : public std::runtime_error {
public:
    explicit ParseException(const std::string& msg) : std::runtime_error(msg) {}
};

#endif //SERVER_PARSEEXCEPTION_H
