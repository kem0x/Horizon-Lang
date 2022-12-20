export module Runtime.FunctionValue;

import<functional>;
import<format>;
import Types.Core;
import AST.Core;
import Runtime.RuntimeValue;

export
{
    struct FunctionValue : RuntimeValue
    {
        std::function<void()> Function;

        FunctionValue(std::function<void()> function)
            : Function(function)
            , RuntimeValue { RuntimeValueType::FunctionValue }
        {
        }

        auto ToString() -> String override
        {
            return std::format("{{\nType: '{}',\n}}", RuntimeValueTypeToString(Type));
        };
    };
}
