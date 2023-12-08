//
// Created by w on 04.12.23.
//

#ifndef ASIO_SERVER_MESSAGEREPOSITORY_H
#define ASIO_SERVER_MESSAGEREPOSITORY_H

#include <unordered_map>
#include <pqxx/pqxx>
#include "DbMessage.h"
#include "DbConnPool.h"

class MessageRepository {
private:
    std::shared_ptr<DbConnPool> dbConnPool;
public:
    explicit MessageRepository(const std::shared_ptr<DbConnPool>& dbConnPool);

    void Create(const DbMessage& msg);
    std::vector<DbMessage> FindAll();

private:

};

#endif //ASIO_SERVER_MESSAGEREPOSITORY_H
