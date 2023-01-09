export module Runtime.EnumValue;

import <format>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Runtime.Callables;
import Runtime.StringValue;
import Reflection;

export
{
    struct EnumValue : ObjectValue
    {
        String Name;

        static Shared<EnumValue> DefaultObject()
        {
            return std::make_shared<EnumValue>("None");
        }

        EnumValue(String name)
            : ObjectValue { RuntimeValueType::EnumValue, "Enum", true }
            , Name(name)
        {
            Properties["ToString"] = std::make_shared<NativeFunction>(
                [this](Vector<Shared<RuntimeValue>> args) -> Shared<RuntimeValue>
                {
                    return std::make_shared<StringValue>(Name);
                });
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\nName: '{}'\n}}", Reflection::EnumToString(Type), Name);
        }

        bool Equals(Shared<RuntimeValue> other) override
        {
            if (other->Is<EnumValue>())
            {
                return Name == other->AsUnchecked<EnumValue>()->Name;
            }

            return false;
        }
    };
}