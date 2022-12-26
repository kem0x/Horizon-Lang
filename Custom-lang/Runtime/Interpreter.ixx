export module Interpreter;

import <iostream>;
import <memory>;
import <format>;
import Types.Core;
import Safety;
import Lexer;
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

bool IsTruthy(Shared<RuntimeValue> value)
{
    if (value->Is<NullValue>() || (value->Is<BoolValue>() && value->As<BoolValue>()->Value == false))
    {
        return false;
    }

    return true;
}

Shared<RuntimeValue> EvalIfExpr(Shared<IfExpr> node, Shared<ExecutionContext> ctx)
{
    if (IsTruthy(Evaluate(node->Condition, ctx)))
    {
        return Evaluate(node->Then, ctx);
    }
    else if (node->Else.has_value())
    {
        return Evaluate(node->Else.value(), ctx);
    }

    return std::make_shared<NullValue>();
}

bool Equals(Shared<RuntimeValue> left, Shared<RuntimeValue> right)
{
    if (left->Is<NullValue>() && right->Is<NullValue>())
    {
        return true;
    }

    if (left->Is<NumberValue>() && right->Is<NumberValue>())
    {
        return left->As<NumberValue>()->Value == right->As<NumberValue>()->Value;
    }

    if (left->Is<StringValue>() && right->Is<StringValue>())
    {
        return left->As<StringValue>()->Value == right->As<StringValue>()->Value;
    }

    if (left->Is<BoolValue>() && right->Is<BoolValue>())
    {
        return left->As<BoolValue>()->Value == right->As<BoolValue>()->Value;
    }

    return false;
}

Shared<RuntimeValue> EvalLogicalExpr(Shared<LogicalExpr> node, Shared<ExecutionContext> ctx)
{
    auto Left = Evaluate(node->Left, ctx);

    switch (node->Operator)
    {
    case LexerTokenType::And:
    {
        if (!IsTruthy(Left))
        {
            return Left;
        }

        break;
    }
    case LexerTokenType::Or:
    {
        if (IsTruthy(Left))
        {
            return Left;
        }

        break;
    }
    case LexerTokenType::EqualEqual:
    {
        auto Right = Evaluate(node->Right, ctx);

        return std::make_shared<BoolValue>(Equals(Left, Right));
    }
    case LexerTokenType::BangEqual:
    {
        auto Right = Evaluate(node->Right, ctx);

        return std::make_shared<BoolValue>(!Equals(Left, Right));
    }

    default:
        auto Right = Evaluate(node->Right, ctx);

        if (Left->Is<NumberValue>() && Right->Is<NumberValue>())
        {
            switch (node->Operator)
            {
            case LexerTokenType::Greater:
            {
                if (Left->As<NumberValue>()->Value > Right->As<NumberValue>()->Value)
                {
                    return Left;
                }

                break;
            }

            case LexerTokenType::GreaterEqual:
            {
                if (Left->As<NumberValue>()->Value >= Right->As<NumberValue>()->Value)
                {
                    return Left;
                }

                break;
            }

            case LexerTokenType::Less:
            {
                if (Left->As<NumberValue>()->Value < Right->As<NumberValue>()->Value)
                {
                    return Left;
                }

                break;
            }

            case LexerTokenType::LessEqual:
            {
                if (Left->As<NumberValue>()->Value <= Right->As<NumberValue>()->Value)
                {
                    return Left;
                }

                break;
            }
            default:
                Safety::Throw(std::format("Tried to evaluate an unknown logical operator {}!", static_cast<int>(node->Operator)));
            }
        }
    }

    return Evaluate(node->Right, ctx);
}

Shared<RuntimeValue> EvalBreak(Shared<BreakStatement> node, Shared<ExecutionContext> ctx)
{
    ctx->ShouldBreak = true;

    if (ctx->Parent.has_value() && !ctx->IsLoopContext)
    {
        ctx->Parent.value()->ShouldBreak = true;

        return EvalBreak(node, ctx->Parent.value());
    }

    return std::make_shared<NullValue>();
}

Shared<RuntimeValue> EvalContinue(Shared<ContinueStatement> node, Shared<ExecutionContext> ctx)
{
    ctx->ShouldContinue = true;

    if (ctx->Parent.has_value() && !ctx->IsLoopContext)
    {
        ctx->Parent.value()->ShouldContinue = true;

        return EvalContinue(node, ctx->Parent.value());
    }

    return std::make_shared<NullValue>();
}

Shared<RuntimeValue> EvalLoop(Shared<LoopStatement> node, Shared<ExecutionContext> ctx)
{
    auto Iterations = node->LoopCount;
    auto CurrentIteration = 0;

    while (true)
    {
        auto BlockCtx = std::make_shared<ExecutionContext>(ctx);

        BlockCtx->IsLoopContext = true;

        for (auto&& stmt : node->Body)
        {
            Evaluate(stmt, BlockCtx);

            if (BlockCtx->ShouldContinue)
            {
                break;
            }
        }

        if (BlockCtx->ShouldBreak)
        {
            break;
        }

        if (Iterations > 0)
        {
            CurrentIteration++;

            if (CurrentIteration == Iterations)
            {
                break;
            }
        }
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

    case ASTNodeType::LoopStatement:
    {
        return EvalLoop(node->As<LoopStatement>(), ctx);
    }

    case ASTNodeType::BreakStatement:
    {
        return EvalBreak(node->As<BreakStatement>(), ctx);
    }

    case ASTNodeType::ContinueStatement:
    {
        return EvalContinue(node->As<ContinueStatement>(), ctx);
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

    case ASTNodeType::LogicalExpr:
    {
        return EvalLogicalExpr(node->As<LogicalExpr>(), ctx);
    }

    default:
    {
        return Safety::Throw<Shared<RuntimeValue>>(std::format("Unsupported AST Node {}.", node->ToString()));
    }
    }
}
