export module Runtime.RuntimeValue;

import<format>;
import Safety;
import Types.Core;

export
{
    enum class RuntimeValueType
    {
        Unknown,
        NullValue,
        BoolValue,
        NumberValue,
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
        auto Is() -> bool
        {
            return String(typeid(T).name()).ends_with(RuntimeValueTypeToString(Type));
        }

        template <typename T>
        auto As() -> Shared<T>
        {
            if (!Is<T>())
            {
                Safety::Throw(std::format("Cannot cast a runtime value of type {} to {}.", RuntimeValueTypeToString(Type), typeid(T).name()));
            }

            // might be horrible but other solutions corrupt the heap so idk
            return std::make_shared<T>((T&)*this);
        }

        virtual auto ToString() -> String
        {
            return std::format("{{\nType: '{}'\n}}", RuntimeValueTypeToString(Type));
        };
    };
}