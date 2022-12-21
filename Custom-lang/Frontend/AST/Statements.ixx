export module AST.Statements;

import <format>;
import Types.Core;
import Safety;
import AST.Core;

export
{
    struct Program : public Statement
    {
        Vector<Shared<Statement>> Body;

        Program()
            : Statement { ASTNodeType::Program }
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tBody: [",
                indentation, ASTNodeTypeToString(Type));

            for (auto&& Stmt : Body)
            {
                output += "\n" + indentation + "\t\t" + Stmt->ToString(indentation + "\t\t") + ",";
            }

            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
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

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tIdentifier: '{2}',\n{0}\tValue: '{3}',\n{0}\tisConst: '{4}'\n{0}}}",
                indentation, ASTNodeTypeToString(Type), Identifier,
                Value.has_value() ? Value.value()->ToString(indentation + "\t") : "null", IsConst);
        }
    };
}
