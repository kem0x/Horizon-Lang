export module AST.Statements;

import <format>;
import Types.Core;
import Safety;
import AST.Core;
import Reflection;

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

    struct LoopStatement : public Statement
    {
        int32_t LoopCount;
        Vector<Shared<Statement>> Body;

        LoopStatement(int32_t loopCount, Vector<Shared<Statement>> body)
            : Statement { ASTNodeType::LoopStatement }
            , LoopCount(loopCount)
            , Body(body)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tLoopCount: '{2}',\n{0}\tBody: [",
                indentation, Reflection::EnumToString(Type), LoopCount);

            for (auto&& Stmt : Body)
            {
                output += "\n" + indentation + "\t\t" + Stmt->ToString(indentation + "\t\t") + ",";
            }

            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
        }
    };

    struct BreakStatement : public Statement
    {
        BreakStatement()
            : Statement { ASTNodeType::BreakStatement }
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}'\n{0}}}",
                indentation, Reflection::EnumToString(Type));
        }
    };

    struct ContinueStatement : public Statement
    {
        ContinueStatement()
            : Statement { ASTNodeType::ContinueStatement }
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}'\n{0}}}",
                indentation, Reflection::EnumToString(Type));
        }
    };

    struct SyncStatement : public Statement
    {
        Shared<Statement> Stmt;

        SyncStatement(Shared<Statement> statement)
            : Statement { ASTNodeType::SyncStatement }
            , Stmt(statement)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tStmt: {2}\n{0}}}",
                indentation, Reflection::EnumToString(Type), Stmt->ToString(indentation + "\t"));
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
                indentation, Reflection::EnumToString(Type), Identifier,
                Value.has_value() ? Value.value()->ToString(indentation + "\t") : "null", IsConst);
        }
    };

    struct FunctionDeclaration : public Statement
    {
        Optional<String> Name;
        Vector<Shared<Expr>> Parameters;
        Vector<Shared<Statement>> Body;

        FunctionDeclaration(Optional<String> name, Vector<Shared<Expr>> parameters, Vector<Shared<Statement>> body)
            : Statement { ASTNodeType::FunctionDeclaration }
            , Name { name }
            , Parameters { parameters }
            , Body { body }
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tName: '{2}',\n{0}\tParameters: [",
                indentation, Reflection::EnumToString(Type), Name.has_value() ? Name.value() : "null");

            for (auto&& Param : Parameters)
            {
                output += "\n" + indentation + "\t\t" + Param->ToString(indentation + "\t\t") + ",";
            }

            output += "\n" + indentation + "\t],\n" + indentation + "\tBody: [";

            for (auto&& Stmt : Body)
            {
                output += "\n" + indentation + "\t\t" + Stmt->ToString(indentation + "\t\t") + ",";
            }

            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
        }
    };

    struct ReturnStatement : public Statement
    {
        Optional<Shared<Expr>> Value;

        ReturnStatement(Optional<Shared<Expr>> value)
            : Statement { ASTNodeType::ReturnStatement }
            , Value(value)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tValue: '{2}'\n{0}}}",
                indentation, Reflection::EnumToString(Type),
                Value.has_value() ? Value.value()->ToString(indentation + "\t") : "null");
        }
    };
}
