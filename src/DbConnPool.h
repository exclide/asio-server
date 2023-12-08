//
// Created by w on 08.12.23.
//

#ifndef SERVER_DBCONNPOOL_H
#define SERVER_DBCONNPOOL_H

#include <string>
#include <stack>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include "ChatException.h"

class DbConnPool {
public:
    explicit DbConnPool(const std::string& connectionString, int connections);

    DbConnPool(const DbConnPool& other) = delete;
    void operator=(const DbConnPool& other) = delete;
    ~DbConnPool();

    std::shared_ptr<pqxx::connection> GetConnection();
    void ReleaseConnection(const std::shared_ptr<pqxx::connection>& connection);
    void CloseConnections();

private:
    void InitPool(const std::string& connectionString, int connections);

    std::mutex m;
    std::stack<std::shared_ptr<pqxx::connection>> connectionPool;
};

#endif //SERVER_DBCONNPOOL_H
