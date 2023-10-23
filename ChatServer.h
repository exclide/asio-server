//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATSERVER_H
#define ASIO_SERVER_CHATSERVER_H

#include "Asio.h"
#include "ChatSession.h"
#include "ChatRoom.h"

class ChatServer : public std::enable_shared_from_this<ChatServer> {
public:
    ChatServer(io_context& ioc, tcp::endpoint& endpoint);

    void StartAccept();

private:
    tcp::acceptor acceptor;
    std::shared_ptr<ChatRoom> room;
};

#endif //ASIO_SERVER_CHATSERVER_H
