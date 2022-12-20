export module AST.Expressions;

import<format>;
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

        virtual auto ToString() -> String override
        {
            return std::format("{{\n\tType: '{}',\nAssigne: '{}',\nValue: '{}'\n}}", ASTNodeTypeToString(Type), Assigne->ToString(), Value->ToString());
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

        virtual auto ToString() -> String override
        {
            String ret = "Arguments: [ ";

            for (auto&& Arg : Arguments)
            {
                ret += Arg->ToString() + ", \n";
            }

            ret.erase(ret.end() - 2, ret.end());

            return std::format("{{\n\tType: '{}',\nCallee: '{}',\n{}\n}}", ASTNodeTypeToString(Type), Callee->ToString(), ret);
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

        virtual auto ToString() -> String override
        {
            return std::format("{{\n\tType: '{}',\nObject: '{}',\nProperty: '{}',\nComputed: '{}'\n}}", ASTNodeTypeToString(Type), Object->ToString(), Property->ToString(), Computed);
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
}