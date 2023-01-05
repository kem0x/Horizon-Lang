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

        static Shared<NumberValue> DefaultObject()
        {
            return std::make_shared<NumberValue>(0);
        }

        NumberValue(int value)
            : ObjectValue { RuntimeValueType::NumberValue, "Number", true }
            , Value(value)
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        };
    };
}
