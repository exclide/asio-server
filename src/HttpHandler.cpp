//
// Created by w on 08.12.23.
//
#include "HttpHandler.h"


HttpHandler::HttpHandler(const std::shared_ptr<AuthService>& authService, const std::string& secretJwtKey) :
    authService(authService),
    secretJwtKey(secretJwtKey) {
}

std::string HttpHandler::GetJwtKey() {
    return secretJwtKey;
}