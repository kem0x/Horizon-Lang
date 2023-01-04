export module Runtime.BoolValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Reflection;

export
{
    //@todo: change this to a runtime enum
    struct BoolValue : ObjectValue
    {
        bool Value;

        BoolValue(bool value)
            : ObjectValue { "Bool", RuntimeValueType::BoolValue, true }
            , Value(value)
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        };
    };
}
