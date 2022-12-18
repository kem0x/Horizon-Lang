#pragma once
#include <string>
#include <memory>
#include <vector>
#include <format>

#include "Lexer.h"
#include <optional>

enum class NodeType
{
    Program,
    VariableDeclaration,

    AssignmentExpr,
    NumericLiteral,
    Identifier,
    BinaryExpr
};

constexpr const char* NodeTypeToString(NodeType NodeType)
{
    switch (NodeType)
    {
    case NodeType::Program:
        return "Program";
    case NodeType::VariableDeclaration:
        return "VariableDeclaration";
    case NodeType::AssignmentExpr:
        return "AssignmentExpr";
    case NodeType::NumericLiteral:
        return "NumericLiteral";
    case NodeType::Identifier:
        return "Identifier";
    case NodeType::BinaryExpr:
        return "BinaryExpr";
    default:
        return "Unimplemented in NodeTypeToString";
    }
}

struct Statement : public std::enable_shared_from_this<Statement>
{
    NodeType Type;

    Statement(NodeType Type)
        : Type(Type)
    {
    }

    virtual auto ToString() -> std::string
    {
        return std::format("{{ Type: {} }}", NodeTypeToString(Type));
    };

    template <typename T>
    auto Is() -> bool
    {
        //@temporary
        return std::string(typeid(T).name()).ends_with(NodeTypeToString(Type));
    }

    template <typename T>
    auto As() -> std::shared_ptr<T>
    {
        if (!Is<T>())
        {
            Throw<std::shared_ptr<T>>(std::format("Cannot cast a statement of type {} to {}.", NodeTypeToString(Type), typeid(T).name()));
        }

        return std::dynamic_pointer_cast<T>(shared_from_this());
    }
};
typedef std::shared_ptr<Statement> StatementPtr;

struct Expr : public Statement
{
    Expr(NodeType Type)
        : Statement(Type)
    {
    }

    virtual auto ToString() -> std::string override
    {
        return std::format("{{ Type: {} }}", NodeTypeToString(Type));
    }
};
typedef std::shared_ptr<Expr> ExprPtr;

struct Program : public Statement
{
    std::vector<StatementPtr> Body;

    Program()
        : Statement { NodeType::Program }
    {
    }

    virtual auto ToString() -> std::string override
    {
        std::string ret = "Body: [ ";

        for (auto&& Stmt : Body)
        {
            ret += Stmt->ToString() + ", ";
        }

        ret.erase(ret.end() - 2, ret.end());
        ret += " ]";

        return ret;
    }
};
typedef std::shared_ptr<Program> ProgramPtr;

struct VariableDeclaration : public Statement
{
    std::string Identifier;
    std::optional<ExprPtr> Value;
    bool IsConst;

    VariableDeclaration(std::string identifier, std::optional<ExprPtr> value, bool isConst)
        : Statement { NodeType::VariableDeclaration }
        , Identifier(identifier)
        , Value(value)
        , IsConst(isConst)
    {
    }

    virtual auto ToString() -> std::string override
    {
        return std::format("{{ Type: {}, Identifier: {}, Value: {}, isConst: {} }}", NodeTypeToString(Type), Identifier, Value.has_value() ? Value.value()->ToString() : "null", IsConst);
    }
};
typedef std::shared_ptr<VariableDeclaration> VariableDeclarationPtr;

struct AssignmentExpr : public Expr
{
    ExprPtr Assigne;
    ExprPtr Value;

    AssignmentExpr(ExprPtr assigne, ExprPtr value)
        : Expr { NodeType::AssignmentExpr }
        , Assigne(assigne)
        , Value(value)
    {
    }

    virtual auto ToString() -> std::string override
    {
        return std::format("{{ Type: {}, Assigne: {}, Value: {} }}", NodeTypeToString(Type), Assigne->ToString(), Value->ToString());
    }
};
typedef std::shared_ptr<AssignmentExpr> AssignmentExprPtr;

struct BinaryExpr : public Expr
{
    ExprPtr Left;
    ExprPtr Right;
    std::string Operator;

    BinaryExpr(ExprPtr left, ExprPtr right, std::string op)
        : Expr { NodeType::BinaryExpr }
        , Left(left)
        , Right(right)
        , Operator(op)
    {
    }

    virtual auto ToString() -> std::string override
    {
        return std::format("{{ Type: {}, Left: {}, Right: {}, Operator: {} }}", NodeTypeToString(Type), Left->ToString(), Right->ToString(), Operator);
    }
};
typedef std::shared_ptr<BinaryExpr> BinaryExprPtr;

struct Identifier : public Expr
{
    std::string Name;

    Identifier(std::string name)
        : Expr { NodeType::Identifier }
        , Name { name }
    {
    }

    virtual auto ToString() -> std::string override
    {
        return std::format("{{ Type: {}, Name: {} }}", NodeTypeToString(Type), Name);
    }
};
typedef std::shared_ptr<Identifier> IdentifierPtr;

struct NumericLiteral : public Expr
{
    float Value;

    NumericLiteral(float value)
        : Expr { NodeType::NumericLiteral }
        , Value(value)
    {
    }

    virtual auto ToString() -> std::string override
    {
        return std::format("{{ Type: {}, Value: {} }}", NodeTypeToString(Type), std::to_string(Value));
    }
};
typedef std::shared_ptr<NumericLiteral> NumericLiteralPtr;
