#include "Statements.h"

auto EvalProgram(Shared<Program> program, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    Shared<RuntimeValue> LastEvaluatedValue = std::make_shared<NullValue>();

    for (auto&& stmt : program->Body)
    {
        LastEvaluatedValue = std::move(Evaluate(stmt, env));
    }

    return LastEvaluatedValue;
}

auto EvalVariableDeclaration(Shared<VariableDeclaration> declaration, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    const auto Value = declaration->Value.has_value()
        ? Evaluate(declaration->Value.value(), env)
        : std::make_shared<NullValue>();

    return env->DeclareVar(declaration->Identifier, Value, declaration->IsConst);
}