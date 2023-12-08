//
// Created by w on 08.12.23.
//
#include "MessageRepository.h"

MessageRepository::MessageRepository(const std::shared_ptr <DbConnPool> &dbConnPool) : dbConnPool(dbConnPool) {}

void MessageRepository::Create(const DbMessage &msg) {
    std::shared_ptr<pqxx::connection> conn;

    try {
        conn = dbConnPool->GetConnection();

        pqxx::work tx(*conn);
        tx.exec_prepared("InsertMessage", msg.receiver, msg.sender, msg.text, msg.date);

        tx.commit();

        dbConnPool->ReleaseConnection(conn);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        dbConnPool->ReleaseConnection(conn);
    }
}

std::vector<DbMessage> MessageRepository::FindAll() {
    std::vector<DbMessage> msgs;
    std::shared_ptr<pqxx::connection> conn;

    try {
        conn = dbConnPool->GetConnection();

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

        dbConnPool->ReleaseConnection(conn);

    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        dbConnPool->ReleaseConnection(conn);
    }

    return msgs;
}
