//
// Created by w on 04.12.23.
//

#ifndef ASIO_SERVER_MESSAGEREPOSITORY_H
#define ASIO_SERVER_MESSAGEREPOSITORY_H

#include <unordered_map>
#include <pqxx/pqxx>
#include "DbMessage.h"

class MessageRepository {
public:

    void Create(const DbMessage& msg) {
        try {
            pqxx::connection conn(
                    "host=localhost port=5432 dbname=postgres user=postgres password=postgres"
            );

            conn.prepare(
                    "insert",
                    "INSERT INTO messages (receiver, sender, text, date) VALUES ($1, $2, $3, $4)");

            pqxx::work tx(conn);
            tx.exec_prepared("insert", msg.receiver, msg.sender, msg.text, msg.date);

            tx.commit();
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }

    std::vector<DbMessage> FindAll() {
        std::vector<DbMessage> msgs;

        try {
            pqxx::connection conn(
                    "host=localhost port=5432 dbname=postgres user=postgres password=postgres"
            );

            std::string sqlQuery = "SELECT * FROM messages ORDER BY date";

            pqxx::work tx{conn};
            pqxx::result res = tx.exec(sqlQuery);

            for (const auto& row : res) {
                auto receiver = row["receiver"].as<std::string>();
                auto sender = row["sender"].as<std::string>();
                auto text = row["text"].as<std::string>();
                auto date = row["date"].as<time_t>();

                DbMessage msg{sender, receiver, text, date};
                msgs.push_back(msg);
            }

        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }

        return msgs;
    }

private:

};

#endif //ASIO_SERVER_MESSAGEREPOSITORY_H
