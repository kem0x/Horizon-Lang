export module Runtime.StringValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;

export
{
    struct StringValue : RuntimeValue
    {
        std::string Value;

        StringValue(std::string value)
            : RuntimeValue { RuntimeValueType::StringValue }
            , Value(value)
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", RuntimeValueTypeToString(Type), Value);
        }
    };
}
