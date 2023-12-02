//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_USER_H
#define ASIO_SERVER_USER_H

#include <string>
#include "Asio.h"

struct User {
    std::string login;
    std::string password;

    bool operator==(const User& other) const {
        return this->login == other.login && this->password == other.password;
    }
};

void to_json(json& j, const User& user) {
    j = json{{"login", user.login}, {"password", user.password}};
}

void from_json(const json& j, User& user) {
    j.at("login").get_to(user.login);
    j.at("password").get_to(user.password);
}

#endif //ASIO_SERVER_USER_H
