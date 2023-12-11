//
// Created by asd on 23-Oct-23.
//

#include "SharedState.h"
#include "WebsocketSession.h"
#include "Asio.h"
#include "../message/ClientMessage.h"
#include "../message/DbMessage.h"

SharedState::SharedState(const std::shared_ptr<MessageService>& messageService,
                         const std::shared_ptr<AuthService>& authService)
        : messageService(messageService),
          authService(authService) {}


void SharedState::Join(const std::string& login, const std::weak_ptr<WebsocketSession>& session) {
    std::lock_guard lock(m);
    wsMap[login] = session;
}

void SharedState::Leave(const std::string& login) {
    std::lock_guard lock(m);
    wsMap.erase(login);
}

void SharedState::Send(const std::string& from, const std::string& jsn) {
    json j, msg;
    ClientMessage msgIn;
    DbMessage dbMessage;

    try {
        j = json::parse(jsn);
        msgIn = j.template get<ClientMessage>();
        dbMessage = {from, msgIn.to, msgIn.text, std::time(0)};
        msg = dbMessage;
    } catch (...) {
        std::cerr << "Wrong json format\n";
    }

    auto ssJson = std::make_shared<std::string>(nlohmann::to_string(msg));
    messageService->AddMessage(dbMessage);

    std::lock_guard lock(m);
    if (auto msgTarget = wsMap[msgIn.to].lock(); msgTarget != nullptr) {
        msgTarget->Send(ssJson);
    } else {
        std::cout << "Receiver client offline, but message saved to db\n";
    }
}

std::shared_ptr<MessageService> SharedState::GetMessageService() {
    return messageService;
}

std::shared_ptr<AuthService> SharedState::GetAuthService() {
    return authService;
}
