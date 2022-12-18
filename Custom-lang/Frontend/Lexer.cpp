#include "Lexer.h"

auto Lexer::HandleMultiCharacterToken() -> void
{
    if (std::isdigit(Source[0]))
    {
        std::string Number;
        while (Source.size() > 0 && std::isdigit(Source[0]))
        {
            Number += Advance();
        }

        Tokens.push_back({ TokenType::Number, Number });
    }
    else if (std::isalpha(Source[0]))
    {
        std::string Identifier;

        while (Source.size() > 0 && std::isalpha(Source[0]))
        {
            Identifier += Advance();
        }

        if (ReservedKeywords.has(Identifier))
        {
            Tokens.push_back({ ReservedKeywords[Identifier], Identifier });
        }
        else
        {
            Tokens.push_back({ TokenType::Identifier, Identifier });
        }
    }
    else if (std::isspace(Source[0]))
    {
        Advance();
    }
    else
    {
        Throw("Unexpected character: " + Source[0]);
    }
}

auto Lexer::Tokenize() -> std::vector<Lexer::Token>
{
    while (Source.size() > 0)
    {
        switch (Source[0])
        {
        case ';':
            Tokens.push_back({ TokenType::Semicolon, Advance() });
            break;
        case '=':
            Tokens.push_back({ TokenType::Equals, Advance() });
            break;
        case '(':
            Tokens.push_back({ TokenType::OpenParen, Advance() });
            break;
        case ')':
            Tokens.push_back({ TokenType::CloseParen, Advance() });
            break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
            Tokens.push_back({ TokenType::BinaryOperator, Advance() });
            break;
        default:
            HandleMultiCharacterToken();
            break;
        }
    }

    Tokens.push_back({ TokenType::EoF, "" });
    return Tokens;
}