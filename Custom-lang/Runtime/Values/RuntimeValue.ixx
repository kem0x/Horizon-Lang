export module Runtime.RuntimeValue;

import <format>;
import Safety;
import Types.Core;
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
        FunctionValue
    };

    constexpr const char* RuntimeValueTypeToString(RuntimeValueType Type)
    {

        switch (Type)
        {
        case RuntimeValueType::NullValue:
            return "NullValue";
        case RuntimeValueType::BoolValue:
            return "BoolValue";
        case RuntimeValueType::NumberValue:
            return "NumberValue";
        case RuntimeValueType::StringValue:
            return "StringValue";
        case RuntimeValueType::ObjectValue:
            return "ObjectValue";
        case RuntimeValueType::FunctionValue:
            return "FunctionValue";

        default:
            return "Unknown (?)";
        }
    }

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

            return StringExtensions::TypeName<T>() == RuntimeValueTypeToString(Type);
        }

        template <typename T>
        Shared<T> As()
        {
            if (!Is<T>())
            {
                Safety::Throw(std::format("Cannot cast a runtime value of type {} to {}.", RuntimeValueTypeToString(Type), StringExtensions::TypeName<T>()));
            }

            // might be horrible but other solutions corrupt the heap so idk
            return std::make_shared<T>(static_cast<T&>(*this));
        }

        virtual String ToString()
        {
            return std::format("{{\nType: '{}'\n}}", RuntimeValueTypeToString(Type));
        }
    };
}
