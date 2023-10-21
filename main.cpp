#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <unordered_set>

using boost::asio::io_context;
using boost::asio::ip::tcp;
using boost::system::error_code;

class ChatSession;

class ChatRoom {
public:
    void Join(const std::shared_ptr<ChatSession>& session) {
        std::lock_guard lock(m);
        sessions.insert(session);
    }

    void Leave(const std::shared_ptr<ChatSession>& session) {
        std::lock_guard lock(m);
        sessions.erase(session);
    }

    void Send(std::string message) {

    }

private:
    std::mutex m;
    std::unordered_set<std::shared_ptr<ChatSession>> sessions;
};

class ChatSession : std::enable_shared_from_this<ChatSession> {
public:
    ChatSession(io_context& ioc, ChatRoom& room) : socket(ioc), room(room) {

    }

    tcp::socket& GetSocket() {
        return socket;
    }

    void Start() {
        room.Join(shared_from_this());

        DoRead();
    }

    void DoRead() {
        auto self{shared_from_this()};

        socket.async_read_some()
    }

    void Send(const std::string& msg) {

    }

private:
    tcp::socket socket;
    ChatRoom& room;
};

class ChatServer {
public:
    ChatServer(io_context& ioc, tcp::endpoint& endpoint)
        : ioc(ioc), acceptor(ioc, endpoint) {
        //конструктор ацептора выше делает open, bind, listen
        StartAccept();
    }


    void StartAccept() {
        auto session{std::make_shared<ChatSession>(ioc, room)};

        acceptor.async_accept(session->GetSocket(),
                [this, session] (const error_code& err) {
                    if (!err) {
                        session->Start();
                    }

                    StartAccept();
        });
    }


private:
    io_context& ioc;
    tcp::acceptor acceptor;
    ChatRoom room;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Correct usage: serv <port>\n";
        return 1;
    }

    io_context ioc;
    tcp::endpoint endpoint(tcp::v4(), std::stoi(argv[1]));

    ChatServer server{ioc, endpoint};

    ioc.run();

    return 0;
}
