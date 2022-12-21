export module Types.FlatMap;

import <array>;
import <type_traits>;
import <initializer_list>;
import Types.Core;

export
{
    template <typename KeyType, typename ValueType, size_t Size = 0>
    struct FlatMap
    {
        using ArrayType = std::conditional_t<
            Size == 0,
            Vector<Pair<KeyType, ValueType>>,
            Array<Pair<KeyType, ValueType>, Size>>;

        ArrayType data;

        constexpr size_t size() const
        {
            return data.size();
        }

        template <typename = std::enable_if_t<std::is_same_v<Vector<Pair<KeyType, ValueType>>, ArrayType>>>
        constexpr void set(const KeyType& key, const ValueType& value)
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

        constexpr Pair<KeyType, ValueType> operator[](int index) const
        {
            return data[index];
        }
    };
}
