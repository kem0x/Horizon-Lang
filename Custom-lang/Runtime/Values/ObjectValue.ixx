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
        }

        ObjectValue(String typeName)
            : RuntimeValue { RuntimeValueType::ObjectValue, typeName }
        {
        }

        ObjectValue(RuntimeValueType type, String typeName, bool isConst = false)
            : RuntimeValue(type, typeName, isConst)
        {
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
