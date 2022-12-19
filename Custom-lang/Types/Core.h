#include <memory>
#include <vector>
#include <array>
#include <optional>
#include <vector>
#include <string>
#include <tuple>
#include <format>
#include <string_view>

template <typename T>
using Shared = std::shared_ptr<T>;

#define MakeShared std::make_shared

template <typename T>
using Unique = std::unique_ptr<T>;

#define MakeUnique std::make_unique

template <typename T>
using Weak = std::weak_ptr<T>;

template <typename T>
using Optional = std::optional<T>;

#define MakeOptional std::make_optional

template <typename T>
using Vector = std::vector<T>;

template <typename T, size_t Size>
using Array = std::array<T, Size>;

template <typename T, typename U>
using Pair = std::pair<T, U>;

using String = std::string;

using StringView = std::string_view;

#define Format std::format