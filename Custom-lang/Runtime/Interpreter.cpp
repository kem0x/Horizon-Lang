#include "Interpreter.h"

auto Evaluate(StatementPtr node, EnviromentPtr env) -> RuntimeValuePtr
{
    switch (node->Type)
    {
    case NodeType::NumericLiteral:
    {
        auto Value = node->As<NumericLiteral>()->Value;
        return std::make_shared<NumberValue>(Value);
    }

    case NodeType::Identifier:
    {
        return EvalIdentifier(node->As<Identifier>(), env);
    }

    case NodeType::BinaryExpr:
    {
        return EvalBinaryExpr(node->As<BinaryExpr>(), env);
    }

    case NodeType::Program:
    {
        return EvalProgram(node->As<Program>(), env);
    }

    case NodeType::VariableDeclaration:
    {
        return EvalVariableDeclaration(node->As<VariableDeclaration>(), env);
    }

    case NodeType::AssignmentExpr:
    {
        return EvalAssignment(node->As<AssignmentExpr>(), env);
    }

    default:
    {
        return Throw<RuntimeValuePtr>(std::format("Unsupported AST Node {}.", node->ToString()));
    }
    }
}