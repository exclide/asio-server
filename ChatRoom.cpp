//
// Created by asd on 23-Oct-23.
//

#include "ChatRoom.h"
#include "ChatSession.h"

void ChatRoom::Join(const std::shared_ptr <ChatSession> &session) {
    std::lock_guard lock(m);
    sessions.insert(session);
}

void ChatRoom::Leave(const std::shared_ptr<ChatSession> &session) {
    std::lock_guard lock(m);
    sessions.erase(session);
}

void ChatRoom::Send(const std::string& msg) {
    for (auto& sh : sessions) {
        sh->Send(msg);
    }
}
