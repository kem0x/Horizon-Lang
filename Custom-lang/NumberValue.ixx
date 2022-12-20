export module NumberValue;

import<format>;
import Types;
import RuntimeValue;

export
{
    struct NumberValue : RuntimeValue
    {
        float Value;

        NumberValue(float value)
            : Value(value)
            , RuntimeValue { RuntimeValueType::NumberValue }
        {
        }

        auto ToString() -> String override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", RuntimeValueTypeToString(Type), Value);
        };
    };
}