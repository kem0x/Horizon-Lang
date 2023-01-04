export module Runtime.NullValue;

import Runtime.RuntimeValue;
import Runtime.ObjectValue;

export
{
    //@todo: change this to a runtime enum
    struct NullValue : public ObjectValue
    {
        NullValue()
            : ObjectValue { "Null", RuntimeValueType::NullValue, true }
        {
        }
    };
}
