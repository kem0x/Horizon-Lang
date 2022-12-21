export module AST.Expressions;

import <format>;
import Types.Core;
import AST.Core;

export
{
    struct AssignmentExpr : public Expr
    {
        Shared<Expr> Assigne;
        Shared<Expr> Value;

        AssignmentExpr(Shared<Expr> assigne, Shared<Expr> value)
            : Expr { ASTNodeType::AssignmentExpr }
            , Assigne(std::move(assigne))
            , Value(std::move(value))
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tAssigne: '{2}',\n{0}\tValue: '{3}'\n{0}}}",
                indentation, ASTNodeTypeToString(Type),
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
            , Callee(std::move(callee))
            , Arguments(arguments)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tCallee: '{2}',\n",
                indentation, ASTNodeTypeToString(Type), Callee->ToString(indentation + "\t"));

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
            , Object(std::move(object))
            , Property(std::move(property))
            , Computed(computed)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tObject: '{2}',\n{0}\tProperty: '{3}',\n{0}\tComputed: '{4}'\n{0}}}",
                indentation, ASTNodeTypeToString(Type),
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
                indentation, ASTNodeTypeToString(Type),
                Left->ToString(indentation + "\t"),
                Right->ToString(indentation + "\t"),
                Operator);

            return output;
        }
    };

    struct BlockExpr : Expr
    {
        Vector<Shared<Statement>> Statements;

        BlockExpr(Vector<Shared<Statement>> statements)
            : Expr { ASTNodeType::BlockExpr }
            , Statements(statements)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tStatements: [",
                indentation, ASTNodeTypeToString(Type));

            for (auto&& Stmt : Statements)
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
        Shared<BlockExpr> Then;
        Optional<Shared<BlockExpr>> Else;

        IfExpr(Shared<Expr> condition, Shared<BlockExpr> then, Optional<Shared<BlockExpr>> else_)
            : Expr { ASTNodeType::IfExpr }
            , Condition(std::move(condition))
            , Then(then)
            , Else(else_)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tCondition: '{2}',\n{0}\tThenBranch: '{3}'",
                indentation, ASTNodeTypeToString(Type),
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
                indentation, ASTNodeTypeToString(Type), Name);
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

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tValue: '{2}'\n{0}}}",
                indentation, ASTNodeTypeToString(Type), Value);
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
                indentation, ASTNodeTypeToString(Type), Value);
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
                indentation, ASTNodeTypeToString(Type),
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
                indentation, ASTNodeTypeToString(Type));

            for (auto&& Prop : Properties)
            {
                output += "\n" + indentation + "\t\t" + Prop->ToString(indentation + "\t\t") + ",";
            }

            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
        }
    };
}
