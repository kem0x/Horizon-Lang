export module Compiler;

import LLVM;

import Safety;
import Types.Core;
import AST.Core;
import AST.Statements;
import AST.Expressions;
import Reflection;
import <format>;

static Shared<llvm::LLVMContext> TheContext = std::make_unique<llvm::LLVMContext>();
static Shared<llvm::Module> TheModule = std::make_unique<llvm::Module>("my cool jit", *TheContext);
static Shared<llvm::IRBuilder<>> Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
static UnorderedMap<String, llvm::Value*> NamedValues;

llvm::Value* Compile(Shared<Statement> node);

llvm::Value* CompileProgram(Shared<Program> node)
{
    llvm::Value* lastValue = nullptr;

    for (auto&& Stmt : node->Body)
    {
        lastValue = Compile(Stmt);
    }

    return lastValue;
}

llvm::Value* CompileIntLiteral(Shared<IntLiteral> node)
{
    return llvm::ConstantInt::get(*TheContext, llvm::APInt(32, node->Value, true));
}

llvm::Value* CompileFloatLiteral(Shared<FloatLiteral> node)
{
    return llvm::ConstantFP::get(*TheContext, llvm::APFloat(node->Value));
}

llvm::Value* CompileStringLiteral(Shared<StringLiteral> node)
{
    return Builder->CreateGlobalStringPtr(node->Value);
}

llvm::Value* CompileIdentifier(Shared<Identifier> node)
{
    auto value = NamedValues[node->Name];

    if (!value)
    {
        Safety::Throw(std::format("Unknown variable name '{0}'", node->Name));
    }

    return value;
}

llvm::Value* CompileBinaryExpr(Shared<BinaryExpr> node)
{
    auto Left = Compile(node->Left);
    auto Right = Compile(node->Right);

    if (!Left || !Right)
    {
        Safety::Throw("Invalid binary expression");
    }

    if (node->Operator == "+")
    {
        return Builder->CreateFAdd(Left, Right, "addtmp");
    }
    else if (node->Operator == "-")
    {
        return Builder->CreateFSub(Left, Right, "subtmp");
    }
    else if (node->Operator == "*")
    {
        return Builder->CreateFMul(Left, Right, "multmp");
    }
    else if (node->Operator == "/")
    {
        return Builder->CreateFDiv(Left, Right, "divtmp");
    }

    return nullptr;
}

export llvm::Value* Compile(Shared<Statement> node)
{
    switch (node->Type)
    {
    case ASTNodeType::Program:
        return CompileProgram(node->As<Program>());

    case ASTNodeType::IntLiteral:
        return CompileIntLiteral(node->As<IntLiteral>());

    case ASTNodeType::FloatLiteral:
        return CompileFloatLiteral(node->As<FloatLiteral>());

    case ASTNodeType::StringLiteral:
        return CompileStringLiteral(node->As<StringLiteral>());

    case ASTNodeType::BinaryExpr:
        return CompileBinaryExpr(node->As<BinaryExpr>());

    default:
        return Safety::Throw<llvm::Value*>(std::format("Unknown AST node type: {}", Reflection::EnumToString(node->Type)));
    }

    return nullptr;
}