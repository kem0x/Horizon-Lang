export module Runtime.NullValue;

import <memory>;
import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Runtime.StringValue;

export
{
    //@todo: change this to a runtime enum
    struct NullValue : public ObjectValue
    {
        static Shared<NullValue> DefaultObject()
        {
            return std::make_shared<NullValue>();
        }

        bool Equals(Shared<RuntimeValue> other) override
        {
            return other->Is<NullValue>();
        }

        static Shared<RuntimeValue> ValueOrNull(Shared<RuntimeValue> value)
        {
            if (value)
            {
                return value;
            }

            return std::make_shared<NullValue>();
        }

        NullValue()
            : ObjectValue { RuntimeValueType::NullValue, "Null", true }
        {
            /*Properties["ToString"] = std::make_shared<NativeFunction>(
                [this](Vector<Shared<RuntimeValue>> args) -> Shared<RuntimeValue>
                {
                    return std::make_shared<StringValue>("null");
                });*/
        }
    };
}
