export module Runtime.NumberValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Reflection;

export
{
    struct NumberValue : RuntimeValue
    {
        float Value;

        NumberValue(float value)
            : RuntimeValue { RuntimeValueType::NumberValue }
            , Value(value)
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        };
    };
}
