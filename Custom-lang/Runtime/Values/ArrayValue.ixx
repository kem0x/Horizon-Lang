export module Runtime.ArrayValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Runtime.Callables;
import Runtime.IntValue;
import Runtime.StringValue;
import Reflection;

export
{
    struct ArrayValue : public ObjectValue
    {
        String ValuesType;
        Vector<Shared<RuntimeValue>> Elements;

        static Shared<ArrayValue> DefaultObject()
        {
            return std::make_shared<ArrayValue>("Any");
        }

        ArrayValue(String valuesType)
            : ObjectValue { RuntimeValueType::ArrayValue, "Array" }
            , ValuesType(valuesType)
        {
            Properties["length"] = std::make_shared<NativeFunction>(
                [this](Vector<Shared<RuntimeValue>> args) -> Shared<RuntimeValue>
                {
                    return std::make_shared<IntValue>(Elements.size());
                });

            Properties["ToString"] = std::make_shared<NativeFunction>(
                [this](Vector<Shared<RuntimeValue>> args) -> Shared<RuntimeValue>
                {
                    String result = "[";
                    for (auto& element : Elements)
                    {
                        result += element->ToString() + ", ";
                    }
                    result += "]";
                    return std::make_shared<StringValue>(result);
                });
        }

        bool Equals(Shared<RuntimeValue> other) override
        {
            if (other->Is<ArrayValue>())
            {
                auto otherArray = other->AsUnchecked<ArrayValue>();

                if (Elements.size() != otherArray->Elements.size())
                {
                    return false;
                }

                for (size_t i = 0; i < Elements.size(); i++)
                {
                    if (!Elements[i]->Equals(otherArray->Elements[i]))
                    {
                        return false;
                    }
                }

                return true;
            }

            return false;
        }

        String ToString() override
        {
            String result = "[";
            for (auto& element : Elements)
            {
                result += std::format("{}, ", element->ToString());
            }

            result += "]";

            return result;
        }
    };
}
