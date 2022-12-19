#include "Lexer.h"

auto Lexer::HandleMultiCharacterToken() -> void
{
    if (std::isdigit(Source[0]))
    {
        String Number;
        while (Source.size() > 0 && std::isdigit(Source[0]))
        {
            Number += Advance();
        }

        Tokens.push_back({ LexerTokenType::Number, Number });
    }
    else if (std::isalpha(Source[0]))
    {
        String Identifier;

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
            Tokens.push_back({ LexerTokenType::Identifier, Identifier });
        }
    }
    else if (std::isspace(Source[0]))
    {
        Advance();
    }
    else
    {
        // Safety::Throw("Unexpected character: " + Source[0]);
    }
}

auto Lexer::Tokenize() -> Vector<Lexer::Token>
{
    while (Source.size() > 0)
    {
        switch (Source[0])
        {
        case ',':
            Tokens.push_back({ LexerTokenType::Comma, Advance() });
            break;
        case ':':
            Tokens.push_back({ LexerTokenType::Colon, Advance() });
            break;
        case ';':
            Tokens.push_back({ LexerTokenType::Semicolon, Advance() });
            break;
        case '=':
            Tokens.push_back({ LexerTokenType::Equals, Advance() });
            break;
        case '(':
            Tokens.push_back({ LexerTokenType::OpenParen, Advance() });
            break;
        case ')':
            Tokens.push_back({ LexerTokenType::CloseParen, Advance() });
            break;
        case '{':
            Tokens.push_back({ LexerTokenType::OpenBrace, Advance() });
            break;
        case '}':
            Tokens.push_back({ LexerTokenType::CloseBrace, Advance() });
            break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
            Tokens.push_back({ LexerTokenType::BinaryOperator, Advance() });
            break;
        default:
            HandleMultiCharacterToken();
            break;
        }
    }

    Tokens.push_back({ LexerTokenType::EoF, "" });
    return Tokens;
}