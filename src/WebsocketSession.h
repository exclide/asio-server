//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_WEBSOCKETSESSION_H
#define ASIO_SERVER_WEBSOCKETSESSION_H

#include <queue>
#include "Asio.h"
#include "ChatRoom.h"
#include "AuthService.h"
#include "MessageService.h"


class WebsocketSession : public std::enable_shared_from_this<WebsocketSession> {
public:
    WebsocketSession(beast::ssl_stream<beast::tcp_stream>&& stream,
                     const std::shared_ptr<ChatRoom>& room,
                     std::string login);
    ~WebsocketSession();

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

    std::string login;
};

template<class Body, class Allocator>
void WebsocketSession::Start(http::request<Body, http::basic_fields<Allocator>> req) {
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
            [self = shared_from_this()](error_code err) {
                if (!err) {
                    std::cout << "Accepted websocket handshake from client\n";
                    self->room->Join(self->login, self->weak_from_this());
                    //send the needed data
                    auto authService = AuthService::GetInstance();
                    auto msgService = MessageService::GetInstance();

                    auto users = authService->FindAllUsers();
                    for (auto& u : users) u.password = "";
                    auto messages = msgService->FindAllForLogin(self->login);
                    json j;

                    j["users"] = users;
                    j["messages"] = messages;
                    //j.push_back(users);
                    //j.push_back(messages);

                    std::string jsonStr = nlohmann::to_string(j);
                    auto ss = std::make_shared<std::string>(jsonStr);
                    std::cout << jsonStr << std::endl;

                    self->Send(ss);


                    self->DoRead();
                } else {
                    std::cout << "Websocket handshake failed: " << err.message() << std::endl;
                }
            });
}

#endif //ASIO_SERVER_WEBSOCKETSESSION_H
