//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATROOM_H
#define ASIO_SERVER_CHATROOM_H

#include <vector>
#include <memory>

class WebsocketSession;

class ChatRoom {
public:
    void Join(const std::weak_ptr<WebsocketSession>& session);
    void Leave(const std::weak_ptr<WebsocketSession>& session);
    void Send(const std::string& msg);

private:
    std::mutex m;
    std::vector<std::weak_ptr<WebsocketSession>> sessions;
};

#endif //ASIO_SERVER_CHATROOM_H
