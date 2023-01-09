export module Binaryen;

#pragma comment(lib, "binaryen.lib")

import "binaryen-c.h";

import <format>;
import Safety;
import Types.Core;
import Logger;
import AST.Core;
import AST.Statements;
import AST.Expressions;
import Extensions.String;

//@todo: change this to a static map
BinaryenType TypeNameToBinaryenType(String typeName)
{
    switch (StringExtensions::Hash(typeName.c_str()))
    {
    case StringExtensions::Hash("Int"):
        return BinaryenTypeInt64();

    case StringExtensions::Hash("Float"):
        return BinaryenTypeFloat64();

    case StringExtensions::Hash("Void"):
        return BinaryenTypeNone();

    default:
        Safety::Throw(std::format("Unknown type name: {}", typeName));
    }

    return BinaryenTypeNone();
}

BinaryenExpressionRef GetDefaultValue(BinaryenModuleRef module, String typeName)
{
    switch (StringExtensions::Hash(typeName.c_str()))
    {
    case StringExtensions::Hash("Int"):
        return BinaryenConst(module, BinaryenLiteralInt64(0));

    case StringExtensions::Hash("Float"):
        return BinaryenConst(module, BinaryenLiteralFloat64(0));

    default:
        Safety::Throw(std::format("Unknown type name: {}", typeName));
    }

    return nullptr;
}

BinaryenExpressionRef Compile(Shared<Statement> node, BinaryenModuleRef module);

BinaryenExpressionRef CompileProgram(Shared<Program> program, BinaryenModuleRef module)
{
    BinaryenExpressionRef LastEvaluatedValue = BinaryenNop(module);

    for (auto&& stmt : program->Body)
    {
        LastEvaluatedValue = Compile(stmt, module);
    }

    return LastEvaluatedValue;
}

BinaryenExpressionRef CompileNumericBinaryExpr(BinaryenExpressionRef left, BinaryenExpressionRef right, String Operator, BinaryenModuleRef module)
{
    BinaryenOp Op;

    if (Operator == "+")
    {
        Op = BinaryenAddInt32();
    }
    else if (Operator == "-")
    {
        Op = BinaryenSubInt32();
    }
    else if (Operator == "*")
    {
        Op = BinaryenMulInt32();
    }
    else if (Operator == "/")
    {
        Op = BinaryenDivSInt32();
    }
    else
    {
        Safety::Throw("Tried to evalute an unknown numeric operator!!");
    }

    return BinaryenBinary(module, Op, left, right);
}

BinaryenExpressionRef CompileBinaryExpr(Shared<BinaryExpr> node, BinaryenModuleRef module)
{
    auto left = BinaryenLocalGet(module, 0, BinaryenTypeInt32());
    auto right = BinaryenLocalGet(module, 1, BinaryenTypeInt32());

    return CompileNumericBinaryExpr(left, right, node->Operator, module);
}

BinaryenExpressionRef CompileFunctionDeclaration(Shared<FunctionDeclaration> node, BinaryenModuleRef module)
{
    Vector<BinaryenType> ParamsTypes;
    ParamsTypes.reserve(node->Parameters.size());

    for (auto&& Param : node->Parameters)
    {
        ParamsTypes.emplace_back(BinaryenTypeInt32()); //@temporary
    }

    auto Params = BinaryenTypeCreate(ParamsTypes.data(), ParamsTypes.size());

    auto Return = BinaryenTypeInt32(); //@temporary

    BinaryenExpressionRef Body = BinaryenNop(module);

    for (auto&& Stmt : node->Body)
    {
        // printf("Stmt: %s\n", Stmt->ToString().c_str());

        Body = Compile(Stmt, module);
    }

    auto Func = BinaryenAddFunction(module, node->Name.c_str(), Params, Return, nullptr, 0, Body);

    BinaryenAddFunctionExport(module, node->Name.c_str(), node->Name.c_str());

    return nullptr;
}

BinaryenExpressionRef CompileVariableDeclaration(Shared<VariableDeclaration> node, BinaryenModuleRef module)
{
    auto Type = TypeNameToBinaryenType(node->TypeName);
    auto Value = GetDefaultValue(module, node->TypeName);

    if (node->Value)
    {
        if (node->Value.value()->Type == ASTNodeType::StringLiteral)
        {
            Value = BinaryenStringConst(module, node->Identifier.c_str());
            BinaryenStringConstSetString(Value, node->Value.value()->ToString().c_str());
        }
        else
        {
            Value = Compile(*node->Value, module);
        }
    }

    BinaryenAddGlobal(module, node->Identifier.c_str(), Type, true, Value);

    // BinaryenAddGlobalExport(module, node->Identifier.c_str(), node->Identifier.c_str());

    return nullptr;
}

export
{
    BinaryenExpressionRef Compile(Shared<Statement> node, BinaryenModuleRef module)
    {
        switch (node->Type)
        {
        case ASTNodeType::Program:
        {
            Timer t1("Program compiled");
            return CompileProgram(node->As<Program>(), module);
        }

        case ASTNodeType::IntLiteral:
            return BinaryenConst(module, BinaryenLiteralInt64(node->As<IntLiteral>()->Value));

        case ASTNodeType::FloatLiteral:
            return BinaryenConst(module, BinaryenLiteralFloat64(node->As<FloatLiteral>()->Value));

        case ASTNodeType::VariableDeclaration:
            return CompileVariableDeclaration(node->As<VariableDeclaration>(), module);

        case ASTNodeType::FunctionDeclaration:
            return CompileFunctionDeclaration(node->As<FunctionDeclaration>(), module);

        case ASTNodeType::BinaryExpr:
            return CompileBinaryExpr(node->As<BinaryExpr>(), module);

        default:
            Safety::Throw(std::format("Unimplemented compiler node {}", node->ToString()));
        }

        return nullptr;
    }
}