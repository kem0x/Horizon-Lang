export module Runtime.FunctionValue;

import <functional>;
import <format>;
import Types.Core;
import AST.Core;
import AST.Expressions;
import Runtime.RuntimeValue;

export
{
    struct FunctionValue : RuntimeValue
    {
        Shared<BlockExpr> Function;

        FunctionValue(Shared<BlockExpr> function)
            : RuntimeValue { RuntimeValueType::FunctionValue }
            , Function(std::move(function))
        {
        }

        String ToString() override
        {
            return std::format("{{\nType: '{}',\n}}", RuntimeValueTypeToString(Type));
        };
    };
}
