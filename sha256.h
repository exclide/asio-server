//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_SHA256_H
#define ASIO_SERVER_SHA256_H

#include <string>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>


std::string sha256(const std::string& str) {
    unsigned char hash [SHA256_DIGEST_LENGTH];
    const auto* data = (const unsigned char*)str.c_str();
    SHA256(data, str.size(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

#endif //ASIO_SERVER_SHA256_H
