export module AST.Statements;

import<format>;
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
}