#pragma once
#include <array>
#include <vector>
#include <stdexcept>

template <typename KeyType, typename ValueType, size_t Size = 0>
struct FlatMap
{
    using ArrayType = std::conditional<
        Size == 0,
        Vector<Pair<KeyType, ValueType>>,
        Array<Pair<KeyType, ValueType>, Size>>::type;

    ArrayType data;

    constexpr auto size() const -> size_t
    {
        return data.size();
    }

    template <typename = std::enable_if_t<std::is_same<Vector<Pair<KeyType, ValueType>>, ArrayType>::value>>
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

    constexpr auto operator[](int index) const -> Pair<KeyType, ValueType>
    {
        return data[index];
    }
};