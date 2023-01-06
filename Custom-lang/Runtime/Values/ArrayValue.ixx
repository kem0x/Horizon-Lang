export module Runtime.ArrayValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
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
