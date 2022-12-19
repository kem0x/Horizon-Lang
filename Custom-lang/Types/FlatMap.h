#pragma once
#include <array>
#include <vector>
#include <stdexcept>

template <typename KeyType, typename RuntimeValueType, size_t Size = 0>
struct FlatMap
{
    using ArrayType = std::conditional<
        Size == 0,
        Vector<std::pair<KeyType, RuntimeValueType>>,
        std::array<std::pair<KeyType, RuntimeValueType>, Size>>::type;

    ArrayType data;

    template <typename = std::enable_if_t<std::is_same<Vector<std::pair<KeyType, RuntimeValueType>>, ArrayType>::value>>
    constexpr auto set(const KeyType& key, const RuntimeValueType& value) -> void
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

    constexpr RuntimeValueType at(KeyType key) const
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

    constexpr RuntimeValueType operator[](const KeyType& key) const
    {
        return at(key);
    }
};