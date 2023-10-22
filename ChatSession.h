//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATSESSION_H
#define ASIO_SERVER_CHATSESSION_H

#include "Asio.h"

class ChatRoom;

class ChatSession : public std::enable_shared_from_this<ChatSession> {
public:
    ChatSession(tcp::socket socket, const std::shared_ptr<ChatRoom>& room);

    void Start();

    void DoRead();

    void DoWrite(std::size_t bytes);

    void Send(const std::string& msg);

private:
    tcp::socket socket;
    boost::asio::streambuf buf;
    std::shared_ptr<ChatRoom> room;
};

#endif //ASIO_SERVER_CHATSESSION_H
