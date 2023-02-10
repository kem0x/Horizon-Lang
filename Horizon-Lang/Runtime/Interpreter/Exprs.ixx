export module Interpreter:Exprs;

import <format>;
import Types.Core;
import Safety;
import Reflection;
import AST.Core;
import AST.Expressions;
import Runtime.ExecutionContext;
import Runtime.RuntimeValue;
import Runtime.IntValue;
import Runtime.FloatValue;
import Runtime.ObjectValue;
import Runtime.Callables;
import Runtime.StringValue;
import Runtime.BoolValue;
import Runtime.NullValue;
import Runtime.EnumValue;
import Runtime.ArrayValue;

Shared<RuntimeValue> Evaluate(Shared<Statement> node, Shared<ExecutionContext> ctx);

Shared<RuntimeValue> EvalBinaryExpr(Shared<BinaryExpr> node, Shared<ExecutionContext> ctx)
{
    auto left = Evaluate(node->Left, ctx);
    auto right = Evaluate(node->Right, ctx);

    if (left->Type != right->Type)
    {
        Safety::Throw("Cannot perform binary operation on different types");
    }

    switch (node->Operator)
    {
    case '+':
        return NullValue::ValueOrNull(left->operator+(right));
    case '-':
        return NullValue::ValueOrNull(left->operator-(right));
    case '*':
        return NullValue::ValueOrNull(left->operator*(right));
    case '/':
        return NullValue::ValueOrNull(left->operator/(right));
    case '%':
        return NullValue::ValueOrNull(left->operator%(right));
    default:
        Safety::Throw("Tried to evalute an unknown numeric operator!!");
    }

    return std::make_shared<NullValue>();
}

Shared<RuntimeValue> EvalObjectExpr(Shared<ObjectLiteral> node, Shared<ExecutionContext> ctx)
{
    auto Object = std::make_shared<ObjectValue>();

    for (auto& Prop : node->Properties)
    {
        auto Value = Prop->Value.has_value() ? Evaluate(Prop->Value.value(), ctx) : ctx->LookupVar(Prop->Key);

        Object->Properties.insert_or_assign(Prop->Key, Value);
    }

    return Object;
}

Shared<RuntimeValue> EvalArrayExpr(Shared<ArrayLiteral> node, Shared<ExecutionContext> ctx)
{
    auto Array = std::make_shared<ArrayValue>("Any");

    for (auto& Element : node->Elements)
    {
        Array->Elements.push_back(Evaluate(Element, ctx));
    }

    return Array;
}

Shared<RuntimeValue> EvalMemberExpr(Shared<MemberExpr> node, Shared<ExecutionContext> ctx)
{
    auto Object = ctx->LookupVar(node->Object->As<Identifier>()->Name);

    Shared<RuntimeValue> Expr = nullptr;

    if (node->Property->Type != ASTNodeType::Identifier)
    {
        Expr = Evaluate(node->Property, ctx);
    }

    if (Object->Is<ArrayValue>())
    {
        if (!node->Computed)
        {
            Safety::Throw("Array member access must be computed");
        }

        if (!Expr->Is<IntValue>())
        {
            Safety::Throw("Array index must be a number");
        }

        auto Array = Object->As<ArrayValue>();

        auto Index = Expr->As<IntValue>()->Value;

        if (Index < 0 or Index >= Array->Elements.size())
        {
            Safety::Throw("Array index out of bounds");
        }

        return Array->Elements[Index];
    }
    else
    {
        String PropertyName;

        if (node->Computed)
        {
            if (!Expr->Is<StringValue>())
            {
                Safety::Throw("Tried to access a member with a non-string or non-numeric property name!");
            }

            PropertyName = Expr->As<StringValue>()->Value;
        }
        else
        {
            if (node->Property->Type != ASTNodeType::Identifier)
            {
                Safety::Throw("Member access expression didn't return an identifier");
            }

            PropertyName = node->Property->As<Identifier>()->Name;
        }

        if (!Object->Is<ObjectValue>() || !Object->As<ObjectValue>()->Properties.contains(PropertyName))
        {
            Safety::Throw("Tried to access a member that doesn't exist!");
        }

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
    auto Instance = std::make_shared<ObjectValue>(this->Name);

    auto NewContext = std::make_shared<ExecutionContext>(context);
    NewContext->IsFunctionContext = true;

    for (auto&& method : Methods)
    {
        Instance->Properties.insert_or_assign(method->Name, std::make_shared<RuntimeFunction>(method, Instance));
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

        const auto ParamName = Param.second;

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

        return Evaluate(node->Right, ctx);
    }
    case LexerTokenType::Or:
    {
        if (IsTruthy(Left))
        {
            return Left;
        }

        return Evaluate(node->Right, ctx);
    }
    case LexerTokenType::EqualEqual:
    {
        auto Right = Evaluate(node->Right, ctx);

        return std::make_shared<BoolValue>(Left->Equals(Right));
    }
    case LexerTokenType::BangEqual:
    {
        auto Right = Evaluate(node->Right, ctx);

        return std::make_shared<BoolValue>(!Left->Equals(Right));
    }

    default:
        auto Right = Evaluate(node->Right, ctx);

        if (Left->Is<IntValue>() and Right->Is<IntValue>())
        {
            switch (node->Operator)
            {
            case LexerTokenType::Greater:
            {
                if (Left->As<IntValue>()->Value > Right->As<IntValue>()->Value)
                {
                    return std::make_shared<BoolValue>(true);
                }

                return std::make_shared<BoolValue>(false);
            }

            case LexerTokenType::GreaterEqual:
            {
                if (Left->As<IntValue>()->Value >= Right->As<IntValue>()->Value)
                {
                    return std::make_shared<BoolValue>(true);
                }

                return std::make_shared<BoolValue>(false);
            }

            case LexerTokenType::Less:
            {
                if (Left->As<IntValue>()->Value < Right->As<IntValue>()->Value)
                {
                    return std::make_shared<BoolValue>(true);
                }

                return std::make_shared<BoolValue>(false);
            }

            case LexerTokenType::LessEqual:
            {
                if (Left->As<IntValue>()->Value <= Right->As<IntValue>()->Value)
                {
                    return std::make_shared<BoolValue>(true);
                }

                return std::make_shared<BoolValue>(false);
            }
            default:
                Safety::Throw(std::format("Tried to evaluate an unknown logical operator {}!", Reflection::EnumToString(node->Operator)));
            }
        }
    }

    return Evaluate(node->Right, ctx);
}
