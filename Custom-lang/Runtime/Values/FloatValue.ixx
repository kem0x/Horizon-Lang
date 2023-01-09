export module Runtime.FloatValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Runtime.Callables;
import Runtime.StringValue;
import Reflection;

export
{
    struct FloatValue : ObjectValue
    {
        double Value;

        static Shared<FloatValue> DefaultObject()
        {
            return std::make_shared<FloatValue>(0);
        }

        FloatValue(double value)
            : ObjectValue { RuntimeValueType::FloatValue, "Float", true }
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
            if (other->Is<FloatValue>())
            {
                return Value == other->AsUnchecked<FloatValue>()->Value;
            }

            return false;
        }

        // virtual operators
        Shared<RuntimeValue> operator+(Shared<RuntimeValue> other) override
        {
            if (other->Is<FloatValue>())
            {
                return std::make_shared<FloatValue>(Value + other->AsUnchecked<FloatValue>()->Value);
            }

            return nullptr;
        }

        Shared<RuntimeValue> operator-(Shared<RuntimeValue> other) override
        {
            if (other->Is<FloatValue>())
            {
                return std::make_shared<FloatValue>(Value - other->AsUnchecked<FloatValue>()->Value);
            }

            return nullptr;
        }

        Shared<RuntimeValue> operator*(Shared<RuntimeValue> other) override
        {
            if (other->Is<FloatValue>())
            {
                return std::make_shared<FloatValue>(Value * other->AsUnchecked<FloatValue>()->Value);
            }

            return nullptr;
        }

        Shared<RuntimeValue> operator/(Shared<RuntimeValue> other) override
        {
            if (other->Is<FloatValue>())
            {
                return std::make_shared<FloatValue>(Value / other->AsUnchecked<FloatValue>()->Value);
            }

            return nullptr;
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
        };
    };
}
