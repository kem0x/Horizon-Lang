#include "Interpreter.h"

auto Evaluate(Shared<Statement> node, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    switch (node->Type)
    {
    case ASTNodeType::NumericLiteral:
    {
        auto Value = node->As<NumericLiteral>()->Value;
        return std::make_shared<NumberValue>(Value);
    }

    case ASTNodeType::Identifier:
    {
        return EvalIdentifier(node->As<Identifier>(), env);
    }

    case ASTNodeType::ObjectLiteral:
    {
        return EvalObjectExpr(node->As<ObjectLiteral>(), env);
    }

    case ASTNodeType::BinaryExpr:
    {
        return EvalBinaryExpr(node->As<BinaryExpr>(), env);
    }

    case ASTNodeType::Program:
    {
        return EvalProgram(node->As<Program>(), env);
    }

    case ASTNodeType::VariableDeclaration:
    {
        return EvalVariableDeclaration(node->As<VariableDeclaration>(), env);
    }

    case ASTNodeType::AssignmentExpr:
    {
        return EvalAssignment(node->As<AssignmentExpr>(), env);
    }

    default:
    {
        return Safety::Throw<Shared<RuntimeValue>>(std::format("Unsupported AST Node {}.", node->ToString()));
    }
    }
}