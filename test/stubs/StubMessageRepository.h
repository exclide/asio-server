//
// Created by w on 08.12.23.
//

#ifndef SERVER_STUBMESSAGEREPOSITORY_H
#define SERVER_STUBMESSAGEREPOSITORY_H

#include "../../src/message/IMessageRepository.h"

class StubMessageRepository : public IMessageRepository {
public:
    void Create(const DbMessage& msg) override;
    std::vector<DbMessage> FindAll() override;

private:
    std::vector<DbMessage> messages;
};

#endif //SERVER_STUBMESSAGEREPOSITORY_H
