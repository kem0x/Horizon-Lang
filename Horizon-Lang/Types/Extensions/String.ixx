export module Extensions.String;

import <fstream>;
import <filesystem>;
import Types.Core;

#define DefineExtension(returnType, name, params, paramsName, body) \
    struct name##T                                                  \
    {                                                               \
        returnType operator()(params) const                         \
            body                                                    \
    };                                                              \
    constexpr name##T name = {};                                    \
    returnType operator|(params, name##T ext)                       \
    {                                                               \
        return ext paramsName;                                      \
    }

export
{
    namespace StringExtensions
    {
        constexpr auto Hash(const char* s, int off = 0) -> unsigned int
        {
            return !s[off] ? 5381 : (Hash(s, off + 1) * 33) ^ s[off];
        }

        DefineExtension(
            String, ReadFile, const std::filesystem::path path, (path), {
                std::ifstream f(path, std::ios::in | std::ios::binary);

                const auto size = file_size(path);
                String result(size, '\0');

                f.read(result.data(), size);

                return result;
            });

        DefineExtension(
            char, Shift, String& str, (str), {
                auto Value = str[0];
                str.erase(str.begin());

                return Value;
            });

        constexpr auto Replace(const String& str, StringView from, StringView to)
        {
            if (!str.contains(from))
                return str;

            String result;
            result.reserve(str.length());

            auto lastPos = str.find(from, 0);
            auto pos = lastPos;

            while (pos != String::npos)
            {
                result.append(str.data() + lastPos, pos - lastPos);
                result.append(to.data(), to.length());

                lastPos = pos + from.length();
                pos = str.find(from, lastPos);
            }

            result.append(str.data() + lastPos, str.length() - lastPos);

            return result;
        }
    }
}
