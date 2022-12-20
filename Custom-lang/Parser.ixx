export module Parser;

import<format>;
import<optional>;
import Types;
import Safety;
import Logger;
import Lexer;
import AST;
import VectorExtensions;

export
{
    class Parser
    {
        Vector<Lexer::Token> Tokens;

        auto NotEoF() -> bool
        {
            return Tokens[0].Type != LexerTokenType::EoF;
        }

        auto Current() -> Lexer::Token
        {
            return Tokens[0];
        }

        auto Advance() -> Lexer::Token
        {
            return VectorExtensions::Shift(Tokens);
        }

        auto Expect(LexerTokenType type, String error) -> Lexer::Token
        {
            if (Current().Type != type)
            {
                Safety::Throw(error);
            }

            return Advance();
        }

        auto ParseAdditiveExpr() -> Shared<Expr>
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

        auto ParseMultiplicativeExpr() -> Shared<Expr>
        {
            auto Left = ParsePrimaryExpr();

            while (Current().Value == "/" || Current().Value == "*" || Current().Value == "%")
            {
                auto Operator = Advance().Value;

                auto Right = ParsePrimaryExpr();

                Left = std::make_shared<BinaryExpr>(Left, Right, Operator);
            }

            return Left;
        }

        auto ParsePrimaryExpr() -> Shared<Expr>
        {
            Log<Info>("Current(): %s", Tokens[0].ToString().c_str());

            switch (Current().Type)
            {
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

        auto ParseObjectExpr() -> Shared<Expr>
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
                else if (Current().Type == LexerTokenType::CloseBrace)
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

        auto ParseAssignmentExpr() -> Shared<Expr>
        {
            const auto left = ParseObjectExpr();

            if (Current().Type == LexerTokenType::Equals)
            {
                Advance(); // Skip '='

                const auto right = ParseAssignmentExpr();

                return std::make_shared<AssignmentExpr>(left, right);
            }

            return left;
        }

        auto ParseExpr() -> Shared<Expr>
        {
            return ParseAssignmentExpr();
        }

        auto ParseStatement() -> Shared<Statement>
        {
            switch (Current().Type)
            {
            case LexerTokenType::Let:
            case LexerTokenType::Const:
                return ParseVariableDeclartion();

            default:
                return ParseExpr();
            }
        }

        auto ParseVariableDeclartion() -> Shared<Statement>
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
        auto ProduceAST(String sourceCode) -> Shared<Program>
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
