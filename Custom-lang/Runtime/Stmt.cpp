module Interpreter;

import<format>;
import Types.Core;
import Safety;
import AST.Core;
import AST.Expressions;
import AST.Statements;
import Runtime.ExecutionContext;
import Runtime.RuntimeValue;
import Runtime.NumberValue;
import Runtime.NullValue;
import Runtime.ObjectValue;

auto EvalProgram(Shared<Program> program, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>
{
    Shared<RuntimeValue> LastEvaluatedValue = std::make_shared<NullValue>();

    for (auto&& stmt : program->Body)
    {
        LastEvaluatedValue = std::move(Evaluate(stmt, ctx));
    }

    return LastEvaluatedValue;
}

auto EvalVariableDeclaration(Shared<VariableDeclaration> declaration, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>
{
    const auto Value = declaration->Value.has_value()
        ? Evaluate(declaration->Value.value(), ctx)
        : std::make_shared<NullValue>();

    return ctx->DeclareVar(declaration->Identifier, Value, declaration->IsConst);
}
