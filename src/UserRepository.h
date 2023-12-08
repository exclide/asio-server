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
#include "DbContext.h"
#include <pqxx/pqxx>

class UserRepository {
private:
    DbContext* dbContext = DbContext::GetInstance();
public:

    User FindByLogin(const std::string& login) {
        std::shared_ptr<pqxx::connection> conn;

        try {
            conn = dbContext->GetConnection();

            std::string sql_query = "SELECT * FROM Users WHERE login = '" + login + "'";

            pqxx::work tx{*conn};
            pqxx::result res = tx.exec(sql_query);

            dbContext->ReleaseConnection(conn);

            if (!res.empty()) {
                auto resLogin = res[0]["login"].as<std::string>();
                auto resPassword = res[0]["password"].as<std::string>();


                return User{resLogin, resPassword};
            }
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
            dbContext->ReleaseConnection(conn);
        }

        return User{};
    }

    User Create(const User& user) {
        std::shared_ptr<pqxx::connection> conn;

        try {
            conn = dbContext->GetConnection();

            std::string sqlQuery = "INSERT INTO users (login, password) VALUES ('" + user.login + "', '" + user.password + "')";

            pqxx::work tx{*conn};
            tx.exec(sqlQuery);
            tx.commit();

            dbContext->ReleaseConnection(conn);
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
            dbContext->ReleaseConnection(conn);
        }

        return user;
    }

    std::vector<User> FindAllUsers() {
        std::vector<User> users;
        std::shared_ptr<pqxx::connection> conn;

        try {
            conn = dbContext->GetConnection();

            std::string sqlQuery = "SELECT login, password FROM users ORDER BY login";

            pqxx::work tx{*conn};

            for (auto [login, password] : tx.query<std::string, std::string>(
                    sqlQuery
            )) {
                users.push_back({login, password});
            }

            dbContext->ReleaseConnection(conn);

        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
            dbContext->ReleaseConnection(conn);
        }

        return users;
    }
};

#endif //ASIO_SERVER_USERREPOSITORY_H
