//
// Created by asd on 23-Oct-23.
//

#include "ChatSession.h"
#include "ChatRoom.h"

ChatSession::ChatSession(tcp::socket socket, const std::shared_ptr <ChatRoom> &room)
    : socket(std::move(socket)), room(room) {
}

ChatSession::~ChatSession() {
    room->Leave(this);
}


void ChatSession::Start() {
    room->Join(this);

    boost::asio::dispatch(
            socket.get_executor(),
            [self = shared_from_this()]() {
                self->DoRead();
            });
}

void ChatSession::DoRead() {
    data.clear();

    boost::asio::async_read_until(
            socket,
            boost::asio::dynamic_buffer(data),
            "\n",
            [self = shared_from_this()](const error_code& err, std::size_t bytes) {
                if (!err) {
                    self->room->Send(self->data);
                    self->DoRead();
                }
            }
    );
}

void ChatSession::Send(const std::shared_ptr<std::string>& msg) {
    boost::asio::async_write(
            socket,
            boost::asio::buffer(*msg),
            [self = shared_from_this(), msg] (error_code err, std::size_t) {

            }
    );
}