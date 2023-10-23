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
    void Join(ChatSession* session);

    void Leave(ChatSession* session);

    void Send(const std::string& msg);

private:
    std::mutex m;
    std::unordered_set<ChatSession*> sessions;
};

#endif //ASIO_SERVER_CHATROOM_H
