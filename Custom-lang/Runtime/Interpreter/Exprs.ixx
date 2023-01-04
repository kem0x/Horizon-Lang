export module Interpreter:Exprs;

import Types.Core;
import Safety;
import Reflection;
import AST.Core;
import AST.Expressions;
import Runtime.ExecutionContext;
import Runtime.RuntimeValue;
import Runtime.NumberValue;
import Runtime.Callables;
import Runtime.StringValue;
import Runtime.BoolValue;
import Runtime.NullValue;

Shared<RuntimeValue> Evaluate(Shared<Statement> node, Shared<ExecutionContext> ctx);

Shared<NumberValue> EvalNumericBinaryExpr(Shared<NumberValue> left, Shared<NumberValue> right, String Operator)
{
    int result = 0;

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
        result = left->Value % right->Value;
    }
    else
    {
        Safety::Throw("Tried to evalute an unknown numeric operator!!");
    }

    return std::make_shared<NumberValue>(result);
}

export Shared<RuntimeValue> EvalBinaryExpr(Shared<BinaryExpr> biexpr, Shared<ExecutionContext> ctx)
{
    auto left = Evaluate(biexpr->Left, ctx);
    auto right = Evaluate(biexpr->Right, ctx);

    if (left->Is<NumberValue>() and right->Is<NumberValue>())
    {
        return EvalNumericBinaryExpr(left->As<NumberValue>(), right->As<NumberValue>(), biexpr->Operator);
    }

    return std::make_shared<NullValue>();
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
    String PropertyName;

    if (node->Computed)
    {
        auto Expr = Evaluate(node->Property, ctx);

        if (Expr->Is<StringValue>())
        {
            PropertyName = Expr->As<StringValue>()->Value;
        }
        else
        {
            Safety::Throw("Computed member access expression didn't return a string");
        }
    }
    else
    {
        if (node->Property->Type == ASTNodeType::Identifier)
        {
            PropertyName = node->Property->As<Identifier>()->Name;
        }
        else
        {
            Safety::Throw("Member access expression didn't return an identifier");
        }
    }

    auto Object = ctx->LookupVar(node->Object->As<Identifier>()->Name);

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

Shared<RuntimeValue> CtorFunction::Call(Shared<ExecutionContext> context, const Vector<Shared<Expr>>& arguments)
{
    auto Instance = std::make_shared<ObjectValue>();

    auto NewContext = std::make_shared<ExecutionContext>(context);
    NewContext->IsFunctionContext = true;

    for (auto&& method : Methods)
    {
        Instance->Properties.set(method->Name, std::make_shared<RuntimeFunction>(method, Instance));
    }

    if (Declaration.has_value())
    {
        NewContext->DeclareVar("this", Instance, true);

        RuntimeFunction(Declaration.value()).Call(NewContext, arguments);
    }

    return Instance;
}

Shared<RuntimeValue> RuntimeFunction::Call(Shared<ExecutionContext> context, const Vector<Shared<Expr>>& arguments)
{
    auto NewContext = std::make_shared<ExecutionContext>(context);
    NewContext->IsFunctionContext = true;

    if (Declaration->Parameters.size() != arguments.size())
    {
        Safety::Throw(std::format("Function {} expected {} arguments, but got {}!", Declaration->Name, Declaration->Parameters.size(), arguments.size()));
    }

    if (Parent.has_value())
    {
        NewContext->DeclareVar("this", Parent.value(), true);
    }

    for (int i = 0; i < Declaration->Parameters.size(); i++)
    {
        auto Param = Declaration->Parameters[i];
        auto Arg = arguments[i];

        const auto ParamName = Param->As<Identifier>()->Name;

        NewContext->DeclareVar(ParamName, Evaluate(Arg, context), false);
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

    auto Function = Callee->As<Callable>();

    if (Function->CallType == CallableType::Native)
    {
        return Function->AsUnchecked<NativeFunction>()->Call(ctx, node->Arguments);
    }

    if (Function->CallType == CallableType::Ctor)
    {
        return Function->AsUnchecked<CtorFunction>()->Call(ctx, node->Arguments);
    }

    return Function->AsUnchecked<RuntimeFunction>()->Call(ctx, node->Arguments);
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
                Safety::Throw(std::format("Tried to evaluate an unknown logical operator {}!", Reflection::EnumToString(node->Operator)));
            }
        }
    }

    return Evaluate(node->Right, ctx);
}
