#pragma once

#include "AST.h"
#include "../Safety.h"
#include "../Types/Extensions/Vector.h"

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

    auto ParseAdditiveExpr() -> Shared<Expr>;

    auto ParseMultiplicativeExpr() -> Shared<Expr>;

    auto ParsePrimaryExpr() -> Shared<Expr>;

    auto ParseAssignmentExpr() -> Shared<Expr>;

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

    auto ParseVariableDeclartion() -> Shared<Statement>;

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
