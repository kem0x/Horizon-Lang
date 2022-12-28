export module Runtime.FunctionValue;

import <functional>;
import <format>;
import Types.Core;
import AST.Core;
import AST.Statements;
import AST.Expressions;
import Runtime.ExecutionContext;
import Runtime.RuntimeValue;

export
{
    struct FunctionValue : RuntimeValue
    {
        Shared<FunctionDeclaration> Declaration;

        FunctionValue(Shared<FunctionDeclaration> Declaration)
            : RuntimeValue(RuntimeValueType::FunctionValue)
            , Declaration(Declaration)
        {
        }
    };
}
