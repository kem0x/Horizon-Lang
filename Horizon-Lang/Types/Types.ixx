export module Types.Core;

import <memory>;
import <string>;
import <optional>;
import <vector>;
import <array>;
import <unordered_map>;
import <list>;

export
{
    template <typename T>
    using Shared = std::shared_ptr<T>;

    template <typename T>
    using Unique = std::unique_ptr<T>;

    template <typename T>
    using Weak = std::weak_ptr<T>;

    template <typename T>
    using Optional = std::optional<T>;

    template <typename T>
    using Vector = std::vector<T>;

    template <class T, std::size_t N>
    using Array = std::array<T, N>;

    template <class T>
    using List = std::list<T>;

    template <typename K, typename V>
    using UnorderedMap = std::unordered_map<K, V>;

    template <typename T, typename U>
    using Pair = std::pair<T, U>;

    using String = std::string;

    using StringView = std::string_view;
}
