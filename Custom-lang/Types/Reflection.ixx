export module Reflection;

import <type_traits>;
import Types.Core;
import Types.ConstexprString;
import Extensions.String;

constexpr size_t ENUM_MAX_RANGE = 40;

// @todo: replace this in future to produce better code
template <auto Start, auto End, auto Inc, class F>
constexpr void ConstexprFor(F&& f)
{
    if constexpr (Start < End)
    {
        f(std::integral_constant<decltype(Start), Start>());
        ConstexprFor<Start + Inc, End, Inc>(f);
    }
}

template <typename E, E V>
consteval auto TemplateArgsToString()
{
    ConstexprString Sig(__FUNCSIG__);

    return ConstexprString(Sig.data, Sig.Find("<") + 1, Sig.Find(">") - 1);
}

template <typename E, E V>
    requires std::is_enum_v<E>
constexpr bool EnumIndexIsValid() noexcept
{
    auto Args = TemplateArgsToString<E, V>();

    return !Args.Contains(")");
}

template <typename E>
    requires std::is_enum_v<E>
constexpr size_t EnumSize() noexcept
{
    size_t count = 0;

    ConstexprFor<0, ENUM_MAX_RANGE, 1>(
        [&](auto i)
        {
            if (EnumIndexIsValid<E, static_cast<E>(i.value)>())
            {
                count++;
            }
        });

    return count;
}

template <typename E, size_t N>
    requires std::is_enum_v<E>
constexpr auto EnumValuesNames() noexcept
{
    Array<StringView, N> Names;

    ConstexprFor<0, N, 1>(
        [&](auto I)
        {
            constexpr auto Args = TemplateArgsToString<E, static_cast<E>(I.value)>();

            constexpr auto SplitOff = Args.Find("::") != -1 ? Args.Find("::") + 2 : Args.Find(",") + 1;

            constexpr auto static Name = ConstexprString(Args.data, SplitOff, Args.Size());

            Names[I] = Name.Data();
        });

    return Names;
}

export
{
    namespace Reflection
    {
        template <typename T>
        constexpr String TypeNameToString() noexcept
        {
            auto TName = StringExtensions::Replace(typeid(T).name(), "class ", "");
            TName = StringExtensions::Replace(TName, "struct ", "");

            return TName;
        }

        template <typename E>
            requires std::is_enum_v<E>
        constexpr StringView EnumToString(E value) noexcept
        {
            constexpr auto Size = EnumSize<E>();

            auto Names = EnumValuesNames<E, Size>();

            auto Index = static_cast<size_t>(value);

            return Names[Index];
        }
    }
}