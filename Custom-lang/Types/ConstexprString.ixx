export module Types.ConstexprString;

import <array>;

export
{
    template <typename T = char, unsigned long int N = 0>
    struct ConstexprString
    {
        using CharType = T;

        size_t length = N - 1;

        CharType data[N] = {};

        constexpr ConstexprString(const CharType (&s)[N]) noexcept
        {
            auto dst = data;
            for (auto src = s; src != s + N; ++src)
            {
                *dst++ = *src;
            }
        }

        constexpr ConstexprString(const CharType (&s)[N], size_t start, size_t newSize) noexcept
        {
            length = newSize - start;

            auto dst = data;
            for (auto src = s + start; src != s + newSize + 1; src++, dst++)
            {
                *dst = *src;
            }

            if (newSize < N)
            {
                *dst++ = '\0';
                length++;
            }
        }

        constexpr operator const CharType*() const noexcept
        {
            return data;
        }

        constexpr auto operator[](int i) const noexcept
        {
            return data[i];
        }

        template <size_t N>
        constexpr bool operator==(const CharType (&s)[N]) const noexcept
        {
            auto str = s;
            auto str2 = data;
            while (*str && (*str == *str2))
            {
                str++;
                str2++;
            }
            return ((*str - *str2) == 0);
        }

        constexpr auto Data() const noexcept
        {
            return data;
        }

        [[nodiscard]] constexpr size_t Size() const noexcept
        {
            return length;
        }

        constexpr auto Array() const noexcept
        {
            std::array<CharType, N> out;

            std::copy(data, data + N, out.begin());

            return out;
        }

        constexpr auto Empty() const noexcept
        {
            return (length == 0);
        }

        template <size_t N>
        constexpr bool Contains(const CharType (&s)[N]) noexcept
        {
            return Find(s) != -1;
        }

        template <std::size_t N>
        constexpr auto Find(const char (&str)[N]) const
        {
            size_t match_size = 0;

            for (auto i = 0; i < Size(); i++)
            {
                for (auto j = 0; j < N - 1; j++)
                {
                    if (data[i + j] == str[j])
                    {
                        match_size++;
                    }
                    else
                    {
                        match_size = 0;
                        break;
                    }
                }

                if (match_size == N - 1)
                {
                    return i;
                }
            }

            return -1;
        }
    };
}