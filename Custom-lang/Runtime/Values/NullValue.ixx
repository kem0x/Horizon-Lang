export module Runtime.NullValue;

import Types.Core;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;

export
{
    //@todo: change this to a runtime enum
    struct NullValue : public ObjectValue
    {
        static Shared<NullValue> DefaultObject()
        {
            return std::make_shared<NullValue>();
        }

        NullValue()
            : ObjectValue { RuntimeValueType::NullValue, "Null", true }
        {
        }
    };
}
