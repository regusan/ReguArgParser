#include "../iutest/include/gtest/iutest_switch.hpp"

#include <string>
#include <vector>
#include "../include/ReguArgParser.hpp"
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
    ArgParser args({"-a", "value1", "-b", "value2", "-c", "3", "-d", "3.14", "-e", "hoge,fuga,piyo"});
    ASSERT_EQ(args.getFlagValue<std::string>({{"-a"}}), "value1");
    ASSERT_EQ(args.getFlagValue<std::string>({{"-b"}}), "value2");
    ASSERT_EQ(args.getFlagValue<int>({{"-c"}}), 3);
    ASSERT_EQ(args.getFlagValue<double>({{"-d"}}), 3.14);
    ASSERT_EQ(args.getFlagValue<std::string>({{"-e"}}), "hoge,fuga,piyo");
}

IUTEST(ArgParserTest, GetFlagArrayValue)
{
    ArgParser args({"-a", "1,2,3", "-b", "4,5,6", "-c", "7.1,7.2,7.3", "-d", "hoge,fuga,piyo"});
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
    auto arrayValueC = args.getFlagArrayValue<double>({{"-c"}});
    ASSERT_EQ(arrayValueC.size(), 3);
    ASSERT_EQ(arrayValueC[0], 7.1);
    ASSERT_EQ(arrayValueC[1], 7.2);
    ASSERT_EQ(arrayValueC[2], 7.3);

    auto arrayValueD = args.getFlagArrayValue<std::string>({{"-d"}});
    ASSERT_EQ(arrayValueD.size(), 3);
    ASSERT_EQ(arrayValueD[0], "hoge");
    ASSERT_EQ(arrayValueD[1], "fuga");
    ASSERT_EQ(arrayValueD[2], "piyo");

    // デフォルト値のテスト
    auto arrayValueE = args.getFlagArrayValue<int>({{"-e"}}, {10, 20, 30});
    ASSERT_EQ(arrayValueE.size(), 3);
    ASSERT_EQ(arrayValueE[0], 10);
    ASSERT_EQ(arrayValueE[1], 20);
    ASSERT_EQ(arrayValueE[2], 30);
}

IUTEST(ArgParserTest, GetFlagValueWithDefault)
{
    ArgParser args({"-a", "value1"});
    ASSERT_EQ(args.getFlagValue<std::string>({{"-a"}}), "value1");
    ASSERT_EQ(args.getFlagValue<std::string>({{"-b"}}, std::string("default")), "default");
}

// エラーのテスト
IUTEST(ArgParserTest, GetFlagValueError)
{
    ArgParser args({"-a", "value1"});
    ASSERT_THROW(args.getFlagValue<int>({{"-b"}}), std::runtime_error);
    ASSERT_THROW(args.getFlagValue<double>({{"-c"}}), std::runtime_error);
}

int main(int argc, char const *argv[])
{
    IUTEST_INIT(&argc, argv);
    IUTEST_RUN_ALL_TESTS();

    char const *argv1[] = {"-a", "-b", "5", "-cat", "hello", "-d", "3.14", "-epic", "1,2,3,4,5"};
    const int argc1 = 9;

    RArg::ArgParser args(argc1, argv1);

    // --- フラグの確認 ---
    bool has_a = args.hasFlag({{"-a"}});

    // --- 値の取得 ---
    int b_val = args.getFlagValue<int>({{"-b"}, "discription"});                       // result: 5
    std::string c_val = args.getFlagValue<std::string>({{"-c", "-cat"}, "help text"}); // result: "hello"
    double d_val = args.getFlagValue<double>({{"-d", "discription"}, "discription"});  // result: 3.14

    // --- 配列値の取得 ---
    std::vector<int> e_array = args.getFlagArrayValue<int>({{"-e", "-epic"}, "discription"});
    // result: e_array = {1, 2, 3, 4, 5}
    // e_array.size() == 5, e_array[0] == 1, ..., e_array[4] == 5

    // --- 存在しないフラグへのデフォルト値 ---
    std::string unknown_val = args.getFlagValue<std::string>({{"-z"}, "discription"}, "default");    // result: "default"
    std::vector<int> unknown_array = args.getFlagArrayValue<int>({{"-x"}, "discription"}, {99, 88}); // result: {99, 88}

    try
    {
        args.getFlagValue<int>({{"-donthave"}});
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }    
    try
    {
        args.getFlagValue<int>({{"-b"}});
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    std::cout << args.GetUsage() << std::endl;

    return 0;
}
