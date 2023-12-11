//
// Created by w on 04.12.23.
//

#include "MessageService.h"


MessageService::MessageService(const std::shared_ptr<IMessageRepository>& messageRepository)
        : messageRepository(messageRepository) {}

void MessageService::AddMessage(const DbMessage &msg) {
    messageRepository->Create(msg);
}

std::vector<DbMessage> MessageService::FindAll() {
    return messageRepository->FindAll();
}

std::vector<DbMessage> MessageService::FindAllForLogin(const std::string &login) {
    auto db = messageRepository->FindAll();
    std::vector<DbMessage> res;

    for (auto& msg : db) {
        if (msg.sender == login || msg.receiver == login) {
            res.push_back(msg);
        }
    }

    return res;
}
