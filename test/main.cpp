//
// Created by w on 07.12.23.
//

#include "gtest/gtest.h"
#include "../src/Sha256.h"
#include "../src/DbConfigParser.h"

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


int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}