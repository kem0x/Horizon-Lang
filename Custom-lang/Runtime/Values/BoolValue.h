#pragma once
#include "RuntimeValue.h"

struct BoolValue : public RuntimeValue
{
    bool Value;

    BoolValue(bool value)
        : Value(value)
        , RuntimeValue { RuntimeValueType::BoolValue }
    {
    }

    virtual auto ToString() -> String
    {
        return std::format("{{\nType: '{}',\nValue: '{}'\n}}", RuntimeValueTypeToString(Type), Value);
    };
};