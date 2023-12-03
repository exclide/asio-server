//
// Created by asd on 23-Oct-23.
//

#include "ChatRoom.h"
#include "WebsocketSession.h"
#include "Asio.h"
#include "Message.h"
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
    json j = json::parse(jsn);
    Message msgIn = j.template get<Message>();
    Message msgOut{from, msgIn.text};
    json msg = msgOut;

    auto ssJson = std::make_shared<std::string>(nlohmann::to_string(msg));
    auto date = std::time(nullptr);
    DbMessage dbMessage{from, msgIn.to, msgIn.text, date};
    messageService->AddMessage(dbMessage);

    std::lock_guard lock(m);
    if (wsMap.count(msgIn.to)) {
        auto msgTarget = wsMap[msgIn.to].lock(); //target websocket session
        msgTarget->Send(ssJson);
    }
}
