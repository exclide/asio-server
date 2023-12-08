//
// Created by w on 04.12.23.
//

#ifndef ASIO_SERVER_MESSAGEREPOSITORY_H
#define ASIO_SERVER_MESSAGEREPOSITORY_H

#include <unordered_map>
#include <pqxx/pqxx>
#include "DbMessage.h"
#include "DbContext.h"

class MessageRepository {
private:
    DbContext* dbContext = DbContext::GetInstance();
public:

    void Create(const DbMessage& msg) {
        std::shared_ptr<pqxx::connection> conn;

        try {
            conn = dbContext->GetConnection();

            conn->prepare(
                    "insert",
                    "INSERT INTO messages (receiver, sender, text, date) VALUES ($1, $2, $3, $4)");

            pqxx::work tx(*conn);
            tx.exec_prepared("insert", msg.receiver, msg.sender, msg.text, msg.date);

            tx.commit();

            dbContext->ReleaseConnection(conn);
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
            dbContext->ReleaseConnection(conn);
        }
    }

    std::vector<DbMessage> FindAll() {
        std::vector<DbMessage> msgs;
        std::shared_ptr<pqxx::connection> conn;

        try {
            conn = dbContext->GetConnection();

            std::string sqlQuery = "SELECT * FROM messages ORDER BY date";

            pqxx::work tx{*conn};
            pqxx::result res = tx.exec(sqlQuery);

            for (const auto& row : res) {
                auto receiver = row["receiver"].as<std::string>();
                auto sender = row["sender"].as<std::string>();
                auto text = row["text"].as<std::string>();
                auto date = row["date"].as<time_t>();

                DbMessage msg{sender, receiver, text, date};
                msgs.push_back(msg);
            }

            dbContext->ReleaseConnection(conn);

        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
            dbContext->ReleaseConnection(conn);
        }

        return msgs;
    }

private:

};

#endif //ASIO_SERVER_MESSAGEREPOSITORY_H
