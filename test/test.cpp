// #include "../iutest/include/iutest.hpp"

#include "../iutest/include/gtest/iutest_switch.hpp"

#include <string>
#include <vector>
#include "ReguArgParser.hpp"
#include <iostream>
using namespace RArg;
using namespace std;

IUTEST(ArgParserTest, HasFlag)
{
    ArgParser args({"-a", "-b", "--long-flag"});
    ASSERT_TRUE(args.hasFlag({{"-a"}}));
    ASSERT_TRUE(args.hasFlag({{"-b"}}));
    ASSERT_TRUE(args.hasFlag({{"--long-flag"}}));
    ASSERT_FALSE(args.hasFlag({{"-c"}}));
}

IUTEST(ArgParserTest, HasFlagWithMixedFlags)
{
    ArgParser args({"-a", "--bravo", "-c"});
    ASSERT_TRUE(args.hasFlag(Flag("-a", "--alpha")));
    ASSERT_TRUE(args.hasFlag(Flag("-b", "--bravo")));
    ASSERT_TRUE(args.hasFlag(Flag("-c", "--charlie")));
    ASSERT_FALSE(args.hasFlag(Flag("-d", "--delta")));
}

IUTEST(ArgParserTest, CheckValue)
{
    ArgParser args({"-a", "value1", "-b", "value2"});
    ASSERT_EQ(args.getFlagValue<std::string>({{"-a"}}), "value1");
    ASSERT_EQ(args.getFlagValue<std::string>({{"-b"}}), "value2");

    // デフォルト値のテスト
    ASSERT_EQ(args.getFlagValue<std::string>({{"-c"}}, std::string("default")), "default");
}

IUTEST(ArgParserTest, GetFlagArrayValue)
{
    ArgParser args({"-a", "1,2,3", "-b", "4,5,6"});
    auto arrayValueA = args.getFlagArrayValue<int>({{"-a"}});
    auto arrayValueB = args.getFlagArrayValue<int>({{"-b"}});

    ASSERT_EQ(arrayValueA.size(), 3);
    ASSERT_EQ(arrayValueA[0], 1);
    ASSERT_EQ(arrayValueA[1], 2);
    ASSERT_EQ(arrayValueA[2], 3);

    ASSERT_EQ(arrayValueB.size(), 3);
    ASSERT_EQ(arrayValueB[0], 4);
    ASSERT_EQ(arrayValueB[1], 5);
    ASSERT_EQ(arrayValueB[2], 6);
}

IUTEST(ArgParserTest, GetFlagValueWithDefault)
{
    ArgParser args({"-a", "value1"});
    ASSERT_EQ(args.getFlagValue<std::string>({{"-a"}}), "value1");
    ASSERT_EQ(args.getFlagValue<std::string>({{"-b"}}, std::string("default")), "default");
}

int main(int argc, char const *argv[])
{
    IUTEST_INIT(&argc, argv);
    IUTEST_RUN_ALL_TESTS();
    return 0;
}
