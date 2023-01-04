export module Runtime.StringValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Reflection;

export
{
    struct StringValue : ObjectValue
    {
        String Value;

        StringValue(String value)
            : ObjectValue { "String", RuntimeValueType::StringValue, true }
            , Value(value)
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        }
    };
}
