export module Runtime.RuntimeValue;

import <format>;
import Safety;
import Types.Core;
import Reflection;
import Extensions.String;

export
{
    enum class RuntimeValueType
    {
        Unknown,
        NullValue,
        BoolValue,
        NumberValue,
        StringValue,
        ObjectValue,
        Callable
    };

    struct RuntimeValue : public std::enable_shared_from_this<RuntimeValue>
    {
        RuntimeValueType Type = RuntimeValueType::Unknown;
        bool IsConstant = false;

        RuntimeValue() = default;

        RuntimeValue(RuntimeValueType Type)
            : Type(Type)
        {
        }

        template <typename T>
        constexpr bool Is()
        {
            static_assert(std::is_base_of_v<RuntimeValue, T>, "T must be derived from RuntimeValue");

            return Reflection::TypeNameToString<T>() == Reflection::EnumToString(Type);
        }

        template <typename T>
        __forceinline Shared<T> AsUnchecked()
        {
            // NOTE: this slices the pointer, so the the sub class properties will be lost
            // might be horrible but other solutions corrupt the heap so i don't know...
            return std::make_shared<T>(static_cast<T&>(*this));
        }

        template <typename T>
        Shared<T> As()
        {
            if (!Is<T>())
            {
                Safety::Throw(std::format("Cannot cast a runtime value of type {} to {}.", Reflection::EnumToString(Type), Reflection::TypeNameToString<T>().c_str()));
            }
            
            return AsUnchecked<T>();
        }

        virtual String ToString()
        {
            return std::format("{{\nType: '{}'\n}}", Reflection::EnumToString(Type));
        }
    };
}
