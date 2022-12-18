#pragma once

#include "AST.h"
#include "../Error.h"
#include "../Types/Extensions/Vector.h"

class Parser
{
    std::vector<Lexer::Token> Tokens;

    auto NotEoF() -> bool
    {
        return Tokens[0].Type != TokenType::EoF;
    }

    auto Current() -> Lexer::Token
    {
        return Tokens[0];
    }

    auto Advance() -> Lexer::Token
    {
        return VectorExtensions::Shift(Tokens);
    }

    auto Expect(TokenType type, std::string error) -> Lexer::Token
    {
        if (Current().Type != type)
        {
            Throw(error);
        }

        return Advance();
    }

    auto ParseAdditiveExpr() -> ExprPtr;

    auto ParseMultiplicativeExpr() -> ExprPtr;

    auto ParsePrimaryExpr() -> ExprPtr;

    auto ParseAssignmentExpr() -> ExprPtr;

    auto ParseExpr() -> ExprPtr
    {
        return ParseAssignmentExpr();
    }

    auto ParseStatement() -> StatementPtr
    {
        switch (Current().Type)
        {
        case TokenType::Let:
        case TokenType::Const:
            return ParseVariableDeclartion();

        default:
            return ParseExpr();
        }
    }

    auto ParseVariableDeclartion() -> StatementPtr;

public:
    auto ProduceAST(std::string sourceCode) -> ProgramPtr
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
