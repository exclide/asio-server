//
// Created by asd on 23-Oct-23.
//

#include "ChatServer.h"

ChatServer::ChatServer(io_context &ioc, tcp::endpoint &endpoint)
        : acceptor(ioc, endpoint),
          sslContext(boost::asio::ssl::context::sslv23) {
    room = std::make_shared<ChatRoom>();

    sslContext.set_options(
            boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use);

    sslContext.use_certificate_chain_file("../ssl/user.crt");
    sslContext.use_private_key_file("../ssl/user.key", boost::asio::ssl::context::pem);
    sslContext.use_tmp_dh_file("../ssl/dh2048.pem");
}

void ChatServer::StartAccept() {
    acceptor.async_accept(
            boost::asio::make_strand(acceptor.get_executor()), //separate strand for every connection
            [self = shared_from_this()] (error_code err, tcp::socket socket) {
                if (!err) {
                    std::make_shared<ChatSession>(
                            boost::asio::ssl::stream<tcp::socket>(std::move(socket), self->sslContext), self->room)->Start();
                }

                self->StartAccept();
            });
}
