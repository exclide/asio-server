//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATSERVER_H
#define ASIO_SERVER_CHATSERVER_H

#include "Asio.h"
#include "ChatSession.h"

class ChatServer {
public:
    ChatServer(io_context& ioc, tcp::endpoint& endpoint) : acceptor(ioc, endpoint) {
        StartAccept();
    }


    void StartAccept() {
        acceptor.async_accept(
                [this] (error_code err, tcp::socket socket) {
                    if (!err) {
                        std::make_shared<ChatSession>(std::move(socket))->Start();
                    }

                    StartAccept();
                });
    }

private:
    tcp::acceptor acceptor;
};

#endif //ASIO_SERVER_CHATSERVER_H
