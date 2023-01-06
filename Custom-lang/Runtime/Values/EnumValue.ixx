export module Runtime.EnumValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Reflection;

export
{
    struct EnumValue : ObjectValue
    {
        String Value;

        EnumValue(String value)
            : ObjectValue { RuntimeValueType::EnumValue, "Enum", true }
            , Value(value)
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        };
    };
}