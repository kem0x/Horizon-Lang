#include "Expressions.h"
#include "../Interpreter.h"

auto EvalNumericBinaryExpr(Shared<NumberValue> left, Shared<NumberValue> right, String Operator) -> Shared<NumberValue>
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
        Safety::Throw("Tried to evalute an unknown numeric operator!!");
    }

    return std::make_shared<NumberValue>(result);
}

auto EvalBinaryExpr(Shared<BinaryExpr> biexpr, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    auto left = Evaluate(biexpr->Left, env);
    auto right = Evaluate(biexpr->Right, env);

    if (left->Is<NumberValue>() && right->Is<NumberValue>())
    {
        return EvalNumericBinaryExpr(left->As<NumberValue>(), right->As<NumberValue>(), biexpr->Operator);
    }

    return std::make_shared<NullValue>();
}

auto EvalIdentifier(Shared<Identifier> ident, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    return env->LookupVar(ident->Name);
}

auto EvalAssignment(Shared<AssignmentExpr> node, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    if (node->Assigne->Type != ASTNodeType::Identifier)
    {
        Safety::Throw(std::format("Invalid assignment target of {}!", node->ToString().c_str()));
    }

    const auto Name = node->Assigne->As<Identifier>()->Name;

    return env->AssignVar(Name, Evaluate(node->Value, env));
}
