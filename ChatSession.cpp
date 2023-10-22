//
// Created by asd on 23-Oct-23.
//

#include "ChatSession.h"
#include "ChatRoom.h"

ChatSession::ChatSession(tcp::socket socket, const std::shared_ptr <ChatRoom> &room)
    : socket(std::move(socket)), room(room) {
}

void ChatSession::Start() {
    room->Join(shared_from_this());
    DoRead();
}

void ChatSession::DoRead() {
    buf.consume(buf.size());

    boost::asio::async_read_until(
            socket,
            buf,
            "\n",
            [self = shared_from_this()](const error_code& err, std::size_t bytes) {
                if (!err) {
                    self->DoWrite(bytes);
                }
            }
    );
}

void ChatSession::DoWrite(std::size_t bytes) {
    boost::asio::async_write(
            socket,
            buf,
            [self = shared_from_this()] (const error_code& err, std::size_t bytes) {
                if (!err) {
                    self->DoRead();
                }
            });
}

void ChatSession::Send(const std::string &msg) {

}
