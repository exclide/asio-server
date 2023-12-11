//
// Created by w on 08.12.23.
//
#include "StubMessageRepository.h"

void StubMessageRepository::Create(const DbMessage &msg) {
    messages.push_back(msg);
}

std::vector<DbMessage> StubMessageRepository::FindAll() {
    return messages;
}
