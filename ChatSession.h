//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATSESSION_H
#define ASIO_SERVER_CHATSESSION_H

#include "Asio.h"

class ChatSession : public std::enable_shared_from_this<ChatSession> {
public:
    ChatSession(tcp::socket socket) : socket(std::move(socket)) {
    }

    void Start() {
        DoRead();
    }

    void DoRead() {
        auto self{shared_from_this()};

        buf.consume(buf.size());

        boost::asio::async_read_until(
                socket,
                buf,
                "\n",
                [this, self](const error_code& err, std::size_t bytes) {
                    if (!err) {
                        DoWrite(bytes);
                    }
                }
        );
    }

    void DoWrite(std::size_t bytes) {
        auto self{shared_from_this()};

        boost::asio::async_write(
                socket,
                buf,
                [this, self] (const error_code& err, std::size_t bytes) {
                    if (!err) {
                        DoRead();
                    }
                });
    }

    void Send(const std::string& msg) {

    }

private:
    tcp::socket socket;
    boost::asio::streambuf buf;
};

#endif //ASIO_SERVER_CHATSESSION_H
