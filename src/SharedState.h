//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_CHATROOM_H
#define ASIO_SERVER_CHATROOM_H

#include <vector>
#include <memory>
#include <unordered_map>
#include "MessageService.h"
#include "AuthService.h"

class WebsocketSession;

class SharedState {
public:
    explicit SharedState(const std::shared_ptr<MessageService>& messageService,
                         const std::shared_ptr<AuthService>& authService);
    void Join(const std::string& login, const std::weak_ptr<WebsocketSession>& session);
    void Leave(const std::string& login);
    void Send(const std::string& from, const std::string& jsn);

    std::shared_ptr<MessageService> GetMessageService();
    std::shared_ptr<AuthService> GetAuthService();

private:
    std::mutex m;
    std::unordered_map<std::string, std::weak_ptr<WebsocketSession>> wsMap;
    std::shared_ptr<MessageService> messageService;
    std::shared_ptr<AuthService> authService;
};

#endif //ASIO_SERVER_CHATROOM_H
