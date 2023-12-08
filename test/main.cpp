//
// Created by w on 07.12.23.
//

#include "gtest/gtest.h"
#include "../src/Sha256.h"
#include "../src/DbConfigParser.h"
#include "../src/MessageService.h"
#include "../src/AuthService.h"
#include "StubMessageRepository.h"
#include "StubUserRepository.h"

TEST(Sha256Test, StringLengthTest) {
    static int sha256StringLength = 64;

    auto stringHash = Sha256("test");
    EXPECT_EQ(stringHash.size(), sha256StringLength);
}

TEST(Sha256Test, EmptyInputTest) {
    ASSERT_DEATH(Sha256(""), "");
}

TEST(DbConfigParserTest, BadFileTest) {
    ASSERT_THROW(ParseDbConfig(""), ParseException);
    ASSERT_THROW(ParseDbConfig("zalupa_slonika"), ParseException);
}

TEST(MessageServiceTest, MessageAddTest) {
    auto msgService = MessageService(std::make_shared<StubMessageRepository>());
    DbMessage msg{"sender", "receiver", "text", time_t(0)};
    msgService.AddMessage(msg);

    auto msgs = msgService.FindAll();
    EXPECT_EQ(msgs.size(), 1);

    msgService.AddMessage(msg);
    msgs = msgService.FindAll();
    EXPECT_EQ(msgs.size(), 2);
}

TEST(MessageServiceTest, MessageFindTest) {
    auto msgService = MessageService(std::make_shared<StubMessageRepository>());
    for (int i = 0; i < 10; i++) {
        DbMessage msg{
            "sender" + std::to_string(i),
            "receiver" + std::to_string(i),
            "text",
            time_t(0)};

        msgService.AddMessage(msg);
    }

    for (int i = 0; i < 10; i++) {
        auto msg1 = msgService.FindAllForLogin("sender" + std::to_string(i));
        auto msg2 = msgService.FindAllForLogin("receiver" + std::to_string(i));
        EXPECT_EQ(msg1.size(), 1);
        EXPECT_EQ(msg2.size(), 1);
    }
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}