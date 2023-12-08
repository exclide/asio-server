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
#include "../src/HttpHandler.h"

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

TEST(AuthServiceTest, EmptyThrowsTest) {
    auto authService = AuthService(std::make_shared<StubUserRepository>());
    User user;
    ASSERT_THROW(authService.Register(user), ChatException);
    ASSERT_THROW(authService.Login(user), ChatException);
}

TEST(AuthServiceTest, RegisterLoginTest) {
    auto authService = AuthService(std::make_shared<StubUserRepository>());

    User user{"asd1", "asd1"};
    authService.Register(user);

    user.password = "asd1";

    auto loggedUser = authService.Login(user);

    EXPECT_FALSE(loggedUser.login.empty());
    EXPECT_FALSE(loggedUser.password.empty());
}

TEST(HttpHandlerTest, LoginTest) {
    auto authService=
            std::make_shared<AuthService>(std::make_shared<StubUserRepository>());
    auto httpHandler = HttpHandler(authService);
    User user{"asd1", "asd1"};
    authService->Register(user);

    http::request<http::string_body> req;
    req.method(http::verb::post);
    req.target("/api/login");
    std::string bodyJson = R"({"login": "asd1", "password": "asd1"})";
    req.body() = bodyJson;

    auto res = httpHandler.HandleRequest(std::move(req));
    EXPECT_TRUE(res.second);

    http::request<http::string_body> req2;
    req.method(http::verb::post);
    req.target("/api/login");
    std::string bodyJson2 = R"({"login": "asd1", "password": "WRONGPASSWORD"})";
    req.body() = bodyJson2;
    auto res2 = httpHandler.HandleRequest(std::move(req));
    EXPECT_FALSE(res2.second);
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}