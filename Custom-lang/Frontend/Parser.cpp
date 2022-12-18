#include "Parser.h"

auto Parser::ParseAdditiveExpr() -> ExprPtr
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

auto Parser::ParseMultiplicativeExpr() -> ExprPtr
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

auto Parser::ParsePrimaryExpr() -> ExprPtr
{
    const auto tk = Current().Type;

    switch (tk)
    {
    case TokenType::Identifier:
        return std::make_shared<Identifier>(Advance().Value);

    case TokenType::Number:
        return std::make_shared<NumericLiteral>(std::stof(Advance().Value));

    case TokenType::OpenParen:
    {
        Advance(); // Skip '('

        auto Value = ParseAdditiveExpr();

        Expect(TokenType::CloseParen, "Unexpected token: " + Current().Value + ", expected a ')'");

        return Value;
    }

    default:
        return Throw<ExprPtr>(std::format("Unexpected token ({}) found during parsing!", Current().ToString().c_str()));
    }
}

auto Parser::ParseAssignmentExpr() -> ExprPtr
{
    const auto left = ParseAdditiveExpr(); // switch this out with objectExpr

    if (Current().Type == TokenType::Equals)
    {
        Advance(); // Skip '='

        const auto right = ParseAssignmentExpr();

        return std::make_shared<AssignmentExpr>(left, right);
    }

    return left;
}

auto Parser::ParseVariableDeclartion() -> StatementPtr
{
    const auto isConstant = Advance().Type == TokenType::Const;

    const auto identifier = Expect(TokenType::Identifier, "Expected identifier name after (let | const) keyword.").Value;

    if (Current().Type == TokenType::Semicolon)
    {
        Advance(); // Skip ';'

        if (isConstant)
        {
            Throw(std::format("A const variable '{}' must be initialized with a value!", identifier.c_str()));
        }

        return std::make_shared<VariableDeclaration>(identifier, std::nullopt, isConstant);
    }

    Expect(TokenType::Equals, std::format("Expected '=' after identifier name '{}'.", identifier.c_str()));

    const auto declaration = std::make_shared<VariableDeclaration>(identifier, ParseExpr(), isConstant);

    Expect(TokenType::Semicolon, std::format("Expected ';' after variable declaration '{}'.", identifier.c_str()));

    return declaration;
}