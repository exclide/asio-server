//
// Created by w on 03.12.23.
//

#ifndef ASIO_SERVER_CLIENTMESSAGE_H
#define ASIO_SERVER_CLIENTMESSAGE_H

#include "Asio.h"

struct ClientMessage {
    std::string to;
    std::string text;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientMessage, to, text);
};

#endif //ASIO_SERVER_CLIENTMESSAGE_H
