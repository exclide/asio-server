//
// Created by w on 08.12.23.
//
#include "HttpSession.h"
#include "HttpHandler.h"
#include "WebsocketSession.h"


HttpSession::HttpSession(
        tcp::socket&& socket,
        boost::asio::ssl::context& ctx,
        const std::shared_ptr<SharedState>& room) :
        stream(std::move(socket), ctx),
        room(room),
        httpHandler(room->GetAuthService()) {
    std::cout << "Http connection open\n";
}

HttpSession::~HttpSession() {
    std::cout << "Http connection closed\n";
}

void HttpSession::Start() {
    boost::asio::dispatch( //the first call won't be on strand, so dispatch to strand
            stream.get_executor(),
            [self = shared_from_this()]() {
                self->DoSslHandshake();
            });
}

void HttpSession::DoSslHandshake() {
    stream.async_handshake(
            boost::asio::ssl::stream_base::server,
            [self = shared_from_this()](error_code err){
                if (!err) {
                    std::cout << "Http: Accepted SSL handshake from client\n";
                    self->DoRead();
                } else if (err == boost::asio::error::eof) {
                    std::cerr << "Connection closed by peer\n";
                } else if (err == boost::asio::error::operation_aborted) {
                    std::cerr << "Operation aborted either thread exit or app request\n";
                } else {
                    std::cerr << "SSL handshake failed: " << err.message() << std::endl;
                }
            });
}

void HttpSession::DoRead() {
    req = {};

    http::async_read(
            stream,
            buffer,
            req,
            [self = shared_from_this()](error_code err, size_t) {
                if (!err) {
                    self->HandleHttpRequest();
                } else if (err == boost::asio::error::eof) {
                    std::cerr << "Connection closed by peer\n";
                    self->DoClose();
                } else if (err == boost::asio::error::operation_aborted) {
                    std::cerr << "Operation aborted either thread exit or app request\n";
                } else {
                    std::cerr << err.message() << std::endl;
                }
            });
}

void HttpSession::HandleHttpRequest() {
    std::cout << "Handle request called\n";

    if (websocket::is_upgrade(req)) {
        if (!req.base().count("Authorization")) return;
        std::string token = req.base().at("Authorization");
        std::cout << token << std::endl;

        static auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{httpHandler.GetJwtKey()})
                .with_issuer("wschat");

        std::string login;

        try {
            auto decoded = jwt::decode(token);
            login = decoded.get_payload_claim("login").as_string();
            std::cout << login << std::endl;

            verifier.verify(decoded);
        } catch (...) {
            std::cerr << "Wrong token received\n";
            return;
        }

        std::cout << "Is upgrade request\n";
        std::make_shared<WebsocketSession>(
                std::move(stream), room, login)->Start(req);
        return;
    }


    auto res = httpHandler.HandleRequest(std::move(req));

    beast::async_write(
            stream,
            std::move(res.first),
            [self = shared_from_this(), login = res.second](error_code err, size_t) {
                if (!err && login) {
                    std::cout << "LoginSuccess\n";
                }  else if (err == boost::asio::error::eof) {
                    std::cerr << "Connection closed by peer\n";
                    self->DoClose();
                } else if (err == boost::asio::error::operation_aborted) {
                    std::cerr << "Operation aborted either thread exit or app request\n";
                } else if (err) {
                    std::cerr << err.message() << std::endl;
                }
            }
    );

}

void HttpSession::DoClose() {
    stream.async_shutdown([self=shared_from_this()](error_code err) {
        if (err)
            std::cerr << err.message() << std::endl;
    });
}
