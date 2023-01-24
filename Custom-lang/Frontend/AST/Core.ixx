export module AST.Core;

import <format>;
import Types.Core;
import Safety;
import Logger;
import Reflection;

export
{
    enum class ASTNodeType
    {
        Unknown,
        Statement,
        Expression,
        DebugStatement,
        Program,
        VariableDeclaration,
        ExternDeclaration,
        FunctionDeclaration,
        ClassDeclaration,
        EnumDeclaration,
        ReturnStatement,
        SyncStatement,
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
        IntLiteral,
        FloatLiteral,
        StringLiteral,
        ArrayLiteral,
        Identifier,
        BinaryExpr,
        ConditionalExpr
    };

    struct Statement : public std::enable_shared_from_this<Statement>
    {
        ASTNodeType Type = ASTNodeType::Statement;

        Statement(ASTNodeType Type)
            : Type(Type)
        {
        }

        virtual String ToString(std::string indentation = "")
        {
            return std::format("{{\n{0}\tType: '{1}'\n{0}}}",
                indentation, Reflection::EnumToString(Type));
        }

        template <typename T>
        constexpr bool Is()
        {
            static_assert(std::is_base_of_v<Statement, T>, "T must be derived from Statement");

            return Reflection::TypeNameToString<T>() == Reflection::EnumToString(Type);
        }

        template <typename T>
        Shared<T> As()
        {
            if (!Is<T>())
            {
                Safety::Throw(std::format("Cannot cast a statement of type {} to {}.", Reflection::EnumToString(Type), Reflection::TypeNameToString<T>()));
            }

            return std::dynamic_pointer_cast<T>(shared_from_this());
        }
    };

    struct Expr : public Statement
    {
        bool bIsTopLevel = true;

        Expr()
            : Statement(ASTNodeType::Expression)
        {
        }

        Expr(ASTNodeType Type)
            : Statement(Type)
        {
        }

        virtual String ToString(std::string indentation = "")
        {
            return std::format("{{\n{0}\tType: '{1}'\n{0}}}",
                indentation, Reflection::EnumToString(Type));
        }
    };
}
