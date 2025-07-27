#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <map>
#include <set>
#include <optional>
namespace RArg
{
    class Flag
    {
    public:
        const std::optional<std::string> shortFlag;
        const std::optional<std::string> longFlag;
        const std::vector<std::string> flags = {};
        auto operator<=>(const Flag &) const = default;
        Flag(std::string shortFlag, std::string longFlag) : shortFlag(shortFlag), longFlag(longFlag), flags({shortFlag, longFlag}) {}
        Flag(std::string flag) : shortFlag(flag), flags({flag}) {}
    };

    /// @brief 引数データ
    class Arg
    {
    private:
    public:
        const Flag flags;
        const std::string helpText;
        auto operator<=>(const Arg &) const = default;
        Arg(const Flag &flags, const std::string &helpText = "")
            : flags(flags),
              helpText(helpText) {}
        std::string GetDisplayText() const
        {
            std::stringstream ss;
            if (flags.shortFlag.has_value())
                ss << flags.shortFlag.value();
            if (flags.longFlag.has_value())
                ss << "," << flags.longFlag.value();
            ss << "\t\t" << helpText;
            return ss.str();
        }
    };

    /// @brief 引数パーサー
    class ArgParser
    {
    private:
        const std::string exeName;
        const std::vector<std::string> args = {};
        std::set<Arg> argKeys;

        void RegistArg(const Arg &arg)
        {
            argKeys.insert(arg);
        }

    public:
        ArgParser(const int argc, const char **argv)
            : exeName(argc > 0 ? argv[0] : ""),
              args(argv + 1, argv + argc) {}
        ArgParser(const int argc, char **argv)
            : ArgParser(argc, const_cast<const char **>(argv)) {} // 非constにも対応させるため
        ArgParser(std::vector<std::string> args)
            : exeName(args.size() > 0 ? args[0] : ""),
              args(args.begin(), args.end()) {}

        /// @brief 指定したフラグが存在するかを取得
        /// @param arg
        /// @return フラグが存在するか
        bool hasFlag(const Arg &arg)
        {
            RegistArg(arg);
            return std::find_first_of(args.begin(), args.end(), arg.flags.flags.begin(), arg.flags.flags.end()) != args.end();
        }

        ///@brief 指定したフラグの値を取得する
        ///@tparam T 型
        ///@param arg 取得したい値のフラグデータ
        ///@return T型に変換された値
        ///@throws std::runtime_error フラグが存在しない、値がない、または型変換に失敗した場合
        template <typename T>
        T getFlagValue(const Arg &arg)
        {
            this->RegistArg(arg);
            const auto it = std::find_first_of(args.begin(), args.end(), arg.flags.flags.begin(), arg.flags.flags.end());
            const auto flagsStr = arg.flags.shortFlag.has_value()
                                      ? arg.flags.shortFlag.value()
                                  : "" + arg.flags.longFlag.has_value()
                                      ? arg.flags.longFlag.value()
                                      : "";
            if (it == args.end())
            {
                std::stringstream ss;
                ss << "Flag \"" << flagsStr << "\" not found. Usage: " << arg.GetDisplayText();
                throw std::runtime_error(ss.str());
            }

            if (it + 1 == args.end())
            {
                std::stringstream ss;
                ss << "Value for flag \"" << flagsStr << "\" not found. Usage: " << arg.GetDisplayText();
                throw std::runtime_error(ss.str());
            }

            const std::string valueStr = *(it + 1);
            if (valueStr.empty())
            {
                std::stringstream ss;
                ss << "Value for flag \"" << flagsStr << "\" is empty. Usage: " << arg.GetDisplayText();
                throw std::runtime_error(ss.str());
            }

            std::stringstream ss(valueStr);
            T value;
            ss >> value;

            // ストリームの終端まで読み込めたか、かつエラーがないかチェック
            if (ss.fail() || !ss.eof())
            {
                std::stringstream ss;
                ss << "Failed to convert value \"" << valueStr << "\" for flag \"" << flagsStr << "\" to type " << typeid(T).name() << ". Usage: " << arg.GetDisplayText();
                throw std::runtime_error(ss.str());
            }

            return value;
        }

        /// @brief 指定したフラグの値を取得する
        /// @tparam T 型
        /// @param arg 取得したい値の引数タイプ
        /// @param defaultValue フラグが存在しない場合のデフォルト値
        /// @return
        template <typename T>
        T getFlagValue(const Arg &arg, const T &defaultValue)
        {
            try
            {
                return this->getFlagValue<T>(arg);
            }
            catch (const std::runtime_error &e)
            {
                return defaultValue;
            }
        }

        ///@brief 指定したフラグの値を取得する
        ///@tparam T 型
        ///@param arg 取得したい値のフラグデータ
        ///@return T型に変換された値
        ///@throws std::runtime_error フラグが存在しない、値がない、または型変換に失敗した場合
        template <typename T>
        std::vector<T> getFlagArrayValue(const Arg &arg)
        {
            std::string valueStr = this->getFlagValue<std::string>(arg);

            // 文字列をカンマで分割し、配列に格納
            std::vector<T> arrayValue;
            std::stringstream ssValueStr(valueStr);
            std::string segment;
            while (std::getline(ssValueStr, segment, ','))
            {
                std::stringstream ss(segment);
                T value;
                ss >> value;
                arrayValue.push_back(value);
            }

            return arrayValue;
        }

        /// @brief 指定したフラグの値を取得する
        /// @tparam T 型
        /// @param arg 取得したい値の引数タイプ
        /// @param defaultValue フラグが存在しない場合のデフォルト値
        /// @return
        template <typename T>
        std::vector<T> getFlagArrayValue(const Arg &arg, const std::vector<T> &defaultValue)
        {
            try
            {
                return getFlagArrayValue<T>(arg);
            }
            catch (const std::runtime_error &e)
            {
                return defaultValue;
            }
        }

        std::string GetUsage() const
        {
            std::stringstream ss;
            ss << "Usage: " << this->exeName << "[option]" << std::endl;
            ss << "Options:" << std::endl;
            for (const auto &arg : argKeys)
            {
                ss << "  " << arg.GetDisplayText() << std::endl;
            }
            return ss.str();
        }
    };
}