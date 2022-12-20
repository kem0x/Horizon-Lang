export module NullValue;

import RuntimeValue;

export
{
    struct NullValue : public RuntimeValue
    {
        NullValue()
            : RuntimeValue { RuntimeValueType::NullValue }
        {
        }
    };
}