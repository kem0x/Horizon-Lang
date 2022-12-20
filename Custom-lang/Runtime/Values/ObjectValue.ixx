export module Runtime.ObjectValue;

import<format>;
import Types.Core;
import Types.FlatMap;
import Runtime.RuntimeValue;

export
{
    struct ObjectValue : public RuntimeValue
    {
        FlatMap<String, Shared<RuntimeValue>> Properties;

        ObjectValue()
            : RuntimeValue { RuntimeValueType::ObjectValue }
        {
        }

        auto ToString() -> String override
        {
            String result = "{\n";
            for (auto i = 0; i < Properties.size(); i++)
            {
                result += std::format("\t{}: {},\n", Properties[i].first, Properties[i].second->ToString());
            }
            result += "}";
            return result;
        };
    };
}
