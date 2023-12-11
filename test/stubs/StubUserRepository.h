//
// Created by w on 08.12.23.
//

#ifndef SERVER_STUBUSERREPOSITORY_H
#define SERVER_STUBUSERREPOSITORY_H

#include "../../src/user/IUserRepository.h"

class StubUserRepository : public IUserRepository {
public:
    User FindByLogin(const std::string& login) override;
    User Create(const User& user) override;
    std::vector<User> FindAllUsers() override;

private:
    std::vector<User> users;
};

#endif //SERVER_STUBUSERREPOSITORY_H
