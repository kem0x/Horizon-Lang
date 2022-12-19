#pragma once
#include "RuntimeValue.h"

struct NullValue : public RuntimeValue
{
    NullValue()
        : RuntimeValue { RuntimeValueType::NullValue }
    {
    }
};