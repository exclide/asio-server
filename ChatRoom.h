//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATROOM_H
#define ASIO_SERVER_CHATROOM_H

#include <vector>
#include <memory>

class ChatSession;

class ChatRoom {
public:
    void Join(const std::weak_ptr<ChatSession>& session);
    void Leave(const std::weak_ptr<ChatSession>& session);
    void Send(const std::string& msg);

private:
    std::mutex m;
    std::vector<std::weak_ptr<ChatSession>> sessions;
};

#endif //ASIO_SERVER_CHATROOM_H
