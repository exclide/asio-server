//
// Created by asd on 23-Oct-23.
//

#include "ChatRoom.h"
#include "WebsocketSession.h"

void ChatRoom::Join(const std::weak_ptr<WebsocketSession>& session) {
    std::lock_guard lock(m);
    sessions.push_back(session);
}

void ChatRoom::Leave(const std::weak_ptr<WebsocketSession>& session) {
    std::lock_guard lock(m);
    std::erase_if(sessions, [&session](auto& s)  {
        return s.lock() == session.lock();
    });
}

void ChatRoom::Send(const std::string& msg) {
    auto ss = std::make_shared<std::string>(msg);

    std::lock_guard lock(m);

    for (auto& wp : sessions) {
        if (auto ptr = wp.lock())
            ptr->Send(ss);
    }
}
