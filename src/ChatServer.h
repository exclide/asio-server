//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATSERVER_H
#define ASIO_SERVER_CHATSERVER_H

#include "Asio.h"
#include "WebsocketSession.h"
#include "SharedState.h"

class ChatServer : public std::enable_shared_from_this<ChatServer> {
public:
    ChatServer(
            io_context& ioc,
            tcp::endpoint& endpoint,
            const std::shared_ptr<SharedState>& sharedState);

    void StartAccept();

private:
    tcp::acceptor acceptor;
    std::shared_ptr<SharedState> room;
    boost::asio::ssl::context sslContext;
};

#endif //ASIO_SERVER_CHATSERVER_H
