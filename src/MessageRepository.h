//
// Created by w on 04.12.23.
//

#ifndef ASIO_SERVER_MESSAGEREPOSITORY_H
#define ASIO_SERVER_MESSAGEREPOSITORY_H

#include <unordered_map>
#include <pqxx/pqxx>
#include "DbMessage.h"
#include "DbConnPool.h"
#include "IMessageRepository.h"

class MessageRepository : public IMessageRepository {
private:
    std::shared_ptr<DbConnPool> dbConnPool;
public:
    explicit MessageRepository(const std::shared_ptr<DbConnPool>& dbConnPool);

    void Create(const DbMessage& msg) override;
    std::vector<DbMessage> FindAll() override;

private:

};

#endif //ASIO_SERVER_MESSAGEREPOSITORY_H
