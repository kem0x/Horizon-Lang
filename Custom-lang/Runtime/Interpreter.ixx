export module Interpreter;

import <iostream>;
import <memory>;
import <format>;
import Types.Core;
import Safety;
import AST.Core;
import AST.Statements;
import AST.Expressions;
import Runtime.ExecutionContext;
import Runtime.RuntimeValue;
import Runtime.NumberValue;
import Runtime.StringValue;
import Runtime.ObjectValue;
import Runtime.BoolValue;
import Runtime.FunctionValue;
import Runtime.NullValue;

Shared<RuntimeValue> Evaluate(Shared<Statement> node, Shared<ExecutionContext> ctx);

Shared<RuntimeValue> EvalProgram(Shared<Program> program, Shared<ExecutionContext> ctx)
{
    Shared<RuntimeValue> LastEvaluatedValue = std::make_shared<NullValue>();

    for (auto&& stmt : program->Body)
    {
        LastEvaluatedValue = std::move(Evaluate(stmt, ctx));
    }

    return LastEvaluatedValue;
}

Shared<RuntimeValue> EvalVariableDeclaration(Shared<VariableDeclaration> declaration, Shared<ExecutionContext> ctx)
{
    const auto Value = declaration->Value.has_value()
        ? Evaluate(declaration->Value.value(), ctx)
        : std::make_shared<NullValue>();

    return ctx->DeclareVar(declaration->Identifier, Value, declaration->IsConst);
}

Shared<RuntimeValue> EvalPrint(Shared<PrintStatement> print, Shared<ExecutionContext> ctx)
{
    const auto Value = Evaluate(print->Value, ctx);

    if (Value->Is<NumberValue>())
    {
        std::cout << Value->As<NumberValue>()->Value << std::endl;
    }
    else if (Value->Is<StringValue>())
    {
        std::cout << Value->As<StringValue>()->Value << std::endl;
    }
    else if (Value->Is<ObjectValue>())
    {
        std::cout << Value->As<ObjectValue>()->ToString() << std::endl;
    }
    else if (Value->Is<BoolValue>())
    {
        std::cout << Value->As<BoolValue>()->Value << std::endl;
    }
    else if (Value->Is<NullValue>())
    {
        std::cout << "null" << std::endl;
    }
    else
    {
        Safety::Throw("Tried to print an unknown value type!");
    }

    return std::make_shared<NullValue>();
}

Shared<NumberValue> EvalNumericBinaryExpr(Shared<NumberValue> left, Shared<NumberValue> right, String Operator)
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
        result = static_cast<int>(left->Value) % static_cast<int>(right->Value);
    }
    else
    {
        Safety::Throw("Tried to evalute an unknown numeric operator!!");
    }

    return std::make_shared<NumberValue>(result);
}

Shared<RuntimeValue> EvalBinaryExpr(Shared<BinaryExpr> biexpr, Shared<ExecutionContext> ctx)
{
    auto left = Evaluate(biexpr->Left, ctx);
    auto right = Evaluate(biexpr->Right, ctx);

    if (left->Is<NumberValue>() && right->Is<NumberValue>())
    {
        return EvalNumericBinaryExpr(left->As<NumberValue>(), right->As<NumberValue>(), biexpr->Operator);
    }

    return std::make_shared<NullValue>();
}

Shared<RuntimeValue> EvalIdentifier(Shared<Identifier> ident, Shared<ExecutionContext> ctx)
{
    return ctx->LookupVar(ident->Name);
}

Shared<RuntimeValue> EvalAssignment(Shared<AssignmentExpr> node, Shared<ExecutionContext> ctx)
{
    if (node->Assigne->Type != ASTNodeType::Identifier)
    {
        Safety::Throw(std::format("Invalid assignment target of {}!", node->ToString().c_str()));
    }

    const auto Name = node->Assigne->As<Identifier>()->Name;

    return ctx->AssignVar(Name, Evaluate(node->Value, ctx));
}

Shared<RuntimeValue> EvalObjectExpr(Shared<ObjectLiteral> node, Shared<ExecutionContext> ctx)
{
    auto Object = std::make_shared<ObjectValue>();

    for (auto& Prop : node->Properties)
    {
        auto Value = Prop->Value.has_value() ? Evaluate(Prop->Value.value(), ctx) : ctx->LookupVar(Prop->Key);

        Object->Properties.set(Prop->Key, Value);
    }

    return Object;
}

Shared<RuntimeValue> EvalBlockExpr(Shared<BlockExpr> node, Shared<ExecutionContext> ctx)
{
    Shared<ExecutionContext> BlockCtx = std::make_shared<ExecutionContext>(ctx);

    Shared<RuntimeValue> LastEvaluatedValue = std::make_shared<NullValue>();

    for (auto&& stmt : node->Statements)
    {
        LastEvaluatedValue = std::move(Evaluate(stmt, BlockCtx));
    }

    return LastEvaluatedValue;
}

Shared<RuntimeValue> EvalCallExpr(Shared<CallExpr> node, Shared<ExecutionContext> ctx)
{
    /*auto Callee = Evaluate(node->Callee, ctx);

    if (!Callee->Is<FunctionValue>())
    {
        Safety::Throw(std::format("Tried to call a non-function value of type {}!", Callee->ToString().c_str()));
    }

    auto Function = Callee->As<FunctionValue>();

    std::vector<Shared<RuntimeValue>> Args;

    for (auto&& Arg : node->Arguments)
    {
        Args.push_back(Evaluate(Arg, ctx));
    }

    return Function->Call(Args);*/

    return std::make_shared<NullValue>();
}

Shared<RuntimeValue> EvalIfExpr(Shared<IfExpr> node, Shared<ExecutionContext> ctx)
{
    auto Condition = Evaluate(node->Condition, ctx);

    if (Condition->Is<BoolValue>() && Condition->As<BoolValue>()->Value == true)
    {
        return Evaluate(node->Then, ctx);
    }
    else if (node->Else.has_value())
    {
        return Evaluate(node->Else.value(), ctx);
    }

    return std::make_shared<NullValue>();
}

export Shared<RuntimeValue> Evaluate(Shared<Statement> node, Shared<ExecutionContext> ctx)
{
    switch (node->Type)
    {
    case ASTNodeType::NumericLiteral:
    {
        auto Value = node->As<NumericLiteral>()->Value;
        return std::make_shared<NumberValue>(Value);
    }

    case ASTNodeType::StringLiteral:
    {
        auto Value = node->As<StringLiteral>()->Value;
        return std::make_shared<StringValue>(Value);
    }

    case ASTNodeType::Identifier:
    {
        return EvalIdentifier(node->As<Identifier>(), ctx);
    }

    case ASTNodeType::PrintStatement:
    {
        return EvalPrint(node->As<PrintStatement>(), ctx);
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

    case ASTNodeType::IfExpr:
    {
        return EvalIfExpr(node->As<IfExpr>(), ctx);
    }

    case ASTNodeType::CallExpr:
    {
        return EvalCallExpr(node->As<CallExpr>(), ctx);
    }

    case ASTNodeType::BlockExpr:
    {
        return EvalBlockExpr(node->As<BlockExpr>(), ctx);
    }

    default:
    {
        return Safety::Throw<Shared<RuntimeValue>>(std::format("Unsupported AST Node {}.", node->ToString()));
    }
    }
}
