export module Runtime.NullValue;

import Runtime.RuntimeValue;

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
