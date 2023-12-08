//
// Created by w on 08.12.23.
//

#include "DbConnPool.h"

DbConnPool::DbConnPool(const std::string &connectionString, int connections) {
    InitPool(connectionString, connections);
}

std::shared_ptr<pqxx::connection> DbConnPool::GetConnection() {
    std::lock_guard lock(m);

    if (connectionPool.empty()) {
        throw ChatException("Pool out of database connections");
    }

    auto connection = connectionPool.top();
    connectionPool.pop();

    return connection;
}

void DbConnPool::ReleaseConnection(const std::shared_ptr<pqxx::connection> &connection) {
    std::lock_guard lock(m);

    connectionPool.push(connection);
}

void DbConnPool::InitPool(const std::string &connectionString, int connections) {
    for (int i = 0; i < connections; i++) {
        connectionPool.push(std::make_shared<pqxx::connection>(connectionString));
        connectionPool.top()->prepare(
                "InsertMessage",
                "INSERT INTO messages (receiver, sender, text, date) VALUES ($1, $2, $3, $4)");
    }
}
