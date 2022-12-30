export module Runtime.StringValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Reflection;

export
{
    struct StringValue : RuntimeValue
    {
        String Value;

        StringValue(String value)
            : RuntimeValue { RuntimeValueType::StringValue }
            , Value(value)
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        }
    };
}
