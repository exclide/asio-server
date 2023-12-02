//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATSESSION_H
#define ASIO_SERVER_CHATSESSION_H

#include <queue>
#include "Asio.h"
#include "ChatRoom.h"


class ChatSession : public std::enable_shared_from_this<ChatSession> {
public:
    ChatSession(beast::ssl_stream<beast::tcp_stream>&& stream, const std::shared_ptr<ChatRoom>& room);
    ~ChatSession();

    void DoRead();
    void DoWrite();
    template<class Body, class Allocator>
    void DoWebsocketHandshake(http::request<Body, http::basic_fields<Allocator>> req);
    template<class Body, class Allocator>
    void Start(http::request<Body, http::basic_fields<Allocator>> req);
    void Send(const std::shared_ptr<std::string>& msg);

private:
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws;
    beast::flat_buffer buffer;

    std::shared_ptr<ChatRoom> room;
    std::queue<std::shared_ptr<std::string>> sendq;
};

template<class Body, class Allocator>
void ChatSession::Start(http::request<Body, http::basic_fields<Allocator>> req) {
    ws.next_layer().async_handshake(
            boost::asio::ssl::stream_base::server,
            [self = shared_from_this(), req](error_code err){
                if (!err) {
                    std::cout << "Accepted SSL handshake from client\n";
                    self->DoWebsocketHandshake(req);
                } else {
                    std::cout << "SSL handshake failed\n";
                }
            });
}

template<class Body, class Allocator>
void ChatSession::DoWebsocketHandshake(http::request<Body, http::basic_fields<Allocator>> req) {
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
            [self = shared_from_this()](error_code err) {
                if (!err) {
                    std::cout << "Accepted websocket handshake from client\n";
                    self->room->Join(self->weak_from_this());
                    self->DoRead();
                } else {
                    std::cout << "Websocket handshake failed: " << err.message() << std::endl;
                }
            });
}

#endif //ASIO_SERVER_CHATSESSION_H
