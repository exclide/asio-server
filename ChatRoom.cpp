//
// Created by asd on 23-Oct-23.
//

#include "ChatRoom.h"
#include "ChatSession.h"

void ChatRoom::Join(ChatSession* session) {
    std::lock_guard lock(m);
    sessions.insert(session);
}

void ChatRoom::Leave(ChatSession* session) {
    std::lock_guard lock(m);
    sessions.erase(session);
}

void ChatRoom::Send(const std::string& msg) {
    auto ss = std::make_shared<std::string>(msg);

    std::vector<std::weak_ptr<ChatSession>> v;
    {
        std::lock_guard lock(m);
        for (auto s : sessions) {
            v.emplace_back(s->weak_from_this());
        }
    }

    for (auto& wp : v) {
        if (auto ptr = wp.lock())
            ptr->Send(ss);
    }
}
