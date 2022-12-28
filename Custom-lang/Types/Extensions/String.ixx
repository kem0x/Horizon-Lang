export module Extensions.String;

import <fstream>;
import <filesystem>;
import Types.Core;

#define DefineExtension(returnType, name, params, paramsName, body) \
    template <typename T>                                           \
    struct name##T                                                  \
    {                                                               \
        returnType operator()(params) const                         \
            body                                                    \
    };                                                              \
    constexpr name##T<void> name = {};                              \
    returnType operator|(params, name##T<void> ext)                 \
    {                                                               \
        return ext paramsName;                                      \
    }

export
{
    namespace StringExtensions
    {

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

        __forceinline constexpr auto Replace(const String& str, StringView from, StringView to)
        {
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

        template <typename T>
        __forceinline constexpr String TypeName()
        {
            auto TName = Replace(typeid(T).name(), "class ", "");
            TName = Replace(TName, "struct ", "");

            return TName;
        }
    }
}
