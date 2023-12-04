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
        db.emplace_back("asd1", "asd2", "asd1 to asd2", time(0));
        db.emplace_back("asd1", "asd3", "asd1 to asd3", time(0));
        db.emplace_back("asd4", "asd1", "asd4 to asd1", time(0));
    }

    void Create(const DbMessage& msg) {
        db.push_back(msg);
    }

    std::vector<DbMessage> FindAll() {
        return db;
    }

private:
    std::vector<DbMessage> db;
};

#endif //ASIO_SERVER_MESSAGEREPOSITORY_H
