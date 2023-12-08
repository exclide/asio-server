//
// Created by w on 08.12.23.
//

#ifndef SERVER_DBCONTEXT_H
#define SERVER_DBCONTEXT_H

#include <string>
#include <stack>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include "ChatException.h"

class DbContext {
public:
    explicit DbContext(const std::string& connectionString, int connections) {
        InitPool(connectionString, connections);
    }

    DbContext(const DbContext& other) = delete;
    void operator=(const DbContext& other) = delete;

    static void Init(const std::string& connectionString, int connections) {
        dbContext = new DbContext(connectionString, connections);
    }

    static DbContext* GetInstance() {
        if (!dbContext) {
            throw ChatException("DbContext was nullptr");
        }
        return dbContext;
    }

    auto GetConnection() {
        std::lock_guard lock(m);

        if (connectionPool.empty()) {
            throw ChatException("Pool out of database connections");
        }

        auto connection = connectionPool.top();
        connectionPool.pop();

        return connection;
    }

    void ReleaseConnection(const std::shared_ptr<pqxx::connection>& connection) {
        std::lock_guard lock(m);

        connectionPool.push(connection);
    }

private:
    void InitPool(const std::string& connectionString, int connections) {
        for (int i = 0; i < connections; i++) {
            connectionPool.push(std::make_shared<pqxx::connection>(connectionString));
        }
    }


    std::mutex m;
    std::stack<std::shared_ptr<pqxx::connection>> connectionPool;
    static DbContext* dbContext;
};

#endif //SERVER_DBCONTEXT_H
