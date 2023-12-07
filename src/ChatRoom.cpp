//
// Created by asd on 23-Oct-23.
//

#include "ChatRoom.h"
#include "WebsocketSession.h"
#include "Asio.h"
#include "ClientMessage.h"
#include "DbMessage.h"

void ChatRoom::Join(const std::string& login, const std::weak_ptr<WebsocketSession>& session) {
    std::lock_guard lock(m);
    wsMap[login] = session;
}

void ChatRoom::Leave(const std::string& login) {
    std::lock_guard lock(m);
    wsMap.erase(login);
}

void ChatRoom::Send(const std::string& from, const std::string& jsn) {
    json j, msg;
    ClientMessage msgIn;
    DbMessage dbMessage;

    try {
        j = json::parse(jsn);
        msgIn = j.template get<ClientMessage>();
        dbMessage = {from, msgIn.to, msgIn.text, std::time(0)};
        msg = dbMessage;
    } catch (...) {
        std::cout << "Wrong json format\n";
    }

    auto ssJson = std::make_shared<std::string>(nlohmann::to_string(msg));
    messageService->AddMessage(dbMessage);

    std::lock_guard lock(m);
    if (wsMap.count(msgIn.to)) {
        auto msgTarget = wsMap[msgIn.to].lock(); //target websocket session
        msgTarget->Send(ssJson);
    } else {
        std::cout << "Receiver client offline, but message saved to db\n";
    }
}
