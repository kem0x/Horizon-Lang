export module Parser;

import <format>;
import <optional>;
import <string>;
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

        Shared<Expr> ParsePrimaryExpr()
        {
            switch (Current().Type)
            {
            case LexerTokenType::If:
                return ParseIfExpr();

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

        Shared<CallExpr> ParseCallExpr(Shared<Expr> caller)
        {
            auto Call = std::make_shared<CallExpr>(caller, ParseArgs());

            if (Current().Type == LexerTokenType::OpenParen)
            {
                Call = ParseCallExpr(Call);
            }
            else if (Current().Type == LexerTokenType::Semicolon)
            {
                Advance();
            }

            return Call;
        }

        Shared<Expr> ParseMemberExpr()
        {
            auto Object = ParsePrimaryExpr();

            while (Current().Type == LexerTokenType::Dot or Current().Type == LexerTokenType::OpenBracket)
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

            if (Current().Type == LexerTokenType::OpenParen)
            {
                Object = ParseCallExpr(Object);
            }

            return Object;
        }

        // ParseUnaryExpr

        // ParseRangeExpr

        // ParseSwitchExpr

        Shared<Expr> ParseMultiplicativeExpr()
        {
            auto Left = ParseMemberExpr();

            while (Current().Value == "/" or Current().Value == "*" or Current().Value == "%")
            {
                auto Operator = Advance().Value;

                auto Right = ParseMemberExpr();

                Left = std::make_shared<BinaryExpr>(Left, Right, Operator);
            }

            return Left;
        }

        Shared<Expr> ParseAdditiveExpr()
        {
            auto Left = ParseMultiplicativeExpr();

            while (Current().Value == "+" or Current().Value == "-")
            {
                auto Operator = Advance().Value;

                auto Right = ParseMultiplicativeExpr();

                Left = std::make_shared<BinaryExpr>(Left, Right, Operator);
            }

            return Left;
        }

        Shared<Expr> ParseComparisonExpr()
        {
            auto Left = ParseAdditiveExpr();

            while (Current().Type == LexerTokenType::Greater
                or Current().Type == LexerTokenType::GreaterEqual
                or Current().Type == LexerTokenType::Less
                or Current().Type == LexerTokenType::LessEqual)
            {
                auto Op = Advance();
                auto Right = ParseAdditiveExpr();

                Left = std::make_shared<ConditionalExpr>(Left, Right, Op.Type);
            }

            // handle 'is' keyword

            return Left;
        }

        Shared<Expr> ParseEqualityExpr()
        {
            auto Left = ParseComparisonExpr();

            while (NotEoF() and (Current().Type == LexerTokenType::BangEqual or Current().Type == LexerTokenType::EqualEqual))
            {
                auto Op = Advance();
                auto Right = ParseComparisonExpr();

                Left = std::make_shared<ConditionalExpr>(Left, Right, Op.Type);
            }

            return Left;
        }

        Shared<Expr> ParseConditionalAndExpr()
        {
            auto Left = ParseEqualityExpr();

            while (Current().Type == LexerTokenType::And)
            {
                auto Op = Advance();
                auto Right = ParseEqualityExpr();
                Left = std::make_shared<ConditionalExpr>(Left, Right, Op.Type);
            }

            return Left;
        }

        Shared<Expr> ParseConditionalOrExpr()
        {
            auto Left = ParseConditionalAndExpr();

            while (Current().Type == LexerTokenType::Or)
            {
                auto Op = Advance();
                auto Right = ParseConditionalAndExpr();
                Left = std::make_shared<ConditionalExpr>(Left, Right, Op.Type);
            }

            return Left;
        }

        // ParseNullCoalescingExpr

        Shared<Expr> ParseObjectExpr()
        {
            if (Current().Type != LexerTokenType::OpenBrace)
            {
                return ParseConditionalOrExpr();
            }

            Advance(); // Skip '{'

            Vector<Shared<Property>> properties;

            while (NotEoF() and Current().Type != LexerTokenType::CloseBrace)
            {
                const auto key = Expect(LexerTokenType::Identifier, "Unexpected token: " + Current().Value + ", expected a property name").Value;

                // Allow shorthand key
                if (Current().Type == LexerTokenType::Comma)
                {
                    Advance(); // Skip ','

                    properties.emplace_back(std::make_shared<Property>(key, std::nullopt));
                    continue;
                }
                if (Current().Type == LexerTokenType::CloseBrace)
                {
                    properties.emplace_back(std::make_shared<Property>(key, std::nullopt));
                    continue;
                }

                Expect(LexerTokenType::Colon, "Unexpected token: " + Current().Value + ", expected a ':'");

                const auto value = ParseExpr();

                properties.emplace_back(std::make_shared<Property>(key, value));

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

            if (Current().Type == LexerTokenType::Equal)
            {
                Advance(); // Skip '='

                const auto right = ParseObjectExpr();

                Expect(LexerTokenType::Semicolon, std::format("Expected ';' after variable assignment, got '{}'", Current().Value));

                return std::make_shared<AssignmentExpr>(left, right);
            }

            return left;
        }

        Shared<Expr> ParseExpr()
        {
            return ParseAssignmentExpr();
        }

        Vector<Shared<Expr>> ParseArgs()
        {
            const auto ParseArgsList = [&]()
            {
                Vector<Shared<Expr>> args;

                while (NotEoF() and Current().Type != LexerTokenType::CloseParen)
                {
                    args.emplace_back(ParseExpr());

                    if (Current().Type != LexerTokenType::CloseParen)
                    {
                        Expect(LexerTokenType::Comma, "Expected ','");
                    }
                }

                return args;
            };

            Expect(LexerTokenType::OpenParen, "Expected '('");

            auto Args = Current().Type == LexerTokenType::CloseParen ? Vector<Shared<Expr>>() : ParseArgsList();

            Expect(LexerTokenType::CloseParen, "Expected ')'");

            return Args;
        }

        Shared<BlockExpr> ParseBlock()
        {
            Vector<Shared<Statement>> Exprs;

            while (NotEoF() and Current().Type != LexerTokenType::CloseBrace)
            {
                Exprs.emplace_back(ParseStatement());
            }

            Expect(LexerTokenType::CloseBrace, "Expected a closing bracket");

            return std::make_shared<BlockExpr>(Exprs);
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

        Shared<Statement> ParseLoopStatement()
        {
            Advance(); // Skip 'loop'

            int32_t Times = 0;

            if (Current().Type == LexerTokenType::OpenParen)
            {
                Advance(); // Skip '('

                const auto LoopCountExpr = ParseExpr();

                if (LoopCountExpr->Is<NumericLiteral>())
                {
                    Times = LoopCountExpr->As<NumericLiteral>()->Value;

                    Expect(LexerTokenType::CloseParen, "Unexpected token: " + Current().Value + ", expected a ')'");
                }
                else
                {
                    Safety::Throw("Loop count must be a number!");
                }
            }

            Expect(LexerTokenType::OpenBrace, "Unexpected token: " + Current().Value + ", expected a '{'");

            auto Block = ParseBlock();

            return std::make_shared<LoopStatement>(Times, Block->Body);
        }

        Shared<Statement> ParseFunctionDeclaration()
        {
            if (Current().Type == LexerTokenType::Function)
                Advance(); // Skip 'function'

            const auto Name = Expect(LexerTokenType::Identifier, "Unexpected token: " + Current().Value + ", expected a function name").Value;

            const auto Parameters = ParseArgs();

            Expect(LexerTokenType::OpenBrace, "Unexpected token: " + Current().Value + ", expected a '{'");

            auto Block = ParseBlock();

            return std::make_shared<FunctionDeclaration>(Name, Parameters, Block->Body);
        }

        Shared<Statement> ParseReturnStatement()
        {
            Advance(); // Skip 'return'

            const auto Value = ParseExpr();

            Expect(LexerTokenType::Semicolon, "Unexpected token: " + Current().Value + ", expected a ';'");

            return std::make_shared<ReturnStatement>(Value);
        }

        Shared<Statement> ParseStatement()
        {
            switch (Current().Type)
            {
            case LexerTokenType::Loop:
                return ParseLoopStatement();

            case LexerTokenType::Break:
                Advance(); // Skip 'break'
                Expect(LexerTokenType::Semicolon, "Expected ';'");
                return std::make_shared<BreakStatement>();

            case LexerTokenType::Continue:
                Advance(); // Skip 'continue'
                Expect(LexerTokenType::Semicolon, "Expected ';'");
                return std::make_shared<ContinueStatement>();

            case LexerTokenType::Function:
                return ParseFunctionDeclaration();

            case LexerTokenType::Return:
                return ParseReturnStatement();

            case LexerTokenType::Let:
            case LexerTokenType::Const:
                return ParseVariableDeclaration();

            case LexerTokenType::Class:
                return ParseClassDeclaration();

            case LexerTokenType::Sync:
                return ParseSyncStatement();

            case LexerTokenType::Debug:
                return ParseDebugStatement();

            case LexerTokenType::OpenBrace:
                Advance();
                return ParseBlock();

            default:
                return ParseExpr();
            }
        }

        Shared<Statement> ParseSyncStatement()
        {
            Advance(); // Skip 'sync'

            auto Value = ParseStatement();

            Expect(LexerTokenType::Semicolon, "Expected ';'");

            return std::make_shared<SyncStatement>(Value);
        }

        Shared<Statement> ParseDebugStatement()
        {
            Advance(); // Skip 'debug'

            return std::make_shared<DebugStatement>();
        }

        Shared<Statement> ParseVariableDeclaration()
        {
            const auto IsConstant = Advance().Type == LexerTokenType::Const;

            const auto Identifier = Expect(LexerTokenType::Identifier, "Expected identifier name after (let | const) keyword.").Value;

            String TypeName = "Any";

            if (Current().Type == LexerTokenType::Colon)
            {
                Advance(); // Skip ':'

                TypeName = Expect(LexerTokenType::Identifier, "Expected type name after ':'").Value;
            }

            if (Current().Type == LexerTokenType::Semicolon)
            {
                Advance(); // Skip ';'

                if (IsConstant)
                {
                    Safety::Throw(std::format("A const variable '{}' must be initialized with a value!", Identifier.c_str()));
                }

                return std::make_shared<VariableDeclaration>(Identifier, TypeName, std::nullopt, IsConstant);
            }

            Expect(LexerTokenType::Equal, std::format("Expected '=' after identifier name '{}'.", Identifier.c_str()));

            const auto declaration = std::make_shared<VariableDeclaration>(Identifier, TypeName, ParseExpr(), IsConstant);

            if (Current().Type == LexerTokenType::Semicolon)
            {
                Advance();
            }

            return declaration;
        }

        Shared<Statement> ParseClassDeclaration()
        {
            Advance(); // Skip 'class'

            const auto Name = Expect(LexerTokenType::Identifier, "Expected class name after 'class' keyword.").Value;

            Expect(LexerTokenType::OpenBrace, "Expected '{' after class name.");

            Vector<Shared<Statement>> Body;

            while (Current().Type != LexerTokenType::CloseBrace)
            {
                Body.push_back(ParseFunctionDeclaration());
            }

            Advance(); // Skip '}'

            Expect(LexerTokenType::Semicolon, "Expected ';' after class declaration");

            return std::make_shared<ClassDeclaration>(Name, Body);
        }

    public:
        Shared<Program> ProduceAST(String sourceCode)
        {
            Lexer lexer(sourceCode);
            Tokens = lexer.Tokenize();

            auto program = std::make_shared<Program>();

            while (NotEoF())
            {
                program->Body.emplace_back(ParseStatement());
            }

            return program;
        }
    };
}
