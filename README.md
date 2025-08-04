# ReguArgParser
Single Header Argument Parser for C++

# Installation
```bash
git clone https://github.com/regusan/ReguArgParser.git

```

# Rquirements
C++20 or later

# Usage
```cpp
// --- 1. フラグの確認 (hasFlag) ---
// コマンドライン引数をシミュレート: your_program -a -b --long-flag --charlie
RArg::ArgParser args({"-a", "-b", "--long-flag", "--charlie"});

bool has_a = args.hasFlag({{"-a"}});                   // result: true
bool has_long_flag = args.hasFlag({{"--long-flag"}});  // result: true
// 複数の別名を持つフラグを確認: -c または --charlie があればtrue
bool has_c_or_charlie = args.hasFlag(RArg::Flag("-c", "--charlie")); // result: true
bool has_z = args.hasFlag({{"-z"}});                   // result: false


// --- 2. フラグの値の取得 (getFlagValue) ---
// コマンドライン引数をシミュレート: your_program -s hello -i 123 -d 3.14
RArg::ArgParser args({"-s", "hello", "-i", "123", "-d", "3.14"});

std::string str_val = args.getFlagValue<std::string>({{"-s"}}); // result: "hello"
int int_val = args.getFlagValue<int>({{"-i"}});                 // result: 123
double double_val = args.getFlagValue<double>({{"-d"}});        // result: 3.14

// 存在しないフラグにデフォルト値を指定して取得
std::string default_str = args.getFlagValue<std::string>({{"-unknown"}}, "default_value"); // result: "default_value"

// 値がない場合に例外が発生する例 (catchブロックで囲んでエラーを抑制)
RArg::ArgParser invalid_args({"-a"}); // -a の後に値がない
try {
invalid_args.getFlagValue<std::string>({{"-a"}});
} catch (const std::runtime_error& e) {
// エラー処理（ここでは何も行わない）
// e.what() は "Flag '-a' requires a value but none was provided." のようなメッセージ


// --- 3. カンマ区切りの値の配列取得 (getFlagArrayValue) ---
// コマンドライン引数をシミュレート: your_program -nums 1,2,3 -words alpha,beta,gamma -floats 1.1,2.2,3.3
RArg::ArgParser args({"-nums", "1,2,3", "-words", "alpha,beta,gamma", "-floats", "1.1,2.2,3.3"});

// 整数の配列を取得
std::vector<int> numbers = args.getFlagArrayValue<int>({{"-nums"}});
// result: numbers = {1, 2, 3}
// numbers.size() == 3, numbers[0] == 1, numbers[1] == 2, numbers[2] == 3

// 文字列の配列を取得
std::vector<std::string> words = args.getFlagArrayValue<std::string>({{"-words"}});
// result: words = {"alpha", "beta", "gamma"}
// words.size() == 3, words[0] == "alpha", words[1] == "beta", words[2] == "gamma"

// 浮動小数点数の配列を取得
std::vector<double> floats = args.getFlagArrayValue<double>({{"-floats"}});
// result: floats = {1.1, 2.2, 3.3}
// floats.size() == 3, floats[0] == 1.1, floats[1] == 2.2, floats[2] == 3.3

// 存在しないフラグにデフォルト配列を指定して取得
std::vector<int> default_array = args.getFlagArrayValue<int>({{"-missing-array"}}, {100, 200});
// result: default_array = {100, 200}
// default_array.size() == 2, default_array[0] == 100, default_array[1] == 200
```