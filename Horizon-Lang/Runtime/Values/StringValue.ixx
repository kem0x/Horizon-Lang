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

        bool Equals(Shared<RuntimeValue> other) override
        {
            if (other->Is<StringValue>())
            {
                return Value == other->AsUnchecked<StringValue>()->Value;
            }

            return false;
        }

        //virtual operators
        Shared<RuntimeValue> operator+(Shared<RuntimeValue> other) override
        {
            if (other->Is<StringValue>())
            {
                return std::make_shared<StringValue>(Value + other->AsUnchecked<StringValue>()->Value);
            }

            return nullptr;
        }

        

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        }
    };
}
