export module Runtime.BoolValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Runtime.Callables;
import Runtime.StringValue;
import Reflection;

export
{
    //@todo: change this to a runtime enum
    struct BoolValue : ObjectValue
    {
        bool Value;

        static Shared<BoolValue> DefaultObject()
        {
            return std::make_shared<BoolValue>(false);
        }

        BoolValue(bool value)
            : ObjectValue { RuntimeValueType::BoolValue, "Bool", true }
            , Value(value)
        {
            Properties["ToString"] = std::make_shared<NativeFunction>(
                [this](Vector<Shared<RuntimeValue>> args) -> Shared<RuntimeValue>
                {
                    return std::make_shared<StringValue>(Value ? "true" : "false");
                });
        }

        bool Equals(Shared<RuntimeValue> other) override
        {
            if (other->Is<BoolValue>())
            {
                return Value == other->AsUnchecked<BoolValue>()->Value;
            }

            return false;
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        };
    };
}
