#pragma once
#include "RuntimeValue.h"

struct NumberValue : public RuntimeValue
{
    float Value;

    NumberValue(float value)
        : Value(value)
        , RuntimeValue { RuntimeValueType::NumberValue }
    {
    }

    virtual auto ToString() -> String
    {
        return std::format("{{\nType: '{}',\nValue: '{}'\n}}", RuntimeValueTypeToString(Type), Value);
    };
};