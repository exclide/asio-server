//
// Created by w on 04.12.23.
//

#include "AuthService.h"
#include "Sha256.h"

AuthService::AuthService(const std::shared_ptr<IUserRepository>& userRepository) : userRepository(userRepository) {}

User AuthService::Login(User &user) {
    if (user.login.empty() || user.password.empty()) {
        throw ChatException("Empty login or password strings");
    }

    User dbUser = userRepository->FindByLogin(user.login);
    user.password = Sha256(user.password);

    return user == dbUser ? user : User{};
}

User AuthService::Register(User &user) {
    if (user.login.empty() || user.password.empty()) {
        throw ChatException("Empty login or password strings");
    }

    user.password = Sha256(user.password);
    return userRepository->Create(user);
}

std::vector<User> AuthService::FindAllUsers() {
    return userRepository->FindAllUsers();
}
