#pragma once
#include "RuntimeValue.h"

struct NullValue : public RuntimeValue
{
    NullValue()
        : RuntimeValue { ValueType::NullValue }
    {
    }
};
typedef std::shared_ptr<NullValue> NullValuePtr;