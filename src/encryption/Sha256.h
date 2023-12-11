//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_SHA256_H
#define ASIO_SERVER_SHA256_H

#include <string>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>


std::string Sha256(const std::string& str);

#endif //ASIO_SERVER_SHA256_H
