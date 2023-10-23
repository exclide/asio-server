//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATSESSION_H
#define ASIO_SERVER_CHATSESSION_H

#include <queue>
#include "Asio.h"

class ChatRoom;

class ChatSession : public std::enable_shared_from_this<ChatSession> {
public:
    ChatSession(tcp::socket socket, const std::shared_ptr<ChatRoom>& room);
    ~ChatSession();

    void Start();
    void DoRead();
    void DoWrite(const std::shared_ptr<std::string>& msg);
    void Send(const std::shared_ptr<std::string>& msg);

private:
    tcp::socket socket;
    std::shared_ptr<ChatRoom> room;
    std::string data;
};

#endif //ASIO_SERVER_CHATSESSION_H
