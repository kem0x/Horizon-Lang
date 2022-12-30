export module Runtime.BoolValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Reflection;

export
{
    struct BoolValue : RuntimeValue
    {
        bool Value;

        BoolValue(bool value)
            : RuntimeValue { RuntimeValueType::BoolValue }
            , Value(value)
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        };
    };
}
