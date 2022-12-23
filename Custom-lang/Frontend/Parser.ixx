export module Parser;

import <format>;
import <optional>;
import Types.Core;
import Safety;
import Logger;
import Lexer;
import AST.Core;
import AST.Expressions;
import AST.Statements;
import Extensions.Vector;

export
{
    class Parser
    {
        Vector<Lexer::Token> Tokens;

        bool NotEoF()
        {
            return Tokens[0].Type != LexerTokenType::EoF;
        }

        Lexer::Token Current()
        {
            return Tokens[0];
        }

        Lexer::Token Advance()
        {
            return VectorExtensions::Shift(Tokens);
        }

        Lexer::Token Expect(LexerTokenType type, String error)
        {
            if (Current().Type != type)
            {
                Safety::Throw(error);
            }

            return Advance();
        }

        Shared<Expr> ParseAdditiveExpr()
        {
            auto Left = ParseMultiplicativeExpr();

            while (Current().Value == "+" || Current().Value == "-")
            {
                auto Operator = Advance().Value;

                auto Right = ParseMultiplicativeExpr();

                Left = std::make_shared<BinaryExpr>(Left, Right, Operator);
            }

            return Left;
        }

        Shared<Expr> ParseCallMemberExpr()
        {
            const auto Member = ParseMemberExpr();

            if (Current().Type == LexerTokenType::OpenParen)
            {
                return ParseCallExpr(Member);
            }

            return Member;
        }

        Shared<CallExpr> ParseCallExpr(Shared<Expr> caller)
        {
            auto Call = std::make_shared<CallExpr>(caller, ParseArgs());

            if (Current().Type == LexerTokenType::OpenParen)
            {
                Call = ParseCallExpr(Call);
            }

            return Call;
        }

        Vector<Shared<Expr>> ParseArgs()
        {
            Expect(LexerTokenType::OpenParen, "Expected '('");

            auto Args = Current().Type == LexerTokenType::CloseParen ? Vector<Shared<Expr>>() : ParseArgsList();

            Expect(LexerTokenType::CloseParen, "Expected ')'");

            return Args;
        }

        Vector<Shared<Expr>> ParseArgsList()
        {
            auto Args = Vector<Shared<Expr>> { std::move(ParseAssignmentExpr()) };

            while (Current().Type == LexerTokenType::Comma)
            {
                Advance();

                Args.push_back(std::move(ParseAssignmentExpr()));
            }

            return Args;
        }

        Shared<Expr> ParseMemberExpr()
        {
            auto Object = ParsePrimaryExpr();

            while (Current().Type == LexerTokenType::Dot || Current().Type == LexerTokenType::OpenBracket)
            {
                const auto Operator = Advance();

                if (Operator.Type == LexerTokenType::Dot)
                {
                    auto Property = ParsePrimaryExpr();

                    if (Property->Type != ASTNodeType::Identifier)
                    {
                        Safety::Throw("Using a non-identifier as a property is not allowed with dot operator!");
                    }

                    Object = std::make_shared<MemberExpr>(Object, Property, false);
                }
                else
                {
                    Object = std::make_shared<MemberExpr>(Object, ParseExpr(), true);

                    Expect(LexerTokenType::CloseBracket, "Expected ']'");
                }
            }

            return Object;
        }

        Shared<Expr> ParseMultiplicativeExpr()
        {
            auto Left = ParseCallMemberExpr();

            while (Current().Value == "/" || Current().Value == "*" || Current().Value == "%")
            {
                auto Operator = Advance().Value;

                auto Right = ParseCallMemberExpr();

                Left = std::make_shared<BinaryExpr>(Left, Right, Operator);
            }

            return Left;
        }

        Shared<Expr> ParsePrimaryExpr()
        {
            switch (Current().Type)
            {
            case LexerTokenType::String:
                return std::make_shared<StringLiteral>(Advance().Value);

            case LexerTokenType::Identifier:
                return std::make_shared<Identifier>(Advance().Value);

            case LexerTokenType::Number:
                return std::make_shared<NumericLiteral>(std::stof(Advance().Value));

            case LexerTokenType::OpenParen:
            {
                Advance(); // Skip '('

                auto Value = ParseAdditiveExpr();

                Expect(LexerTokenType::CloseParen, "Unexpected token: " + Current().Value + ", expected a ')'");

                return Value;
            }

            default:
                return Safety::Throw<Shared<Expr>>(std::format("Unexpected token {} found during parsing!", Current().ToString()));
            }
        }

        Shared<Expr> ParseObjectExpr()
        {
            if (Current().Type != LexerTokenType::OpenBrace)
            {
                return ParseAdditiveExpr();
            }

            Advance(); // Skip '{'

            Vector<Shared<Property>> properties;

            while (NotEoF() && Current().Type != LexerTokenType::CloseBrace)
            {
                const auto key = Expect(LexerTokenType::Identifier, "Unexpected token: " + Current().Value + ", expected a property name").Value;

                // Allow shorthand key
                if (Current().Type == LexerTokenType::Comma)
                {
                    Advance(); // Skip ','

                    properties.push_back(std::make_shared<Property>(key, std::nullopt));
                    continue;
                }
                if (Current().Type == LexerTokenType::CloseBrace)
                {
                    properties.push_back(std::make_shared<Property>(key, std::nullopt));
                    continue;
                }

                Expect(LexerTokenType::Colon, "Unexpected token: " + Current().Value + ", expected a ':'");

                const auto value = ParseExpr();

                properties.push_back(std::make_shared<Property>(key, value));

                if (Current().Type != LexerTokenType::CloseBrace)
                {
                    Expect(LexerTokenType::Comma, "Unexpected token: " + Current().Value + ", expected a ','");
                }
            }

            Expect(LexerTokenType::CloseBrace, "Unexpected token: " + Current().Value + ", expected a ']'");

            return std::make_shared<ObjectLiteral>(properties);
        }

        Shared<Expr> ParseAssignmentExpr()
        {
            const auto left = ParseObjectExpr();

            if (Current().Type == LexerTokenType::Equals)
            {
                Advance(); // Skip '='

                const auto right = ParseAssignmentExpr();

                Expect(LexerTokenType::Semicolon, std::format("Expected ';' after variable assignment, got '{}'", Current().Value));

                return std::make_shared<AssignmentExpr>(left, right);
            }

            return left;
        }

        Shared<Expr> ParseExpr()
        {
            return ParseAssignmentExpr();
        }

        Shared<BlockExpr> ParseBlock()
        {
            Vector<Shared<Statement>> exprs;

            while (NotEoF() && Current().Type != LexerTokenType::CloseBrace)
            {
                exprs.push_back(std::move(ParseStatement()));
            }

            Expect(LexerTokenType::CloseBrace, "Expected a closing bracket");

            return std::make_shared<BlockExpr>(exprs);
        }

        Shared<Expr> ParseIfExpr()
        {
            Expect(LexerTokenType::If, "Unexpected token: " + Current().Value + ", expected a 'if'");

            Expect(LexerTokenType::OpenParen, "Unexpected token: " + Current().Value + ", expected a '('");

            const auto condition = ParseExpr();

            Expect(LexerTokenType::CloseParen, "Unexpected token: " + Current().Value + ", expected a ')'");
            Expect(LexerTokenType::OpenBrace, "Unexpected token: " + Current().Value + ", expected a '{'");

            auto body = ParseBlock();

            if (Current().Type == LexerTokenType::Else)
            {
                Advance(); // Skip

                if (Current().Type == LexerTokenType::If)
                {
                    return std::make_shared<IfExpr>(condition, body, ParseIfExpr());
                }

                Expect(LexerTokenType::OpenBrace, "Unexpected token: " + Current().Value + ", expected a '{'");

                auto elseBody = ParseBlock();

                return std::make_shared<IfExpr>(condition, body, elseBody);
            }

            return std::make_shared<IfExpr>(condition, body, std::nullopt);
        }

        Shared<Statement> ParsePrintStatement()
        {
            Advance();

            auto Value = ParseExpr();

            Expect(LexerTokenType::Semicolon, "Expected ';'");

            return std::make_shared<PrintStatement>(Value);
        }

        Shared<Statement> ParseStatement()
        {
            switch (Current().Type)
            {
            case LexerTokenType::Print:
                return ParsePrintStatement();

            case LexerTokenType::Let:
            case LexerTokenType::Const:
                return ParseVariableDeclartion();

            case LexerTokenType::If:
                return ParseIfExpr();

            case LexerTokenType::OpenBrace:
                Advance();
                return ParseBlock();

            default:
                return ParseExpr();
            }
        }

        Shared<Statement> ParseVariableDeclartion()
        {
            const auto isConstant = Advance().Type == LexerTokenType::Const;

            const auto identifier = Expect(LexerTokenType::Identifier, "Expected identifier name after (let | const) keyword.").Value;

            if (Current().Type == LexerTokenType::Semicolon)
            {
                Advance(); // Skip ';'

                if (isConstant)
                {
                    Safety::Throw(std::format("A const variable '{}' must be initialized with a value!", identifier.c_str()));
                }

                return std::make_shared<VariableDeclaration>(identifier, std::nullopt, isConstant);
            }

            Expect(LexerTokenType::Equals, std::format("Expected '=' after identifier name '{}'.", identifier.c_str()));

            const auto declaration = std::make_shared<VariableDeclaration>(identifier, ParseExpr(), isConstant);

            Expect(LexerTokenType::Semicolon, std::format("Expected ';' after variable declaration '{}'.", identifier.c_str()));

            return declaration;
        }

    public:
        Shared<Program> ProduceAST(String sourceCode)
        {
            Lexer lexer(sourceCode);
            Tokens = lexer.Tokenize();

            auto program = std::make_shared<Program>();

            while (NotEoF())
            {
                program->Body.push_back(ParseStatement());
            }

            return program;
        }
    };
}
