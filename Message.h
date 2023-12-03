//
// Created by w on 03.12.23.
//

#ifndef ASIO_SERVER_MESSAGE_H
#define ASIO_SERVER_MESSAGE_H

#include "Asio.h"

struct Message {
    std::string to;
    std::string text;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Message, to, text);
};

#endif //ASIO_SERVER_MESSAGE_H
