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

        static Shared<StringValue> DefaultObject()
        {
            return std::make_shared<StringValue>("");
        }

        StringValue(String value)
            : ObjectValue { RuntimeValueType::StringValue, "String", true }
            , Value(value)
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        }
    };
}
