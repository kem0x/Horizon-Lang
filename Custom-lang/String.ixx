export module StringExtensions;

import Types;

export
{
    namespace StringExtensions
    {
        inline auto Shift(String& String) -> char
        {
            auto Value = String[0];
            String.erase(String.begin());
            return Value;
        }

        static constexpr auto LEraseTillLast(StringView& str, char c)
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
}