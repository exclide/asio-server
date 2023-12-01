//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATSESSION_H
#define ASIO_SERVER_CHATSESSION_H

#include <queue>
#include "Asio.h"

class ChatRoom;


class ChatSession : public std::enable_shared_from_this<ChatSession> {
public:
    ChatSession(tcp::socket&& socket, boost::asio::ssl::context& ctx, const std::shared_ptr<ChatRoom>& room);
    ~ChatSession();

    void Start();
    void DoRead();
    void DoWrite();
    void DoSslHandshake();
    void DoWebsocketHandshake();
    void Send(const std::shared_ptr<std::string>& msg);

private:
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws;
    beast::flat_buffer buffer;

    std::shared_ptr<ChatRoom> room;
    std::queue<std::shared_ptr<std::string>> sendq;
};

#endif //ASIO_SERVER_CHATSESSION_H
