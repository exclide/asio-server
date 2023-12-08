//
// Created by asd on 23-Oct-23.
//

#include "ChatServer.h"
#include "HttpSession.h"

ChatServer::ChatServer(
        io_context& ioc,
        tcp::endpoint& endpoint,
        const std::shared_ptr<SharedState>& sharedState)
        : acceptor(ioc, endpoint),
          sslContext(boost::asio::ssl::context::tlsv13_server),
          room(sharedState) {

    sslContext.set_options(
            boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use);

    sslContext.use_certificate_chain_file("ssl/user.crt");
    sslContext.use_private_key_file("ssl/user.key", boost::asio::ssl::context::pem);
    sslContext.use_tmp_dh_file("ssl/dh2048.pem");
}

void ChatServer::StartAccept() {
    acceptor.async_accept(
            boost::asio::make_strand(acceptor.get_executor()), //separate strand for every connection
            [self = shared_from_this()] (error_code err, tcp::socket socket) {
                if (!err) {
                    std::make_shared<HttpSession>(std::move(socket), self->sslContext,  self->room)->Start();
                } else if (err == boost::asio::error::operation_aborted) {
                    std::cerr << "Operation aborted either thread exit or app request\n";
                } else {
                    std::cerr << err.message() << std::endl;
                }

                self->StartAccept();
            });
}
