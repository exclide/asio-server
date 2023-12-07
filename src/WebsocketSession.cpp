//
// Created by asd on 23-Oct-23.
//

#include "WebsocketSession.h"

#include <utility>
#include "ChatRoom.h"


WebsocketSession::WebsocketSession(beast::ssl_stream<beast::tcp_stream>&& stream,
                                   const std::shared_ptr <ChatRoom> &room,
                                   std::string login)
    : ws(std::move(stream)), room(room), login(std::move(login)) {
}

WebsocketSession::~WebsocketSession() {
    std::cout << "Closed WS connection from client\n";
    room->Leave(login);
}


void WebsocketSession::DoRead() {
    ws.async_read(
            buffer,
            [self = shared_from_this()](error_code err, size_t bytesRead) {
                if (!err) {
                    auto str = beast::buffers_to_string(self->buffer.data());
                    std::cout << "Received json: " << str;
                    self->room->Send(self->login, str);
                    self->buffer.consume(self->buffer.size());
                    self->DoRead();
                }
            });
}

void WebsocketSession::Send(const std::shared_ptr<std::string>& msg) {
    std::cout << "Sending json:" << *msg << std::endl;

    boost::asio::post(
            ws.get_executor(),
            [self = shared_from_this(), msg]() {
                self->sendq.push(msg);
                //need a queue, to avoid interleaving async_write calls (can only have one write at a time)
                if (self->sendq.size() == 1) {
                    self->DoWrite();
                }
            }
    );
}

void WebsocketSession::DoWrite() {
    ws.async_write(
            boost::asio::buffer(*sendq.front()),
            [self = shared_from_this()] (error_code err, std::size_t) {
                if (!err) {
                    self->sendq.pop();
                }

                if (!self->sendq.empty()) {
                    self->DoWrite();
                }
            }
    );
}
