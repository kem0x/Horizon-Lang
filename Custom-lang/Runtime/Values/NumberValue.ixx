export module Runtime.NumberValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Reflection;

export
{
    struct NumberValue : ObjectValue
    {
        int Value;

        NumberValue(int value)
            : ObjectValue { "Number", RuntimeValueType::NumberValue, true }
            , Value(value)
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        };
    };
}
