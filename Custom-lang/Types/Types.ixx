export module Types.Core;

import <memory>;
import <string>;
import <optional>;
import <vector>;
import <array>;
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

    template <typename T, typename U>
    using Pair = std::pair<T, U>;

    using String = std::string;

    using StringView = std::string_view;
}
