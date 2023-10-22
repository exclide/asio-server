//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATROOM_H
#define ASIO_SERVER_CHATROOM_H

#include <unordered_set>
#include <memory>

class ChatSession;

class ChatRoom {
public:
    void Join(const std::shared_ptr<ChatSession>& session);

    void Leave(const std::shared_ptr<ChatSession>& session);

    void Send(const std::string& message);

private:
    std::mutex m;
    std::unordered_set<std::shared_ptr<ChatSession>> sessions;
};

#endif //ASIO_SERVER_CHATROOM_H
