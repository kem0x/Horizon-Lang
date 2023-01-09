export module Runtime.IntValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Runtime.StringValue;
import Runtime.Callables;
import Reflection;

export
{
    struct IntValue : ObjectValue
    {
        int Value;

        static Shared<IntValue> DefaultObject()
        {
            return std::make_shared<IntValue>(0);
        }

        IntValue(int value)
            : ObjectValue { RuntimeValueType::IntValue, "Int", true }
            , Value(value)
        {
            Properties["ToString"] = std::make_shared<NativeFunction>(
                [this](Vector<Shared<RuntimeValue>> args) -> Shared<RuntimeValue>
                {
                    return std::make_shared<StringValue>(std::to_string(Value));
                });
        }

        bool Equals(Shared<RuntimeValue> other) override
        {
            if (other->Is<IntValue>())
            {
                return Value == other->AsUnchecked<IntValue>()->Value;
            }

            return false;
        }

        // virtual operators
        Shared<RuntimeValue> operator+(Shared<RuntimeValue> other) override
        {
            if (other->Is<IntValue>())
            {
                return std::make_shared<IntValue>(Value + other->AsUnchecked<IntValue>()->Value);
            }

            return nullptr;
        }

        Shared<RuntimeValue> operator-(Shared<RuntimeValue> other) override
        {
            if (other->Is<IntValue>())
            {
                return std::make_shared<IntValue>(Value - other->AsUnchecked<IntValue>()->Value);
            }

            return nullptr;
        }

        Shared<RuntimeValue> operator*(Shared<RuntimeValue> other) override
        {
            if (other->Is<IntValue>())
            {
                return std::make_shared<IntValue>(Value * other->AsUnchecked<IntValue>()->Value);
            }

            return nullptr;
        }

        Shared<RuntimeValue> operator/(Shared<RuntimeValue> other) override
        {
            if (other->Is<IntValue>())
            {
                return std::make_shared<IntValue>(Value / other->AsUnchecked<IntValue>()->Value);
            }

            return nullptr;
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        }
    };
}
