//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_USERREPOSITORY_H
#define ASIO_SERVER_USERREPOSITORY_H

#include <unordered_map>
#include <string>
#include <vector>
#include "Sha256.h"
#include "User.h"
#include "DbConnPool.h"
#include <pqxx/pqxx>

class UserRepository {
private:
    std::shared_ptr<DbConnPool> dbConnPool;
public:
    explicit UserRepository(const std::shared_ptr<DbConnPool>& dbConnPool);

    User FindByLogin(const std::string& login);
    User Create(const User& user);
    std::vector<User> FindAllUsers();
};

#endif //ASIO_SERVER_USERREPOSITORY_H
