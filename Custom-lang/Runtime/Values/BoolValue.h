#pragma once
#include "RuntimeValue.h"

struct BoolValue : public RuntimeValue
{
    bool Value;

    BoolValue(bool value)
        : Value(value)
        , RuntimeValue { ValueType::BoolValue }
    {
    }

    virtual auto ToString() -> std::string
    {
        return std::format("{{ Type: {}, Value: {} }}", ValueTypeToString(Type), Value);
    };
};
typedef std::shared_ptr<BoolValue> BoolValuePtr;