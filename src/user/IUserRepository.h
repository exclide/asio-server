//
// Created by w on 08.12.23.
//

#ifndef SERVER_IUSERREPOSITORY_H
#define SERVER_IUSERREPOSITORY_H

#include <string>
#include "User.h"

class IUserRepository {
public:
    virtual User FindByLogin(const std::string& login) = 0;
    virtual User Create(const User& user) = 0;
    virtual std::vector<User> FindAllUsers() = 0;
};

#endif //SERVER_IUSERREPOSITORY_H
