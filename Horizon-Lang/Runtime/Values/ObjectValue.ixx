export module Runtime.ObjectValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;

export
{
    struct ObjectValue : public RuntimeValue
    {
        UnorderedMap<String, Shared<RuntimeValue>> Properties;

        static Shared<ObjectValue> DefaultObject()
        {
            return std::make_shared<ObjectValue>();
        }

        ObjectValue()
            : RuntimeValue { RuntimeValueType::ObjectValue, "Object" }
        {
            /*Properties["ToString"] = std::make_shared<NativeFunction>(
                [this](Vector<Shared<RuntimeValue>> args) -> Shared<RuntimeValue>
                {
                    return std::make_shared<StringValue>(ToString());
                });*/
        }

        ObjectValue(String typeName)
            : RuntimeValue { RuntimeValueType::ObjectValue, typeName }
        {
        }

        ObjectValue(RuntimeValueType type, String typeName, bool isConst = false)
            : RuntimeValue(type, typeName, isConst)
        {
        }

        bool Equals(Shared<RuntimeValue> other) override
        {
            if (other->Type != Type)
                return false;

            auto otherObject = std::dynamic_pointer_cast<ObjectValue>(other);

            if (Properties.size() != otherObject->Properties.size())
                return false;

            for (auto& [key, value] : Properties)
            {
                if (!otherObject->Properties.contains(key))
                    return false;

                if (!value->Equals(otherObject->Properties[key]))
                    return false;
            }

            return true;
        }

        String ToString() override
        {
            String result = "{\n";
            for (auto& [key, value] : Properties)
            {
                result += std::format("  {} : {},\n", key, value->ToString());
            }

            result += "}";

            return result;
        };
    };
}
