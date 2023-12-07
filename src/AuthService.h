//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_AUTHSERVICE_H
#define ASIO_SERVER_AUTHSERVICE_H

#include <memory>
#include "UserRepository.h"
#include "ChatException.h"

class AuthService {
private:
    explicit AuthService(UserRepository* userRepository) : userRepository(userRepository) {}
    UserRepository* userRepository;
    static AuthService* authService;

public:
    ~AuthService() = delete;
    AuthService(const AuthService& other) = delete;
    void operator=(const AuthService& other) = delete;

    static AuthService* GetInstance() {
        if (authService == nullptr) {
            authService = new AuthService(new UserRepository);
        }

        return authService;
    }

    User Login(User& user) {
        if (user.login.empty() || user.password.empty()) {
            throw ChatException("Empty login or password strings");
        }

        User dbUser = userRepository->FindByLogin(user.login);
        user.password = Sha256(user.password);

        return user == dbUser ? user : User{};
    }

    User Register(User& user) {
        if (user.login.empty() || user.password.empty()) {
            throw ChatException("Empty login or password strings");
        }

        user.password = Sha256(user.password);
        return userRepository->Create(user);
    }

    std::vector<User> FindAllUsers() {
        return userRepository->FindAllUsers();
    }
};


#endif //ASIO_SERVER_AUTHSERVICE_H
