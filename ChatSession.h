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
    ChatSession(boost::asio::ssl::stream<tcp::socket>&& socket, const std::shared_ptr<ChatRoom>& room);
    ~ChatSession();

    void Start();
    void DoRead();
    void DoWrite();
    void DoHandshake();
    void Send(const std::shared_ptr<std::string>& msg);

private:
    //tcp::socket socket;
    boost::asio::ssl::stream<tcp::socket> socket;
    std::shared_ptr<ChatRoom> room;
    std::string data;
    std::queue<std::shared_ptr<std::string>> sendq;
    std::string clientAdr;
};

#endif //ASIO_SERVER_CHATSESSION_H
