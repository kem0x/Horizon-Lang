export module AST.Core;

import <format>;
import Types.Core;
import Safety;
import Logger;
import Extensions.String;

export
{
    enum class ASTNodeType
    {
        Program,
        VariableDeclaration,
        FunctionDeclaration,
        ReturnStatement,
        PrintStatement,
        LoopStatement,
        BreakStatement,
        ContinueStatement,

        BlockExpr,
        IfExpr,
        AssignmentExpr,
        MemberExpr,
        CallExpr,

        Property,
        ObjectLiteral,
        NumericLiteral,
        StringLiteral,
        Identifier,
        BinaryExpr,
        ConditionalExpr
    };

    constexpr const char* ASTNodeTypeToString(ASTNodeType ASTNodeType)
    {
        switch (ASTNodeType)
        {
        case ASTNodeType::Program:
            return "Program";
        case ASTNodeType::VariableDeclaration:
            return "VariableDeclaration";
        case ASTNodeType::FunctionDeclaration:
            return "FunctionDeclaration";
        case ASTNodeType::ReturnStatement:
            return "ReturnStatement";
        case ASTNodeType::PrintStatement:
            return "PrintStatement";
        case ASTNodeType::LoopStatement:
            return "LoopStatement";
        case ASTNodeType::BreakStatement:
            return "BreakStatement";
        case ASTNodeType::ContinueStatement:
            return "ContinueStatement";

        case ASTNodeType::BlockExpr:
            return "BlockExpr";
        case ASTNodeType::IfExpr:
            return "IfExpr";
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
        case ASTNodeType::StringLiteral:
            return "StringLiteral";
        case ASTNodeType::Identifier:
            return "Identifier";
        case ASTNodeType::BinaryExpr:
            return "BinaryExpr";
        case ASTNodeType::ConditionalExpr:
            return "ConditionalExpr";

        default:
            Log<Error>("Unimplemented AST Node Type. (%i)", ASTNodeType);
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

        virtual String ToString(std::string indentation = "")
        {
            return std::format("{{\n{0}\tType: '{1}'\n{0}}}",
                indentation, ASTNodeTypeToString(Type));
        }

        template <typename T>
        constexpr bool Is()
        {
            static_assert(std::is_base_of_v<Statement, T>, "T must be derived from Statement");

            return StringExtensions::TypeName<T>() == ASTNodeTypeToString(Type);
        }

        template <typename T>
        Shared<T> As()
        {
            if (!Is<T>())
            {
                Safety::Throw(std::format("Cannot cast a statement of type {} to {}.", ASTNodeTypeToString(Type), StringExtensions::TypeName<T>()));
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

        virtual String ToString(std::string indentation = "")
        {
            return std::format("{{\n{0}\tType: '{1}'\n{0}}}",
                indentation, ASTNodeTypeToString(Type));
        }
    };
}
