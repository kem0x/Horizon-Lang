#pragma once
#include "RuntimeValue.h"

struct NumberValue : public RuntimeValue
{
    float Value;

    NumberValue(float value)
        : Value(value)
        , RuntimeValue { ValueType::NumberValue }
    {
    }

    virtual auto ToString() -> std::string
    {
        return std::format("{{ Type: {}, Value: {} }}", ValueTypeToString(Type), Value);
    };
};
typedef std::shared_ptr<NumberValue> NumberValuePtr;