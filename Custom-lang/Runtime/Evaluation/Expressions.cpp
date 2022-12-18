#include "Expressions.h"
#include "../Interpreter.h"

auto EvalNumericBinaryExpr(NumberValuePtr left, NumberValuePtr right, std::string Operator) -> NumberValuePtr
{
    float result = 0;

    if (Operator == "+")
    {
        result = left->Value + right->Value;
    }
    else if (Operator == "-")
    {
        result = left->Value - right->Value;
    }
    else if (Operator == "*")
    {
        result = left->Value * right->Value;
    }
    else if (Operator == "/")
    {
        //@todo: division by zero fail-safe
        result = left->Value / right->Value;
    }
    else if (Operator == "%")
    {
        result = (int)left->Value % (int)right->Value;
    }
    else
    {
        Throw("Tried to evalute an unknown numeric operator!!");
    }

    return std::make_shared<NumberValue>(result);
}

auto EvalBinaryExpr(BinaryExprPtr biexpr, EnviromentPtr env) -> RuntimeValuePtr
{
    auto left = Evaluate(biexpr->Left, env);
    auto right = Evaluate(biexpr->Right, env);

    if (left->Is<NumberValue>() && right->Is<NumberValue>())
    {
        return EvalNumericBinaryExpr(left->As<NumberValue>(), right->As<NumberValue>(), biexpr->Operator);
    }

    return std::make_shared<NullValue>();
}

auto EvalIdentifier(IdentifierPtr ident, EnviromentPtr env) -> RuntimeValuePtr
{
    return env->LookupVar(ident->Name);
}

auto EvalAssignment(AssignmentExprPtr node, EnviromentPtr env) -> RuntimeValuePtr
{
    if (node->Assigne->Type != NodeType::Identifier)
    {
        Throw(std::format("Invalid assignment target of {}!", node->ToString().c_str()));
    }

    const auto Name = node->Assigne->As<Identifier>()->Name;

    return env->AssignVar(Name, Evaluate(node->Value, env));
}
