#pragma once
#include "Lexer.h"

enum class ASTNodeType
{
    Program,
    VariableDeclaration,

    AssignmentExpr,

    Property,
    ObjectLiteral,
    NumericLiteral,
    Identifier,
    BinaryExpr
};

constexpr const char* ASTNodeTypeToString(ASTNodeType ASTNodeType)
{
    switch (ASTNodeType)
    {
    case ASTNodeType::Program:
        return "Program";
    case ASTNodeType::VariableDeclaration:
        return "VariableDeclaration";
    case ASTNodeType::AssignmentExpr:
        return "AssignmentExpr";
    case ASTNodeType::Property:
        return "Property";
    case ASTNodeType::ObjectLiteral:
        return "ObjectLiteral";
    case ASTNodeType::NumericLiteral:
        return "NumericLiteral";
    case ASTNodeType::Identifier:
        return "Identifier";
    case ASTNodeType::BinaryExpr:
        return "BinaryExpr";
    default:
        return "Unimplemented in ASTNodeTypeToString";
    }
}

struct Statement : public std::enable_shared_from_this<Statement>
{
    ASTNodeType Type;

    Statement(ASTNodeType Type)
        : Type(Type)
    {
    }

    virtual auto ToString() -> String
    {
        return std::format("{{\n\tType: '{}'\n}}", ASTNodeTypeToString(Type));
    };

    template <typename T>
    auto Is() -> bool
    {
        //@temporary
        return String(typeid(T).name()).ends_with(ASTNodeTypeToString(Type));
    }

    template <typename T>
    auto As() -> Shared<T>
    {
        if (!Is<T>())
        {
            Safety::Throw(std::format("Cannot cast a statement of type {} to {}.", ASTNodeTypeToString(Type), typeid(T).name()));
        }

        return std::dynamic_pointer_cast<T>(shared_from_this());
    }
};

struct Expr : public Statement
{
    Expr(ASTNodeType Type)
        : Statement(Type)
    {
    }

    virtual auto ToString() -> String override
    {
        return std::format("{{\n\tType: '{}'\n}}", ASTNodeTypeToString(Type));
    }
};

struct Program : public Statement
{
    Vector<Shared<Statement>> Body;

    Program()
        : Statement { ASTNodeType::Program }
    {
    }

    virtual auto ToString() -> String override
    {
        String ret = "Body: [ ";

        for (auto&& Stmt : Body)
        {
            ret += Stmt->ToString() + ", \n";
        }

        ret.erase(ret.end() - 2, ret.end());
        ret += " \n\t]";

        return ret;
    }
};

struct VariableDeclaration : public Statement
{
    String Identifier;
    Optional<Shared<Expr>> Value;
    bool IsConst;

    VariableDeclaration(String identifier, Optional<Shared<Expr>> value, bool isConst)
        : Statement { ASTNodeType::VariableDeclaration }
        , Identifier(identifier)
        , Value(value)
        , IsConst(isConst)
    {
    }

    virtual auto ToString() -> String override
    {
        return std::format("{{\n\tType: '{}',\n\tIdentifier: '{}',\nValue: '{}',\n\tisConst: '{}'\n}}", ASTNodeTypeToString(Type), Identifier, Value.has_value() ? Value.value()->ToString() : "null", IsConst);
    }
};

struct AssignmentExpr : public Expr
{
    Shared<Expr> Assigne;
    Shared<Expr> Value;

    AssignmentExpr(Shared<Expr> assigne, Shared<Expr> value)
        : Expr { ASTNodeType::AssignmentExpr }
        , Assigne(assigne)
        , Value(value)
    {
    }

    virtual auto ToString() -> String override
    {
        return std::format("{{\n\tType: '{}',\nAssigne: '{}',\nValue: '{}'\n}}", ASTNodeTypeToString(Type), Assigne->ToString(), Value->ToString());
    }
};

struct BinaryExpr : public Expr
{
    Shared<Expr> Left;
    Shared<Expr> Right;
    String Operator;

    BinaryExpr(Shared<Expr> left, Shared<Expr> right, String op)
        : Expr { ASTNodeType::BinaryExpr }
        , Left(left)
        , Right(right)
        , Operator(op)
    {
    }

    virtual auto ToString() -> String override
    {
        return std::format("{{\n\tType: '{}',\nLeft: '{}',\nRight: '{}',\n\tOperator: '{}'\n}}", ASTNodeTypeToString(Type), Left->ToString(), Right->ToString(), Operator);
    }
};

struct Identifier : public Expr
{
    String Name;

    Identifier(String name)
        : Expr { ASTNodeType::Identifier }
        , Name { name }
    {
    }

    virtual auto ToString() -> String override
    {
        return std::format("{{\n\tType: '{}',\n\tName: '{}'\n}}", ASTNodeTypeToString(Type), Name);
    }
};

struct NumericLiteral : public Expr
{
    float Value;

    NumericLiteral(float value)
        : Expr { ASTNodeType::NumericLiteral }
        , Value(value)
    {
    }

    virtual auto ToString() -> String override
    {
        return std::format("{{\n\tType: '{}',\n\tValue: '{}'\n}}", ASTNodeTypeToString(Type), std::to_string(Value));
    }
};

struct Property : public Expr
{
    String Key;
    Optional<Shared<Expr>> Value;

    Property(String key, Optional<Shared<Expr>> value)
        : Expr { ASTNodeType::Property }
        , Key(key)
        , Value(value)
    {
    }

    virtual auto ToString() -> String override
    {
        return std::format("{{\n\tType: '{}',\n\tKey: '{}',\n\tValue: '{}'\n}}", ASTNodeTypeToString(Type), Key, Value.has_value() ? Value.value()->ToString() : "null");
    }
};

struct ObjectLiteral : public Expr
{
    Vector<Shared<Property>> Properties;

    ObjectLiteral(Vector<Shared<Property>> properties)
        : Expr { ASTNodeType::ObjectLiteral }
        , Properties(properties)
    {
    }

    virtual auto ToString() -> String override
    {
        String ret = "Properties: [ ";

        for (auto&& Prop : Properties)
        {
            ret += Prop->ToString() + ", \n";
        }

        ret.erase(ret.end() - 2, ret.end());
        ret += " \n\t]";

        return ret;
    }
};

