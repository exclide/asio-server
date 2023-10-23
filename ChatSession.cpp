//
// Created by asd on 23-Oct-23.
//

#include "ChatSession.h"
#include "ChatRoom.h"

ChatSession::ChatSession(tcp::socket socket, const std::shared_ptr <ChatRoom> &room)
    : socket(std::move(socket)), room(room) {
}

ChatSession::~ChatSession() {
    room->Leave(weak_from_this());
}


void ChatSession::Start() {
    room->Join(weak_from_this());

    boost::asio::dispatch( //the first call won't be on strand, so dispatch to strand
            socket.get_executor(),
            [self = shared_from_this()]() {
                self->DoRead();
            });
}

void ChatSession::DoRead() {
    boost::asio::async_read_until(
            socket,
            boost::asio::dynamic_buffer(data),
            "\n",
            [self = shared_from_this()](const error_code& err, std::size_t bytes) {
                if (!err) {
                    self->room->Send(self->data);
                    self->data.clear();
                    self->DoRead();
                }
            }
    );
}

void ChatSession::Send(const std::shared_ptr<std::string>& msg) {
    boost::asio::post(
            socket.get_executor(),
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
    boost::asio::async_write(
            socket,
            boost::asio::buffer(*sendq.front()),
            [self = shared_from_this()] (error_code err, std::size_t) {
                self->sendq.pop();

                if (!self->sendq.empty()) {
                    self->DoWrite();
                }
            }
    );
}