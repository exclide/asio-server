//
// Created by w on 04.12.23.
//

#ifndef ASIO_SERVER_MESSAGEREPOSITORY_H
#define ASIO_SERVER_MESSAGEREPOSITORY_H

#include <unordered_map>
#include "DbMessage.h"

class MessageRepository {
public:
    MessageRepository() {

    }

    DbMessage Create(const DbMessage& msg) {
        db.push_back(msg);
    }

    std::vector<DbMessage> FindAll() {
        return db;
    }

private:
    std::vector<DbMessage> db;
};

#endif //ASIO_SERVER_MESSAGEREPOSITORY_H
