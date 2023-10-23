//
// Created by asd on 23-Oct-23.
//

#include "ChatServer.h"

ChatServer::ChatServer(io_context &ioc, tcp::endpoint &endpoint) : acceptor(ioc, endpoint) {
    room = std::make_shared<ChatRoom>();
}

void ChatServer::StartAccept() {
    acceptor.async_accept(
            boost::asio::make_strand(acceptor.get_executor()), //separate strand for every connection
            [self = shared_from_this()] (error_code err, tcp::socket socket) {
                if (!err) {
                    std::cout << "Accepted connection from: " << socket.remote_endpoint() << std::endl;
                    std::make_shared<ChatSession>(std::move(socket), self->room)->Start();
                }

                self->StartAccept();
            });
}
