//
// Created by asd on 23-Oct-23.
//

#include "ChatServer.h"

ChatServer::ChatServer(io_context &ioc, tcp::endpoint &endpoint) : acceptor(ioc, endpoint) {
    room = std::make_shared<ChatRoom>();
    StartAccept();
}

void ChatServer::StartAccept() {
    std::cout << "async accept call\n";

    acceptor.async_accept(
            boost::asio::make_strand(acceptor.get_executor()), //отдельный стрэнд для каждого соединения
            [this] (error_code err, tcp::socket socket) {
                if (!err) {
                    std::make_shared<ChatSession>(std::move(socket), room)->Start();
                }

                StartAccept();
            });

    std::cout << "async accept call exit\n";
}
