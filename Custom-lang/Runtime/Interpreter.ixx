export module Interpreter;

import<memory>;
import<format>;
import Types.Core;
import Safety;
import AST.Core;
import AST.Statements;
import AST.Expressions;
import Runtime.ExecutionContext;
import Runtime.RuntimeValue;
import Runtime.NumberValue;

export
{
    auto Evaluate(Shared<Statement> node, Shared<ExecutionContext> ctx)->Shared<RuntimeValue>;
}