export module Interpreter;

import <iostream>;
import <memory>;
import <format>;
import <thread>;
import Types.Core;
import Safety;
import Lexer;
import AST.Core;
import AST.Statements;
import AST.Expressions;
import Runtime.Thread;
import Runtime.ExecutionContext;
import Runtime.RuntimeValue;
import Runtime.NumberValue;
import Runtime.StringValue;
import Runtime.ObjectValue;
import Runtime.BoolValue;
import Runtime.Callables;
import Runtime.NullValue;

Shared<RuntimeValue> Evaluate(Shared<Statement> node, Shared<ExecutionContext> ctx);

Shared<RuntimeValue> EvalProgram(Shared<Program> program, Shared<ExecutionContext> ctx)
{
    Shared<RuntimeValue> LastEvaluatedValue = std::make_shared<NullValue>();

    for (auto&& stmt : program->Body)
    {
        LastEvaluatedValue = Evaluate(stmt, ctx);
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

    if (left->Is<NumberValue>() and right->Is<NumberValue>())
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

Shared<RuntimeValue> EvalMemberExpr(Shared<MemberExpr> node, Shared<ExecutionContext> ctx)
{
    String ObjectName;
    String PropertyName;

    if (node->Object->Type == ASTNodeType::Identifier)
    {
        ObjectName = node->Object->As<Identifier>()->Name;
    }
    else if (node->Object->Type == ASTNodeType::StringLiteral)
    {
        ObjectName = node->Object->As<StringLiteral>()->Value;
    }

    if (node->Property->Type == ASTNodeType::Identifier)
    {
        PropertyName = node->Property->As<Identifier>()->Name;
    }
    else if (node->Property->Type == ASTNodeType::StringLiteral)
    {
        PropertyName = node->Property->As<StringLiteral>()->Value;
    }

    auto Object = ctx->LookupVar(ObjectName);

    if (Object->Is<ObjectValue>() && Object->As<ObjectValue>()->Properties.has(PropertyName))
    {
        return Object->As<ObjectValue>()->Properties[PropertyName];
    }

    return std::make_shared<NullValue>();
}

Shared<RuntimeValue> EvalBlockExpr(Shared<BlockExpr> node, Shared<ExecutionContext> ctx)
{
    auto BlockCtx = std::make_shared<ExecutionContext>(ctx);

    Shared<RuntimeValue> LastEvaluatedValue = std::make_shared<NullValue>();

    for (auto&& stmt : node->Body)
    {
        LastEvaluatedValue = Evaluate(stmt, BlockCtx);
    }

    return LastEvaluatedValue;
}

Shared<RuntimeValue> RuntimeFunction::Call(Shared<ExecutionContext> context, const Vector<Shared<Expr>>& arguments)
{
    auto NewContext = std::make_shared<ExecutionContext>(context);
    NewContext->IsFunctionContext = true;

    for (int i = 0; i < Declaration->Parameters.size(); i++)
    {
        auto Param = Declaration->Parameters[i];

        for (auto&& Arg : arguments)
        {
            const auto ParamName = Param->As<Identifier>()->Name;

            NewContext->DeclareVar(ParamName, Evaluate(Arg, context), true);
        }
    }

    Shared<RuntimeValue> Result = std::make_shared<NullValue>();

    for (auto&& stmt : Declaration->Body)
    {
        Result = Evaluate(stmt, NewContext);

        if (NewContext->ShouldReturn)
        {
            return Result;
        }
    }

    return Result;
}

Shared<RuntimeValue> NativeFunction::Call(Shared<ExecutionContext> context, const Vector<Shared<Expr>>& arguments)
{
    Vector<Shared<RuntimeValue>> Args;

    for (auto&& Arg : arguments)
    {
        Args.emplace_back(Evaluate(Arg, context));
    }

    return Function(Args);
}

Shared<RuntimeValue> EvalCallExpr(Shared<CallExpr> node, Shared<ExecutionContext> ctx)
{
    auto Callee = Evaluate(node->Callee, ctx);

    if (!Callee->Is<Callable>())
    {
        Safety::Throw(std::format("Tried to call a non-function value of type {}!", Callee->ToString().c_str()));
    }

    auto Function = std::dynamic_pointer_cast<Callable>(Callee);

    if (Function->CallType == CallabeType::Native)
    {
        return Function->AsUnchecked<NativeFunction>()->Call(ctx, node->Arguments);
    }

    return Function->AsUnchecked<RuntimeFunction>()->Call(ctx, node->Arguments);
}

Shared<RuntimeValue> EvalReturnStatement(Shared<ReturnStatement> node, Shared<ExecutionContext> ctx)
{
    if (ctx->IsGlobalContext or !ctx->Parent.has_value())
    {
        Safety::Throw("Tried to return from a non-function context!");
    }

    if (ctx->IsFunctionContext)
    {
        ctx->ShouldReturn = true;

        if (node->Value.has_value())
        {
            return Evaluate(node->Value.value(), ctx);
        }

        return std::make_shared<NullValue>();
    }

    return EvalReturnStatement(node, ctx->Parent.value());
}

bool IsTruthy(Shared<RuntimeValue> value)
{
    if (value->Is<NullValue>() or (value->Is<BoolValue>() and value->As<BoolValue>()->Value == false))
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
    if (left->Is<NullValue>() and right->Is<NullValue>())
    {
        return true;
    }

    if (left->Is<NumberValue>() and right->Is<NumberValue>())
    {
        return left->As<NumberValue>()->Value == right->As<NumberValue>()->Value;
    }

    if (left->Is<StringValue>() and right->Is<StringValue>())
    {
        return left->As<StringValue>()->Value == right->As<StringValue>()->Value;
    }

    if (left->Is<BoolValue>() and right->Is<BoolValue>())
    {
        return left->As<BoolValue>()->Value == right->As<BoolValue>()->Value;
    }

    return false;
}

Shared<RuntimeValue> EvalLogicalExpr(Shared<ConditionalExpr> node, Shared<ExecutionContext> ctx)
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

        if (Left->Is<NumberValue>() and Right->Is<NumberValue>())
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

    return std::make_shared<NullValue>();
}

Shared<RuntimeValue> EvalContinue(Shared<ContinueStatement> node, Shared<ExecutionContext> ctx)
{
    ctx->ShouldContinue = true;

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

Shared<RuntimeValue> EvalFunctionDeclaration(Shared<FunctionDeclaration> node, Shared<ExecutionContext> ctx)
{
    auto Function = std::make_shared<RuntimeFunction>(node);

    if (node->Name.has_value())
    {
        ctx->DeclareVar(node->Name.value(), Function, false);
    }

    return Function;
}

Shared<RuntimeValue> EvalSyncStatement(Shared<SyncStatement> node, Shared<ExecutionContext> ctx)
{
    return std::make_shared<RuntimeThread>(Evaluate, node->Stmt, ctx);
}

export Shared<RuntimeValue> Evaluate(Shared<Statement> node, Shared<ExecutionContext> ctx)
{
    switch (node->Type)
    {
    case ASTNodeType::NumericLiteral:
        return std::make_shared<NumberValue>(node->As<NumericLiteral>()->Value);

    case ASTNodeType::StringLiteral:
        return std::make_shared<StringValue>(node->As<StringLiteral>()->Value);

    case ASTNodeType::Identifier:
        return EvalIdentifier(node->As<Identifier>(), ctx);

    case ASTNodeType::LoopStatement:
        return EvalLoop(node->As<LoopStatement>(), ctx);

    case ASTNodeType::BreakStatement:
        return EvalBreak(node->As<BreakStatement>(), ctx);

    case ASTNodeType::ContinueStatement:
        return EvalContinue(node->As<ContinueStatement>(), ctx);

    case ASTNodeType::ObjectLiteral:
        return EvalObjectExpr(node->As<ObjectLiteral>(), ctx);

    case ASTNodeType::MemberExpr:
        return EvalMemberExpr(node->As<MemberExpr>(), ctx);

    case ASTNodeType::BinaryExpr:
        return EvalBinaryExpr(node->As<BinaryExpr>(), ctx);

    case ASTNodeType::Program:
        return EvalProgram(node->As<Program>(), ctx);

    case ASTNodeType::VariableDeclaration:
        return EvalVariableDeclaration(node->As<VariableDeclaration>(), ctx);

    case ASTNodeType::AssignmentExpr:
        return EvalAssignment(node->As<AssignmentExpr>(), ctx);

    case ASTNodeType::IfExpr:
        return EvalIfExpr(node->As<IfExpr>(), ctx);

    case ASTNodeType::FunctionDeclaration:
        return EvalFunctionDeclaration(node->As<FunctionDeclaration>(), ctx);

    case ASTNodeType::CallExpr:
        return EvalCallExpr(node->As<CallExpr>(), ctx);

    case ASTNodeType::ReturnStatement:
        return EvalReturnStatement(node->As<ReturnStatement>(), ctx);

    case ASTNodeType::BlockExpr:
        return EvalBlockExpr(node->As<BlockExpr>(), ctx);

    case ASTNodeType::ConditionalExpr:
        return EvalLogicalExpr(node->As<ConditionalExpr>(), ctx);

    case ASTNodeType::SyncStatement:
        return EvalSyncStatement(node->As<SyncStatement>(), ctx);

    default:
        return Safety::Throw<Shared<RuntimeValue>>(std::format("Unsupported AST Node {}.", node->ToString()));
    }
}
