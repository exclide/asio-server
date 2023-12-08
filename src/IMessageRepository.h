//
// Created by w on 08.12.23.
//

#ifndef SERVER_IMESSAGEREPOSITORY_H
#define SERVER_IMESSAGEREPOSITORY_H

#include <vector>
#include "DbMessage.h"

class IMessageRepository {
public:
    virtual void Create(const DbMessage& msg) = 0;
    virtual std::vector<DbMessage> FindAll() = 0;
};

#endif //SERVER_IMESSAGEREPOSITORY_H
