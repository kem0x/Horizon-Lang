export module Compiler;

import LLVM;

import Safety;
import Types.Core;
import AST.Core;
import AST.Statements;
import AST.Expressions;
import Reflection;
import <format>;
import <memory>;

import "KaleidoscopeJIT.h";

export Unique<llvm::LLVMContext> TheContext;
export Unique<llvm::Module> TheModule;
export Unique<llvm::IRBuilder<>> Builder;
export Unique<llvm::legacy::FunctionPassManager> TheFPM;

export Unique<llvm::orc::KaleidoscopeJIT> TheJIT;

UnorderedMap<String, llvm::Value*> NamedValues;

UnorderedMap<String, Shared<ExternDeclaration>> Externs;

llvm::Type* TypeNameToLLVMType(String typeName)
{
    if (typeName == "Int")
    {
        return llvm::Type::getInt64Ty(*TheContext);
    }
    else if (typeName == "Float")
    {
        return llvm::Type::getDoubleTy(*TheContext);
    }
    else if (typeName == "Bool")
    {
        return llvm::Type::getInt1Ty(*TheContext);
    }
    else if (typeName == "String")
    {
        return llvm::Type::getInt8PtrTy(*TheContext);
    }
    else
    {
        Safety::Throw(std::format("Unknown type: '{}'", typeName));
    }

    return llvm::Type::getInt8PtrTy(*TheContext);
}

llvm::Value* Compile(Shared<Statement> node);

llvm::Value* CompileProgram(Shared<Program> node)
{
    llvm::Value* LastValue = nullptr;

    for (auto&& Stmt : node->Body)
    {
        LastValue = Compile(Stmt);
    }

    return LastValue;
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
    auto Value = NamedValues[node->Name];

    if (!Value)
    {
        Safety::Throw(std::format("Unknown variable name '{0}'", node->Name));
    }

    return Value;
}

llvm::Value* CompileBinaryExpr(Shared<BinaryExpr> node)
{
    auto Left = Compile(node->Left);
    auto Right = Compile(node->Right);

    if (!Left || !Right)
    {
        Safety::Throw("Invalid binary expression");
    }

    switch (node->Operator)
    {
    case '+':
        return Builder->CreateAdd(Left, Right, "addtmp");
    case '-':
        return Builder->CreateSub(Left, Right, "subtmp");
    case '*':
        return Builder->CreateMul(Left, Right, "multmp");
    case '/':
        return Builder->CreateUDiv(Left, Right, "divtmp");
    default:
        Safety::Throw(std::format("Invalid binary operator: '{0}'", node->Operator));
    }

    return nullptr;
}

llvm::Value* CompileFunctionDeclaration(Shared<FunctionDeclaration> node)
{
    Vector<llvm::Type*> ArgsTypes;

    for (auto&& Arg : node->Parameters)
    {
        ArgsTypes.push_back(TypeNameToLLVMType(Arg.first));
    }

    auto FunctionType = llvm::FunctionType::get(TypeNameToLLVMType(node->ReturnType), ArgsTypes, false);

    auto Function = llvm::Function::Create(FunctionType, llvm::Function::ExternalLinkage, node->Name, *TheModule);

    auto BB = llvm::BasicBlock::Create(*TheContext, "entry", Function);

    Builder->SetInsertPoint(BB);

    for (auto i = 0; i < node->Parameters.size(); i++)
    {
        auto Param = node->Parameters[i];

        auto Arg = Function->arg_begin() + i;

        Arg->setName(Param.second);

        NamedValues[Param.second] = Arg;
    }

    llvm::Value* ReturnValue = nullptr;

    for (auto&& Stmt : node->Body)
    {
        ReturnValue = Compile(Stmt);
    }

    if (node->ReturnType == "Void" || !ReturnValue)
    {
        Builder->CreateRetVoid();
    }
    else
    {
        Builder->CreateRet(ReturnValue);
    }

    llvm::verifyFunction(*Function);

    TheFPM->run(*Function);

    return Function;
}

llvm::Value* CompileCallExpr(Shared<CallExpr> node)
{
    auto Name = node->Callee->As<Identifier>()->Name;

    if (auto CalleeF = TheModule->getFunction(Name))
    {
        if (CalleeF->arg_size() != node->Arguments.size())
        {
            Safety::Throw(std::format("Incorrect number of arguments passed to '{0}'", Name));
        }

        Vector<llvm::Value*> ArgsV;

        for (auto&& Arg : node->Arguments)
        {
            ArgsV.push_back(Compile(Arg));
        }

        return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
    }
}

llvm::Value* CompileExternDeclaration(Shared<ExternDeclaration> node)
{
    Externs[node->Identifier] = node;

    return nullptr;
}

export llvm::Value* Compile(Shared<Statement> node)
{
    // auto expr = std::dynamic_pointer_cast<Expression>(node);

    // if (expr)
    {
        // if (expr->bIsTopLevel)
        {
            auto RT = TheJIT->getMainJITDylib().createResourceTracker();

            auto TSM = llvm::orc::ThreadSafeModule(std::move(TheModule), std::move(TheContext));
            TheJIT->addModule(std::move(TSM), RT);

            // Search the JIT for the __anon_expr symbol.
            auto ExprSymbol = TheJIT->lookup("__anon_expr");

            // Get the symbol's address and cast it to the right type (takes no
            // arguments, returns a double) so we can call it as a native function.
            double (*FP)() = (double (*)())(intptr_t)ExprSymbol->getAddress();
            fprintf(stderr, "Evaluated to %f\n", FP());

            // Delete the anonymous expression module from the JIT.
            RT->remove();
        }
    }

    switch (node->Type)
    {
    case ASTNodeType::Program:
        return CompileProgram(node->As<Program>());

    case ASTNodeType::Identifier:
        return CompileIdentifier(node->As<Identifier>());

    case ASTNodeType::IntLiteral:
        return CompileIntLiteral(node->As<IntLiteral>());

    case ASTNodeType::FloatLiteral:
        return CompileFloatLiteral(node->As<FloatLiteral>());

    case ASTNodeType::StringLiteral:
        return CompileStringLiteral(node->As<StringLiteral>());

    case ASTNodeType::BinaryExpr:
        return CompileBinaryExpr(node->As<BinaryExpr>());

    case ASTNodeType::FunctionDeclaration:
        return CompileFunctionDeclaration(node->As<FunctionDeclaration>());

    case ASTNodeType::CallExpr:
        return CompileCallExpr(node->As<CallExpr>());

    default:
        return Safety::Throw<llvm::Value*>(std::format("Unknown AST node type: {}", Reflection::EnumToString(node->Type)));
    }

    return nullptr;
}