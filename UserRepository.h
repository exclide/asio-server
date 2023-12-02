//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_USERREPOSITORY_H
#define ASIO_SERVER_USERREPOSITORY_H

#include <unordered_map>
#include <string>
#include <vector>
#include "sha256.h"
#include "User.h"

class UserRepository {
public:
    User FindByLogin(const std::string& login) {
        if (db.find(login) == db.end()) {
            return User{};
        }

        return db[login];
    }

    void Create(const User& user) {
        if (db.find(user.login) == db.end()) {
            db[user.login] = User{user.login, user.password};
        }
    }

    std::vector<User> FindAllUsers() {
        std::vector<User> users;

        for (auto [_, user] : db) {
            users.push_back(user);
        }

        return users;
    }

private:
    std::unordered_map<std::string, User> db;
};

#endif //ASIO_SERVER_USERREPOSITORY_H
