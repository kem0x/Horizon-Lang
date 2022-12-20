export module AST.Core;

import<format>;
import Types.Core;
import Safety;

export
{
    enum class ASTNodeType
    {
        Program,
        VariableDeclaration,

        AssignmentExpr,
        MemberExpr,
        CallExpr,

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
        case ASTNodeType::MemberExpr:
            return "MemberExpr";
        case ASTNodeType::CallExpr:
            return "CallExpr";

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

        auto ToString() -> String override
        {
            return std::format("{{\n\tType: '{}'\n}}", ASTNodeTypeToString(Type));
        }
    };
}