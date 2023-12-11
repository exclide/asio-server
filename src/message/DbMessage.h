//
// Created by w on 04.12.23.
//

#ifndef ASIO_SERVER_DBMESSAGE_H
#define ASIO_SERVER_DBMESSAGE_H

#include <ctime>
#include "../network/Asio.h"

struct DbMessage {
    std::string sender;
    std::string receiver;
    std::string text;
    std::time_t date;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DbMessage, sender, receiver, text, date);
};

#endif //ASIO_SERVER_DBMESSAGE_H
