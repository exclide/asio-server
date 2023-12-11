//
// Created by w on 08.12.23.
//
#include "StubUserRepository.h"

User StubUserRepository::FindByLogin(const std::string &login) {
    for (auto& user : users) {
        if (user.login == login) {
            return user;
        }
    }

    return User{};
}

User StubUserRepository::Create(const User &user) {
    users.push_back(user);

    return user;
}

std::vector<User> StubUserRepository::FindAllUsers() {
    return users;
}
