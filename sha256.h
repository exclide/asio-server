//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_SHA256_H
#define ASIO_SERVER_SHA256_H

#include <string>
#include <openssl/sha.h>

std::string sha256(const std::string& str) {
    unsigned char hash [SHA256_DIGEST_LENGTH];
    auto hashedPassword =
            std::string((char*)SHA256((const unsigned char*)(str.data()), str.size(), hash));

    return hashedPassword;
}

#endif //ASIO_SERVER_SHA256_H
