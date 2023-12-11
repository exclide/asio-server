//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_USERREPOSITORY_H
#define ASIO_SERVER_USERREPOSITORY_H

#include <unordered_map>
#include <string>
#include <vector>
#include "../encryption/Sha256.h"
#include "User.h"
#include "../database/DbConnPool.h"
#include "IUserRepository.h"
#include <pqxx/pqxx>

class UserRepository : public IUserRepository {
private:
    std::shared_ptr<DbConnPool> dbConnPool;
public:
    explicit UserRepository(const std::shared_ptr<DbConnPool>& dbConnPool);

    User FindByLogin(const std::string& login) override;
    User Create(const User& user) override;
    std::vector<User> FindAllUsers() override;
};

#endif //ASIO_SERVER_USERREPOSITORY_H
