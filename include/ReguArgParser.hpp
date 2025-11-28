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
    /// @brief フラグデータ
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
    static std::ostream &operator<<(std::ostream &os, const Flag &flag)
    {
        if (flag.shortFlag.has_value())
            os << flag.shortFlag.value() << ",";
        if (flag.longFlag.has_value())
            os << flag.longFlag.value();
        return os;
    }

    /// @brief 引数データ
    class Arg
    {
    private:
    public:
        const Flag flags;
        const std::string helpText;
        const std::optional<std::string> defaultValueStr = std::nullopt;
        auto operator<=>(const Arg &) const = default;
        Arg(const Flag &flags, const std::string &helpText = "", const std::optional<std::string> defaultValueStr = std::nullopt)
            : flags(flags),
              helpText(helpText),
              defaultValueStr(defaultValueStr) {}
        std::string GetDisplayText(size_t spaceCount = 2) const
        {
            std::stringstream ss;
            ss << flags;
            for (size_t i = 0; i < spaceCount; i++)
                ss << " ";
            ss << helpText;

            if (defaultValueStr.has_value())
            {
                ss << " (default: " << defaultValueStr.value() << ")";
            }
            return ss.str();
        }
    };

    /// @brief 引数パーサー
    class ArgParser
    {
    private:
        const size_t MAX_SPACE_LEN = 16;
        const std::string exeName;
        const std::vector<std::string> args = {};
        std::set<Arg> argKeys;

        void RegistArg(const Arg &arg)
        {
            argKeys.insert(arg);
        }

                template <typename T>
        T __findFlagValue(const Arg &arg)
        {
            const auto it = std::find_first_of(args.begin(), args.end(), arg.flags.flags.begin(), arg.flags.flags.end());
            const auto flagsStr = (arg.flags.shortFlag.has_value() ? arg.flags.shortFlag.value() : "") +
                                  (arg.flags.longFlag.has_value() ? arg.flags.longFlag.value() : "");
            // フラグがない場合
            if (it == args.end())
            {
                std::stringstream ss;
                ss << "Flag \"" << flagsStr << "\" not found. Usage: " << arg.GetDisplayText();
                throw std::runtime_error(ss.str());
            }

            // 値がない場合
            if (it + 1 == args.end())
            {
                std::stringstream ss;
                ss << "Value for flag \"" << flagsStr << "\" not found. Usage: " << arg.GetDisplayText();
                throw std::runtime_error(ss.str());
            }

            // 値の取得
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
        
        ///@brief 指定したフラグの値を取得する
        ///@tparam T 型
        ///@param arg 取得したい値のフラグデータ
        ///@return T型に変換された値
        ///@throws std::runtime_error フラグが存在しない、値がない、または型変換に失敗した場合
        template <typename T>
        std::vector<T> _getFlagArrayValue(const Arg &arg)
        {
            // 文字列をカンマで分割し、配列に格納
            std::vector<T> arrayValue;
            std::stringstream ssValueStr(this->__findFlagValue<std::string>(arg));
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
            return __findFlagValue<T>(arg);
        }

        /// @brief 指定したフラグの値を取得する
        /// @tparam T 型
        /// @param arg 取得したい値の引数タイプ
        /// @param defaultValue フラグが存在しない場合のデフォルト値
        /// @return
        template <typename T>
        T getFlagValue(const Arg &arg, const T &defaultValue)
        {
            std::stringstream defaultValueStr;
            defaultValueStr << defaultValue;
            Arg argWithDefault(arg.flags, arg.helpText, defaultValueStr.str()); // 引数付きで再構築
            this->RegistArg(argWithDefault);
            try
            {
                return this->__findFlagValue<T>(argWithDefault);
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
            this->RegistArg(arg);
            return this->_getFlagArrayValue<T>(arg);
        }

        /// @brief 指定したフラグの値を取得する
        /// @tparam T 型
        /// @param arg 取得したい値の引数タイプ
        /// @param defaultValue フラグが存在しない場合のデフォルト値
        /// @return
        template <typename T>
        std::vector<T> getFlagArrayValue(const Arg &arg, const std::vector<T> &defaultValue)
        {
            std::stringstream defaultValueStr;
            for (const auto &value : defaultValue)
            {
                defaultValueStr << value << ",";
            }
            defaultValueStr << defaultValue.back();
            Arg argWithDefault(arg.flags, arg.helpText, defaultValueStr.str()); // 引数付きで再構築
            this->RegistArg(argWithDefault);
            try
            {
                return this->getFlagArrayValue<T>(arg);
            }
            catch (const std::runtime_error &e)
            {
                return defaultValue;
            }
        }

        std::string GetUsage() const
        {
            // 各引数のキーの最大の文字数を求める
            size_t maxLen = 0;
            for (const auto &arg : argKeys)
            {
                std::stringstream argss;
                argss << arg.flags;
                maxLen = std::max(maxLen, argss.str().length());
            }
            maxLen = std::min(maxLen, MAX_SPACE_LEN); // 長すぎる場合は短くする
            std::stringstream ss;
            ss << "Usage: " << this->exeName << "[option]" << std::endl;
            ss << "Options:" << std::endl;

            for (const auto &arg : argKeys)
            {
                constexpr size_t space = 2;
                std::stringstream argss;
                argss << arg.flags;
                const size_t delta = maxLen - argss.str().length();
                ss << "  " << arg.GetDisplayText(delta + space) << std::endl;
            }
            return ss.str();
        }
    };
}