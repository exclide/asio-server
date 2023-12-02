//
// Created by asd on 23-Oct-23.
//

#include "ChatSession.h"
#include "ChatRoom.h"


ChatSession::ChatSession(beast::ssl_stream<beast::tcp_stream>&& stream, const std::shared_ptr <ChatRoom> &room)
    : ws(std::move(stream)), room(room) {
}

ChatSession::~ChatSession() {
    std::cout << "Closed connection from client\n";
    room->Leave(weak_from_this());
}


void ChatSession::DoRead() {
    ws.async_read(
            buffer,
            [self = shared_from_this()](error_code err, size_t bytesRead) {
                if (!err) {
                    auto str = beast::buffers_to_string(self->buffer.data());
                    std::cout << "Broadcasting message: " << str;
                    self->room->Send(str);
                    self->buffer.consume(self->buffer.size());
                    self->DoRead();
                }
            });
}

void ChatSession::Send(const std::shared_ptr<std::string>& msg) {
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

void ChatSession::DoWrite() {
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
