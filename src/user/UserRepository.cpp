//
// Created by w on 08.12.23.
//
#include "UserRepository.h"

UserRepository::UserRepository(const std::shared_ptr <DbConnPool> &dbConnPool) : dbConnPool(dbConnPool) {}

User UserRepository::FindByLogin(const std::string &login) {
    std::shared_ptr<pqxx::connection> conn;

    try {
        conn = dbConnPool->GetConnection();

        std::string sql_query = "SELECT * FROM Users WHERE login = '" + login + "'";

        pqxx::work tx{*conn};
        pqxx::result res = tx.exec(sql_query);

        dbConnPool->ReleaseConnection(conn);

        if (!res.empty()) {
            auto resLogin = res[0]["login"].as<std::string>();
            auto resPassword = res[0]["password"].as<std::string>();


            return User{resLogin, resPassword};
        }
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        dbConnPool->ReleaseConnection(conn);
    }

    return User{};
}

User UserRepository::Create(const User &user) {
    std::shared_ptr<pqxx::connection> conn;

    try {
        conn = dbConnPool->GetConnection();

        std::string sqlQuery = "INSERT INTO users (login, password) VALUES ('" + user.login + "', '" + user.password + "')";

        pqxx::work tx{*conn};
        tx.exec(sqlQuery);
        tx.commit();

        dbConnPool->ReleaseConnection(conn);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        dbConnPool->ReleaseConnection(conn);
    }

    return user;
}

std::vector<User> UserRepository::FindAllUsers() {
    std::vector<User> users;
    std::shared_ptr<pqxx::connection> conn;

    try {
        conn = dbConnPool->GetConnection();

        std::string sqlQuery = "SELECT login, password FROM users ORDER BY login";

        pqxx::work tx{*conn};

        for (auto [login, password] : tx.query<std::string, std::string>(
                sqlQuery
        )) {
            users.push_back({login, password});
        }

        dbConnPool->ReleaseConnection(conn);

    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        dbConnPool->ReleaseConnection(conn);
    }

    return users;
}
