export module Interpreter;

export import :Stmts;
export import :Exprs;

import Types.Core;
import AST.Core;
import Runtime.ExecutionContext;
import Runtime.RuntimeValue;
import Runtime.IntValue;
import Runtime.FloatValue;

export Shared<RuntimeValue> Evaluate(Shared<Statement> node, Shared<ExecutionContext> ctx)
{
    switch (node->Type)
    {
    case ASTNodeType::IntLiteral:
        return std::make_shared<IntValue>(node->As<IntLiteral>()->Value);

    case ASTNodeType::FloatLiteral:
        return std::make_shared<FloatValue>(node->As<FloatLiteral>()->Value);

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

    case ASTNodeType::ArrayLiteral:
        return EvalArrayExpr(node->As<ArrayLiteral>(), ctx);

    case ASTNodeType::MemberExpr:
        return EvalMemberExpr(node->As<MemberExpr>(), ctx);

    case ASTNodeType::BinaryExpr:
        return EvalBinaryExpr(node->As<BinaryExpr>(), ctx);

    case ASTNodeType::Program:
    {
        Timer t1("Program evaluation");
        return EvalProgram(node->As<Program>(), ctx);
    }

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

    case ASTNodeType::ClassDeclaration:
        return EvalClassDeclaration(node->As<ClassDeclaration>(), ctx);

    case ASTNodeType::EnumDeclaration:
        return EvalEnumDeclaration(node->As<EnumDeclaration>(), ctx);

    case ASTNodeType::DebugStatement:
        return EvalDebugStatement(node->As<DebugStatement>(), ctx);

    default:
        return Safety::Throw<Shared<RuntimeValue>>(std::format("Unsupported AST Node {}.", node->ToString()));
    }
}
