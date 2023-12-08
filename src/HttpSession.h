//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_HTTPSESSION_H
#define ASIO_SERVER_HTTPSESSION_H

#include "Asio.h"
#include "AuthService.h"
#include "HttpHandler.h"
#include "ChatRoom.h"


class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
    HttpSession(
            tcp::socket&& socket,
            boost::asio::ssl::context& ctx,
            const std::shared_ptr<ChatRoom>& room);

    ~HttpSession();

    void Start();
    void DoSslHandshake();
    void DoRead();
    void HandleHttpRequest();
    void DoClose();


private:
    beast::ssl_stream<beast::tcp_stream> stream;
    beast::flat_buffer buffer;
    http::request<http::string_body> req;
    std::shared_ptr<ChatRoom> room;

    HttpHandler httpHandler;
};

#endif //ASIO_SERVER_HTTPSESSION_H
