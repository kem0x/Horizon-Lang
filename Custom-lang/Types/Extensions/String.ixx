export module Extensions.String;

import <fstream>;
import <filesystem>;
import Types.Core;

export
{
    namespace StringExtensions
    {
        __forceinline String ReadFile(std::filesystem::path path)
        {
            std::ifstream f(path, std::ios::in | std::ios::binary);

            const auto size = file_size(path);
            String result(size, '\0');

            f.read(result.data(), size);

            return result;
        }

        __forceinline char Shift(String& String)
        {
            auto Value = String[0];
            String.erase(String.begin());
            return Value;
        }

        __forceinline constexpr auto LEraseTillLast(StringView& str, char c)
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
