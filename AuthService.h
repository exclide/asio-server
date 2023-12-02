//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_AUTHSERVICE_H
#define ASIO_SERVER_AUTHSERVICE_H

#include <memory>
#include "UserRepository.h"

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
        User dbUser = userRepository->FindByLogin(user.login);
        user.password = sha256(user.password);

        return user == dbUser ? user : User{};
    }

    User Register(User& user) {
        user.password = sha256(user.password);
        return userRepository->Create(user);
    }

    std::vector<User> FindAllUsers() {
        return userRepository->FindAllUsers();
    }
};

AuthService* AuthService::authService = nullptr;

#endif //ASIO_SERVER_AUTHSERVICE_H
