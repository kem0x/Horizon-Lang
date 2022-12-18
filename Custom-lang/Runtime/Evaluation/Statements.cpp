#include "Statements.h"

auto EvalProgram(ProgramPtr program, EnviromentPtr env) -> RuntimeValuePtr
{
    RuntimeValuePtr LastEvaluatedValue = std::make_shared<NullValue>();

    for (auto&& stmt : program->Body)
    {
        LastEvaluatedValue = std::move(Evaluate(stmt, env));
    }

    return LastEvaluatedValue;
}

auto EvalVariableDeclaration(VariableDeclarationPtr declaration, EnviromentPtr env) -> RuntimeValuePtr
{
    const auto Value = declaration->Value.has_value()
        ? Evaluate(declaration->Value.value(), env)
        : std::make_shared<NullValue>();

    return env->DeclareVar(declaration->Identifier, Value, declaration->IsConst);
}