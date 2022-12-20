module Interpreter;

import<format>;
import Types.Core;
import Logger;
import Safety;
import AST.Core;
import AST.Expressions;
import AST.Statements;
import Runtime.ExecutionContext;
import Runtime.RuntimeValue;
import Runtime.NumberValue;
import Runtime.NullValue;
import Runtime.ObjectValue;
import Runtime.FunctionValue;

auto Evaluate(Shared<Statement> node, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>;
auto EvalProgram(Shared<Program> program, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>;
auto EvalVariableDeclaration(Shared<VariableDeclaration> declaration, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>;

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

auto EvalBinaryExpr(Shared<BinaryExpr> biexpr, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>
{
    auto left = Evaluate(biexpr->Left, ctx);
    auto right = Evaluate(biexpr->Right, ctx);

    if (left->Is<NumberValue>() && right->Is<NumberValue>())
    {
        return EvalNumericBinaryExpr(left->As<NumberValue>(), right->As<NumberValue>(), biexpr->Operator);
    }

    return std::make_shared<NullValue>();
}

auto EvalIdentifier(Shared<Identifier> ident, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>
{
    return ctx->LookupVar(ident->Name);
}

auto EvalAssignment(Shared<AssignmentExpr> node, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>
{
    if (node->Assigne->Type != ASTNodeType::Identifier)
    {
        Safety::Throw(std::format("Invalid assignment target of {}!", node->ToString().c_str()));
    }

    const auto Name = node->Assigne->As<Identifier>()->Name;

    return ctx->AssignVar(Name, Evaluate(node->Value, ctx));
}

auto EvalObjectExpr(Shared<ObjectLiteral> node, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>
{
    auto Object = std::make_shared<ObjectValue>();

    for (auto& Prop : node->Properties)
    {
        auto Value = Prop->Value.has_value() ? Evaluate(Prop->Value.value(), ctx) : ctx->LookupVar(Prop->Key);

        Object->Properties.set(Prop->Key, Value);
    }

    return Object;
}

auto EvalCallExpr(Shared<CallExpr> node, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>
{
    auto Callee = Evaluate(node->Callee, ctx);

    Callee->As<FunctionValue>()->Function();

    return std::make_shared<NumberValue>(0);
}

auto Evaluate(Shared<Statement> node, Shared<ExecutionContext> ctx) -> Shared<RuntimeValue>
{
    switch (node->Type)
    {
    case ASTNodeType::NumericLiteral:
    {
        auto Value = node->As<NumericLiteral>()->Value;
        return std::make_shared<NumberValue>(Value);
    }

    case ASTNodeType::Identifier:
    {
        return EvalIdentifier(node->As<Identifier>(), ctx);
    }

    case ASTNodeType::ObjectLiteral:
    {
        return EvalObjectExpr(node->As<ObjectLiteral>(), ctx);
    }

    case ASTNodeType::BinaryExpr:
    {
        return EvalBinaryExpr(node->As<BinaryExpr>(), ctx);
    }

    case ASTNodeType::Program:
    {
        return EvalProgram(node->As<Program>(), ctx);
    }

    case ASTNodeType::VariableDeclaration:
    {
        return EvalVariableDeclaration(node->As<VariableDeclaration>(), ctx);
    }

    case ASTNodeType::AssignmentExpr:
    {
        return EvalAssignment(node->As<AssignmentExpr>(), ctx);
    }

    case ASTNodeType::CallExpr:
    {
        return EvalCallExpr(node->As<CallExpr>(), ctx);
    }

    default:
    {
        return Safety::Throw<Shared<RuntimeValue>>(std::format("Unsupported AST Node {}.", node->ToString()));
    }
    }
}
