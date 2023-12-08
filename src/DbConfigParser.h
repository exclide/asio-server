//
// Created by w on 08.12.23.
//

#ifndef SERVER_DBCONFIGPARSER_H
#define SERVER_DBCONFIGPARSER_H

#include <string>
#include <fstream>
#include "ParseException.h"
#include "DbConfig.h"


bool StringStartsWith(const std::string& source, const std::string& with) {
    auto sz = with.size();
    int i = 0;

    while (i < std::min(source.size(), with.size()) && source[i] == with[i]) i++;

    return i == sz;
}

std::string splitAfterDelimiter(const std::string& str, char delimiter = '=') {
    return str.substr(str.find(delimiter) + 1);
}

DbConfig ParseDbConfig(const std::string& configFile) {
    std::ifstream input(configFile);

    if (!input.is_open()) {
        throw ParseException("Can't open file with path: " + configFile);
    }

    std::string in;
    DbConfig cfg;

    while (input >> in) {
        if (StringStartsWith(in, "host")) cfg.host = splitAfterDelimiter(in);
        else if (StringStartsWith(in, "port")) cfg.port = splitAfterDelimiter(in);
        else if (StringStartsWith(in, "dbname")) cfg.dbname = splitAfterDelimiter(in);
        else if (StringStartsWith(in, "user")) cfg.user = splitAfterDelimiter(in);
        else if (StringStartsWith(in, "password")) cfg.password = splitAfterDelimiter(in);
        else if (StringStartsWith(in, "connections")) cfg.connections = std::stoi(splitAfterDelimiter(in));
    }

    if (cfg.host.empty() || cfg.port.empty() || cfg.dbname.empty()
    || cfg.user.empty() || cfg.password.empty() || cfg.connections == 0) {
        throw ParseException{"Wrong config input"};
    }

    return cfg;
}

#endif //SERVER_DBCONFIGPARSER_H
