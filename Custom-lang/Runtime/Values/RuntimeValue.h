#pragma once
#include <format>

enum class ValueType
{
    Unknown,
    NullValue,
    BoolValue,
    NumberValue
};

constexpr const char* ValueTypeToString(ValueType Type)
{
    switch (Type)
    {
    case ValueType::NullValue:
        return "NullValue";
    case ValueType::BoolValue:
        return "BoolValue";
    case ValueType::NumberValue:
        return "NumberValue";

    default:
        return "Unknown (?)";
    }
}

struct RuntimeValue
{
    ValueType Type = ValueType::Unknown;
    bool IsConstant = false;

    RuntimeValue() = default;

    RuntimeValue(ValueType Type)
        : Type(Type)
    {
    }

    auto GetAsShared()
    {
        return std::shared_ptr<RuntimeValue>(this);
    }

    template <typename T>
    auto Is() -> bool
    {
        return std::string(typeid(T).name()).ends_with(ValueTypeToString(Type));
    }

    template <typename T>
    auto As() -> std::shared_ptr<T>
    {
        if (!Is<T>())
        {
            Throw<std::shared_ptr<T>>(std::format("Cannot cast a runtime value of type {} to {}.", ValueTypeToString(Type), typeid(T).name()));
        }

        return std::dynamic_pointer_cast<T>(GetAsShared());
    }

    virtual auto ToString() -> std::string
    {
        return std::format("{{ Type: {} }}", ValueTypeToString(Type));
    };
};
typedef std::shared_ptr<RuntimeValue> RuntimeValuePtr;