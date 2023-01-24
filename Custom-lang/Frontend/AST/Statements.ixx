export module AST.Statements;

import <format>;
import Types.Core;
import Safety;
import AST.Core;
import Reflection;

export
{
    struct DebugStatement : public Statement
    {
        DebugStatement()
            : Statement { ASTNodeType::DebugStatement }
        {
        }
    };

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

    struct ClassDeclaration : public Statement
    {
        String Name;
        Vector<Shared<Statement>> Body;

        ClassDeclaration(String name, Vector<Shared<Statement>> body)
            : Statement { ASTNodeType::ClassDeclaration }
            , Name(name)
            , Body(body)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tName: '{2}',\n{0}\tBody: [",
                indentation, Reflection::EnumToString(Type), Name);

            for (auto&& Stmt : Body)
            {
                output += "\n" + indentation + "\t\t" + Stmt->ToString(indentation + "\t\t") + ",";
            }

            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
        }
    };

    struct EnumDeclaration : public Statement
    {
        String Name;
        Vector<String> Values;

        EnumDeclaration(String name, Vector<String> values)
            : Statement { ASTNodeType::EnumDeclaration }
            , Name(name)
            , Values(values)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tName: '{2}',\n{0}\tValues: [",
                indentation, Reflection::EnumToString(Type), Name);

            for (auto&& Value : Values)
            {
                output += "\n" + indentation + "\t\t'" + Value + "',";
            }

            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
        }
    };

    struct VariableDeclaration : public Statement
    {
        String Identifier;
        String TypeName;
        Optional<Shared<Expr>> Value;
        bool IsConst;

        VariableDeclaration(String identifier, String typeName, Optional<Shared<Expr>> value, bool isConst)
            : Statement { ASTNodeType::VariableDeclaration }
            , Identifier(identifier)
            , TypeName(typeName)
            , Value(value)
            , IsConst(isConst)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            return std::format("{{\n{0}\tType: '{1}',\n{0}\tIdentifier: '{2}',\n{0}\tTypeName: '{3}',\n{0}\tValue: {4},\n{0}\tIsConst: {5}\n{0}}}",
                indentation, Reflection::EnumToString(Type), Identifier, TypeName, Value ? Value->get()->ToString(indentation + "\t") : "null", IsConst);
        }
    };

    // external functions
    struct ExternDeclaration : public Statement
    {
        String Identifier;
        String TypeName;
        Vector<Pair<String, String>> Parameters;

        ExternDeclaration(String identifier, String typeName, Vector<Pair<String, String>> parameters)
            : Statement { ASTNodeType::ExternDeclaration }
            , Identifier(identifier)
            , TypeName(typeName)
            , Parameters(parameters)
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tIdentifier: '{2}',\n{0}\tTypeName: '{3}',\n{0}\tParameters: [",
                indentation, Reflection::EnumToString(Type), Identifier, TypeName);

            for (auto&& Parameter : Parameters)
            {
                output += "\n" + indentation + "\t\t{ Identifier: '" + Parameter.first + "', TypeName: '" + Parameter.second + "' },";
            }

            output += "\n" + indentation + "\t]";

            output += "\n" + indentation + "}";

            return output;
        }
    };

    struct FunctionDeclaration : public Statement
    {
        String Name;
        Vector<Pair<String, String>> Parameters;
        String ReturnType;
        Vector<Shared<Statement>> Body;

        FunctionDeclaration(String name, Vector<Pair<String, String>> parameters, String returnType, Vector<Shared<Statement>> body)
            : Statement { ASTNodeType::FunctionDeclaration }
            , Name { name }
            , Parameters { parameters }
            , ReturnType(returnType)
            , Body { body }
        {
        }

        virtual String ToString(std::string indentation = "") override
        {
            std::string output = std::format("{{\n{0}\tType: '{1}',\n{0}\tName: '{2}',\n{0}\tParameters: [",
                indentation, Reflection::EnumToString(Type), Name);

            for (auto&& Param : Parameters)
            {
                output += "\n" + indentation + "\t\t{ Identifier: '" + Param.first + "', TypeName: '" + Param.second + "' },";
            }

            output += "\n" + indentation + "\t],\n{0}\tReturnType: '{3}',\n{0}\tBody: [",
                indentation, Reflection::EnumToString(Type), Name, ReturnType;

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
