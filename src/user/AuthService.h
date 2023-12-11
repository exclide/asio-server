//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_AUTHSERVICE_H
#define ASIO_SERVER_AUTHSERVICE_H

#include <memory>
#include "IUserRepository.h"
#include "../exceptions/ChatException.h"

class AuthService {
private:
    std::shared_ptr<IUserRepository> userRepository;

public:
    explicit AuthService(const std::shared_ptr<IUserRepository>& userRepository);
    AuthService(const AuthService& other) = delete;
    void operator=(const AuthService& other) = delete;

    User Login(User& user);
    User Register(User& user);
    std::vector<User> FindAllUsers();
};


#endif //ASIO_SERVER_AUTHSERVICE_H
