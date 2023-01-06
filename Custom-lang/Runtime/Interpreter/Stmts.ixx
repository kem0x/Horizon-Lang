export module Interpreter:Stmts;

import <iostream>;
import <memory>;
import <format>;
import <thread>;
import Logger;
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
import Runtime.ObjectValue;
import Runtime.ArrayValue;
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
    auto Value = declaration->Value.has_value()
        ? Evaluate(declaration->Value.value(), ctx)
        : std::make_shared<NullValue>();

    if (Value->Is<NullValue>())
    {
        Value = ctx->LookupVar(declaration->TypeName); // Default value
    }

    if (declaration->TypeName != Value->TypeName and declaration->TypeName != "Any")
    {
        Safety::Throw(std::format("Type mismatch: '{}' is not '{}'", Value->TypeName, declaration->TypeName));
    }

    return ctx->DeclareVar(declaration->Identifier, Value, declaration->IsConst);
}

Shared<RuntimeValue> EvalIdentifier(Shared<Identifier> ident, Shared<ExecutionContext> ctx)
{
    return ctx->LookupVar(ident->Name);
}

Shared<RuntimeValue> EvalAssignment(Shared<AssignmentExpr> node, Shared<ExecutionContext> ctx)
{
    if (node->Assigne->Type != ASTNodeType::Identifier && node->Assigne->Type != ASTNodeType::MemberExpr)
    {
        Safety::Throw(std::format("Invalid assignment target of {}!", node->Assigne->ToString().c_str()));
    }

    if (node->Assigne->Type == ASTNodeType::Identifier)
    {
        const auto Ident = node->Assigne->As<Identifier>();
        const auto Var = ctx->LookupVar(Ident->Name);
        auto Value = Evaluate(node->Value, ctx);

        if (Value->Is<NullValue>())
        {
            Value = ctx->LookupVar(Var->TypeName); // Default value
        }

        if (Var->TypeName != Value->TypeName
            and Var->TypeName != "Any")
        {
            Safety::Throw(std::format("Type mismatch in assignment of {} to {}!", Var->TypeName, Value->TypeName));
        }

        return ctx->AssignVar(Ident->Name, Value);
    }

    const auto Member = node->Assigne->As<MemberExpr>();
    const auto Value = Evaluate(node->Value, ctx);

    auto Object = Evaluate(Member->Object, ctx);

    if (Object->Is<NullValue>())
    {
        Safety::Throw(std::format("Cannot access member of null value!"));
    }

    if (Object->Is<ArrayValue>())
    {
        const auto Array = Object->As<ArrayValue>();
        const auto Index = Evaluate(Member->Property, ctx);

        if (!Index->Is<NumberValue>())
        {
            Safety::Throw(std::format("Array index must be a number!"));
        }

        const auto IndexValue = Index->As<NumberValue>()->Value;

        if (IndexValue < 0 || IndexValue >= Array->Elements.size())
        {
            Safety::Throw(std::format("Array index out of bounds!"));
        }

        Array->Elements[IndexValue] = Value;

        return Value;
    }

    if (!Object->Is<ObjectValue>())
    {
        Safety::Throw("Tried to assign to a non-object!");
    }

    auto ObjectVal = Object->As<ObjectValue>();

    /*if (ObjectVal->IsConstant)
    {
        Safety::Throw("Tried to assign to a const object!");
    }*/

    const auto Ident = Member->Property->As<Identifier>();

    ObjectVal->Properties.insert_or_assign(Ident->Name, Value);

    return std::make_shared<NullValue>();
}

Shared<RuntimeValue> EvalClassDeclaration(Shared<ClassDeclaration> node, Shared<ExecutionContext> ctx)
{
    Optional<Shared<FunctionDeclaration>> Constructor = std::nullopt;
    Vector<Shared<FunctionDeclaration>> Methods = {};

    for (auto&& member : node->Body)
    {
        if (member->Is<FunctionDeclaration>())
        {
            auto Function = member->As<FunctionDeclaration>();

            if (Function->Name == node->Name)
            {
                Constructor = Function;
            }
            else
            {
                Methods.push_back(Function);
            }
        }
        else
        {
            Safety::Throw("Tried to declare a class with a non-function member!");
        }
    }

    auto Class = std::make_shared<CtorFunction>(node->Name, Constructor, Methods);

    ctx->DeclareVar(node->Name, Class, false);

    return Class;
}

Shared<RuntimeValue> EvalEnumDeclaration(Shared<EnumDeclaration> node, Shared<ExecutionContext> ctx)
{
    auto Enum = std::make_shared<EnumValue>(node->Name);

    for (auto&& member : node->Body)
    {
        if (member->Is<EnumMember>())
        {
            auto Member = member->As<EnumMember>();

            Enum->Members.insert_or_assign(Member->Name, Member->Value);
        }
        else
        {
            Safety::Throw("Tried to declare an enum with a non-member!");
        }
    }

    ctx->DeclareVar(node->Name, Enum, false);

    return Enum;
}

Shared<RuntimeValue> EvalFunctionDeclaration(Shared<FunctionDeclaration> node, Shared<ExecutionContext> ctx)
{
    auto Function = std::make_shared<RuntimeFunction>(node);

    ctx->DeclareVar(node->Name, Function, false);

    return Function;
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

Shared<RuntimeValue> EvalSyncStatement(Shared<SyncStatement> node, Shared<ExecutionContext> ctx)
{
    return std::make_shared<RuntimeThread>(Evaluate, node->Stmt, ctx);
}

Shared<RuntimeValue> EvalDebugStatement(Shared<DebugStatement> node, Shared<ExecutionContext> ctx)
{
    for (auto&& [Name, Value] : ctx->Variables)
    {
        printf("%s: %s\n", Name.c_str(), Value->ToString().c_str());
    }

    return std::make_shared<NullValue>();
}
