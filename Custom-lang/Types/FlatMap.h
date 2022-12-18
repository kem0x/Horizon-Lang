#pragma once
#include <array>
#include <vector>
#include <stdexcept>

template <typename KeyType, typename ValueType, size_t Size = 0>
struct FlatMap
{
    using ArrayType = std::conditional<
        Size == 0,
        std::vector<std::pair<KeyType, ValueType>>,
        std::array<std::pair<KeyType, ValueType>, Size>>::type;

    ArrayType data;

    template <typename = std::enable_if_t<std::is_same<std::vector<std::pair<KeyType, ValueType>>, ArrayType>::value>>
    constexpr auto set(const KeyType& key, const ValueType& value) -> void
    {
        if (has(key))
        {
            for (auto&& [k, v] : data)
            {
                if (k == key)
                {
                    v = value;
                    return;
                }
            }
        }
        else
        {
            data.push_back({ key, value });
        }
    }

    constexpr ValueType at(KeyType key) const
    {
        for (auto& [k, v] : data)
        {
            if (k == key)
            {
                return v;
            }
        }

        return {};
    }

    constexpr bool has(KeyType key) const
    {
        for (auto& [k, v] : data)
        {
            if (k == key)
            {
                return true;
            }
        }

        return false;
    }

    constexpr ValueType operator[](const KeyType& key) const
    {
        return at(key);
    }
};