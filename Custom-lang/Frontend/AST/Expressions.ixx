export module AST.Expressions;

import <format>;
import Lexer;
import Types.Core;
import AST.Core;
import Reflection;

export
{
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

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tAssigne: '{2}',\n{0}\tValue: '{3}'\n{0}}}",
                indentation, Reflection::EnumToString(Type),
                Assigne->ToString(indentation + "\t"),
                Value->ToString(indentation + "\t"));

            return output;
        }
    };

    struct CallExpr : public Expr
    {
        Shared<Expr> Callee;
        Vector<Shared<Expr>> Arguments;

        CallExpr(Shared<Expr> callee, Vector<Shared<Expr>> arguments)
            : Expr { ASTNodeType::CallExpr }
            , Callee(callee)
            , Arguments(arguments)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tCallee: '{2}',\n",
                indentation, Reflection::EnumToString(Type), Callee->ToString(indentation + "\t"));

            output += indentation + "\tArguments: [";
            for (auto&& Arg : Arguments)
            {
                output += "\n" + indentation + "\t\t" + Arg->ToString(indentation + "\t\t") + ",";
            }
            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
        }
    };

    struct MemberExpr : public Expr
    {
        Shared<Expr> Object;
        Shared<Expr> Property;
        bool Computed;

        MemberExpr(Shared<Expr> object, Shared<Expr> property, bool computed)
            : Expr { ASTNodeType::MemberExpr }
            , Object(object)
            , Property(property)
            , Computed(computed)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tObject: '{2}',\n{0}\tProperty: '{3}',\n{0}\tComputed: '{4}'\n{0}}}",
                indentation, Reflection::EnumToString(Type),
                Object->ToString(indentation + "\t"),
                Property->ToString(indentation + "\t"),
                Computed);
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

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tLeft: '{2}',\n{0}\tRight: '{3}',\n{0}\tOperator: '{4}'\n{0}}}",
                indentation, Reflection::EnumToString(Type),
                Left->ToString(indentation + "\t"),
                Right->ToString(indentation + "\t"),
                Operator);

            return output;
        }
    };

    struct BlockExpr : Expr
    {
        Vector<Shared<Statement>> Body;

        BlockExpr(Vector<Shared<Statement>> body)
            : Expr { ASTNodeType::BlockExpr }
            , Body(body)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tBody: [",
                indentation, Reflection::EnumToString(Type));

            for (auto&& Stmt : Body)
            {
                output += "\n" + indentation + "\t\t" + Stmt->ToString(indentation + "\t\t") + ",";
            }

            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
        }
    };

    struct IfExpr : public Expr
    {
        Shared<Expr> Condition;
        Shared<Expr> Then;
        Optional<Shared<Expr>> Else;

        IfExpr(Shared<Expr> condition, Shared<Expr> then, Optional<Shared<Expr>> else_)
            : Expr { ASTNodeType::IfExpr }
            , Condition(condition)
            , Then(then)
            , Else(else_)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tCondition: '{2}',\n{0}\tThenBranch: '{3}'",
                indentation, Reflection::EnumToString(Type),
                Condition->ToString(indentation + "\t"),
                Then->ToString(indentation + "\t"));

            if (Else)
            {
                output += ",\n" + indentation + "\tElseBranch: '" + Else.value()->ToString(indentation + "\t") + "'";
            }

            output += "\n" + indentation + "}";

            return output;
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

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tName: '{2}'\n{0}}}",
                indentation, Reflection::EnumToString(Type), Name);
        }
    };

    struct NumericLiteral : public Expr
    {
        int Value;

        NumericLiteral(int value)
            : Expr { ASTNodeType::NumericLiteral }
            , Value(value)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tValue: '{2}'\n{0}}}",
                indentation, Reflection::EnumToString(Type), Value);
        }
    };

    struct StringLiteral : public Expr
    {
        String Value;

        StringLiteral(String value)
            : Expr { ASTNodeType::StringLiteral }
            , Value(value)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tValue: '{2}'\n{0}}}",
                indentation, Reflection::EnumToString(Type), Value);
        }
    };

    struct ArrayLiteral : public Expr
    {
        Vector<Shared<Expr>> Elements;

        ArrayLiteral(Vector<Shared<Expr>> elements)
            : Expr { ASTNodeType::ArrayLiteral }
            , Elements(elements)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tElements: [",
                indentation, Reflection::EnumToString(Type));

            for (auto&& Element : Elements)
            {
                output += "\n" + indentation + "\t\t" + Element->ToString(indentation + "\t\t") + ",";
            }

            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
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

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tKey: '{2}',\n{0}\tValue: '{3}'\n{0}}}",
                indentation, Reflection::EnumToString(Type),
                Key,
                Value.has_value() ? Value.value()->ToString(indentation + "\t") : "null");
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

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tProperties: [",
                indentation, Reflection::EnumToString(Type));

            for (auto&& Prop : Properties)
            {
                output += "\n" + indentation + "\t\t" + Prop->ToString(indentation + "\t\t") + ",";
            }

            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
        }
    };

    struct ConditionalExpr : public Expr
    {
        Shared<Expr> Left;
        Shared<Expr> Right;
        LexerTokenType Operator;

        ConditionalExpr(Shared<Expr> left, Shared<Expr> right, LexerTokenType op)
            : Expr { ASTNodeType::ConditionalExpr }
            , Left(left)
            , Right(right)
            , Operator(op)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tLeft: '{2}',\n{0}\tRight: '{3}',\n{0}\tOperator: '{4}'\n{0}}}",
                indentation, Reflection::EnumToString(Type),
                Left->ToString(indentation + "\t"),
                Right->ToString(indentation + "\t"),
                Reflection::EnumToString(Operator));

            return output;
        }
    };
}
