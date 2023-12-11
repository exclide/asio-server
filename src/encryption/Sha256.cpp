//
// Created by w on 04.12.23.
//

#include <cassert>
#include "Sha256.h"

std::string Sha256(const std::string& str) {
    assert(str.size());

    unsigned char hash [SHA256_DIGEST_LENGTH];

    const auto* data = (const unsigned char*)str.c_str();
    SHA256(data, str.size(), hash);
    std::stringstream ss;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}