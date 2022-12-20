export module BoolValue;

import<format>;
import Types;
import RuntimeValue;

export
{
    struct BoolValue : RuntimeValue
    {
        bool Value;

        BoolValue(bool value)
            : Value(value)
            , RuntimeValue { RuntimeValueType::BoolValue }
        {
        }

        auto ToString() -> String override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", RuntimeValueTypeToString(Type), Value);
        };
    };
}
