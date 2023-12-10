//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_WEBSOCKETSESSION_H
#define ASIO_SERVER_WEBSOCKETSESSION_H

#include <queue>
#include "Asio.h"
#include "SharedState.h"
#include "AuthService.h"
#include "MessageService.h"


class WebsocketSession : public std::enable_shared_from_this<WebsocketSession> {
public:
    WebsocketSession(beast::ssl_stream<beast::tcp_stream>&& stream,
                     const std::shared_ptr<SharedState>& room,
                     std::string login);
    ~WebsocketSession();

    template<class Body, class Allocator>
    void Start(http::request<Body, http::basic_fields<Allocator>> req);
    void Send(const std::shared_ptr<std::string>& msg);

private:
    void DoRead();
    void DoWrite();
    template<class Body, class Allocator>
    void DoWebsocketHandshake(http::request<Body, http::basic_fields<Allocator>> req);
    void DoClose();
    void DoPing();

    void Fail(error_code err, char const* what);
    std::shared_ptr<std::string> FormFirstMessage();

    boost::asio::steady_timer pingTimer;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws;
    beast::flat_buffer buffer;

    std::shared_ptr<SharedState> room;
    std::queue<std::shared_ptr<std::string>> sendq;
    std::string login;

    bool pongReceived = true;
    size_t pingRespondTimeSecs = 15;
};

template<class Body, class Allocator>
void WebsocketSession::Start(http::request<Body, http::basic_fields<Allocator>> req) {
    ws.next_layer().async_handshake(
            boost::asio::ssl::stream_base::server,
            [self = shared_from_this(), req](error_code err) {
                if (err) return self->Fail(err, "SSL handshake failed");

                std::cout << "Accepted SSL handshake from client\n";
                self->DoWebsocketHandshake(req);
            });
}

template<class Body, class Allocator>
void WebsocketSession::DoWebsocketHandshake(http::request<Body, http::basic_fields<Allocator>> req) {
    // Set suggested timeout settings for the websocket
    ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        "websocket-chat");
            }));

    // Accept the websocket handshake
    ws.async_accept(
            req,
            [this, self = shared_from_this()](error_code err) {
                if (err) return Fail(err, "Websocket handshake failed");

                std::cout << "Accepted websocket handshake from client\n";

                ws.control_callback(
                        [this](auto kind, auto payload) {
                            if (kind == websocket::frame_type::pong) {
                                std::cout << "Received pong frame from peer\n";
                                pongReceived = true;
                            }
                        });

                DoPing();

                room->Join(login, weak_from_this());

                auto ss = FormFirstMessage();
                Send(ss);

                DoRead();
            });
}

#endif //ASIO_SERVER_WEBSOCKETSESSION_H
