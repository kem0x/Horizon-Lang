#pragma once
#include <string>

namespace StringExtensions
{
    inline auto Shift(std::string& String) -> char
    {
        auto Value = String[0];
        String.erase(String.begin());
        return Value;
    }

    static constexpr auto LEraseTillLast(std::string_view& str, char c)
    {
        auto ret = str.data();

        for (auto i = str.size() - 1; i > 0; i--)
        {
            if (str[i] == c)
            {
                ret += i + 1;
                break;
            }
        }

        return ret;
    }
}
