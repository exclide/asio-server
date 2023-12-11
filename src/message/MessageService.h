//
// Created by w on 04.12.23.
//

#ifndef ASIO_SERVER_MESSAGESERVICE_H
#define ASIO_SERVER_MESSAGESERVICE_H

#include <memory>
#include "IMessageRepository.h"

class MessageService {
private:
    std::shared_ptr<IMessageRepository> messageRepository;

public:
    explicit MessageService(const std::shared_ptr<IMessageRepository>& messageRepository);
    MessageService(const MessageService& other) = delete;
    void operator=(const MessageService& other) = delete;

    void AddMessage(const DbMessage& msg);
    std::vector<DbMessage> FindAll();
    std::vector<DbMessage> FindAllForLogin(const std::string& login);
};


#endif //ASIO_SERVER_MESSAGESERVICE_H
