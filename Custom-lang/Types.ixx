export module Types;

import<memory>;
import<string>;
import<optional>;
import<array>;
import<vector>;

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

    template <typename T, size_t Size>
    using Array = std::array<T, Size>;

    template <typename T, typename U>
    using Pair = std::pair<T, U>;

    using String = std::string;

    using StringView = std::string_view;
}