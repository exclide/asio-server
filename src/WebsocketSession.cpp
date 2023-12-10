//
// Created by asd on 23-Oct-23.
//

#include "WebsocketSession.h"

#include <utility>
#include "SharedState.h"


WebsocketSession::WebsocketSession(beast::ssl_stream<beast::tcp_stream>&& stream,
                                   const std::shared_ptr <SharedState> &room,
                                   std::string login)
    :
    pingTimer(stream.get_executor()),
    ws(std::move(stream)),
    room(room),
    login(std::move(login)) {
}

WebsocketSession::~WebsocketSession() {
    std::cout << "Closed WS connection from client\n";
    room->Leave(login);
}


void WebsocketSession::DoRead() {
    ws.async_read(
            buffer,
            [self = shared_from_this()](error_code err, size_t bytesRead) {
                if (err) return self->Fail(err, "Websocket read failed");

                auto str = beast::buffers_to_string(self->buffer.data());
                std::cout << "Received json: " << str;
                self->room->Send(self->login, str);
                self->buffer.consume(self->buffer.size());
                self->DoRead();
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
                if (err) return self->Fail(err, "Websocket write failed");

                self->sendq.pop();

                if (!self->sendq.empty()) {
                    self->DoWrite();
                }
            }
    );
}

void WebsocketSession::Fail(error_code err, const char *what) {
    if (err == boost::asio::error::operation_aborted || err == websocket::error::closed
        || err == boost::asio::error::eof)
        return;

    std::cerr << what << ": " << err.message() << std::endl;
}

void WebsocketSession::DoClose() {
    if (ws.is_open()) {
        ws.async_close("", [self = shared_from_this()](auto err) {
            if (err) return self->Fail(err, "DoClose");
        });
    }
}


void WebsocketSession::DoPing() {
    if (!pongReceived) {
        std::cerr << "Client didn't answer ping, closing websocket\n";
        return DoClose();
    }

    ws.async_ping("", [this](auto err) {
        if (err) return Fail(err, "DoPing");
        pongReceived = false;

        pingTimer.expires_after(boost::asio::chrono::seconds(pingExpirationTime));
        pingTimer.async_wait([this](auto err) {
            if (!err) DoPing();
        });
    });
}

std::shared_ptr<std::string> WebsocketSession::FormFirstMessage() {
    //send the needed data
    auto authService = room->GetAuthService();
    auto msgService = room->GetMessageService();

    auto users = authService->FindAllUsers();
    for (auto& u : users) u.password = "";
    auto messages = msgService->FindAllForLogin(login);
    json j;

    j["users"] = users;
    j["messages"] = messages;

    std::string jsonStr = nlohmann::to_string(j);
    auto ss = std::make_shared<std::string>(jsonStr);
    std::cout << jsonStr << std::endl;

    return ss;
}

