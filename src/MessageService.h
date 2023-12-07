//
// Created by w on 04.12.23.
//

#ifndef ASIO_SERVER_MESSAGESERVICE_H
#define ASIO_SERVER_MESSAGESERVICE_H

#include <memory>
#include "MessageRepository.h"

class MessageService {
private:
    explicit MessageService(MessageRepository* messageRepository) : messageRepository(messageRepository) {}
    MessageRepository* messageRepository;
    static MessageService* messageService;

public:
    ~MessageService() = delete;
    MessageService(const MessageService& other) = delete;
    void operator=(const MessageService& other) = delete;

    static MessageService* GetInstance() {
        if (messageService == nullptr) {
            messageService = new MessageService(new MessageRepository);
        }

        return messageService;
    }

    void AddMessage(const DbMessage& msg) {
        messageRepository->Create(msg);
    }

    std::vector<DbMessage> FindAll() {
        return messageRepository->FindAll();
    }

    std::vector<DbMessage> FindAllForLogin(const std::string& login) {
        auto db = messageRepository->FindAll();
        std::vector<DbMessage> res;

        for (auto& msg : db) {
            if (msg.sender == login || msg.receiver == login) {
                res.push_back(msg);
            }
        }

        return res;
    }
};


#endif //ASIO_SERVER_MESSAGESERVICE_H
