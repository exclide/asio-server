//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_USER_H
#define ASIO_SERVER_USER_H

#include <string>
#include "../network/Asio.h"

struct User {
    std::string login;
    std::string password;

    bool operator==(const User& other) const {
        return this->login == other.login && this->password == other.password;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, login, password);
};



#endif //ASIO_SERVER_USER_H
