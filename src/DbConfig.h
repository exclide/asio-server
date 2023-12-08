//
// Created by w on 08.12.23.
//

#ifndef SERVER_DBCONFIG_H
#define SERVER_DBCONFIG_H

#include <string>

class DbConfig {
public:
    std::string host;
    std::string port;
    std::string dbname;
    std::string user;
    std::string password;

    int connections;

    std::string GetDbConnectionString() const {
        std::string conn;

        conn += "host=" + host + " ";
        conn += "port=" + port + " ";
        conn += "dbname=" + dbname + " ";
        conn += "user=" + user + " ";
        conn += "password=" + password + " ";

        return conn;
    }
};

#endif //SERVER_DBCONFIG_H
