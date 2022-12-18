#pragma once
#include <string>
#include <string_view>
#include <vector>

#include "../Error.h"
#include "../Types/FlatMap.h"
#include "../Types/Extensions/String.h"

enum class TokenType
{
    Unknown,
    Number,
    Identifier,
    String,

    Let,
    Const,

    OpenParen,
    CloseParen,
    Equals,
    Semicolon,
    BinaryOperator,

    EoF
};

class Lexer
{
public:
    struct Token
    {
        TokenType Type;
        std::string Value;

        Token(TokenType type, std::string_view value)
            : Type(type)
            , Value(value)
        {
        }

        Token(TokenType type, char value)
            : Type(type)
            , Value(1, value)
        {
        }

        auto ToString() -> std::string
        {
            return ("Type: " + std::to_string((int)Type) + " | Value: " + Value);
        }
    };

private:
    const static constexpr FlatMap<std::string_view, TokenType, 2> ReservedKeywords = {
        { { { "let", TokenType::Let }, { "const", TokenType::Const } } }
    };

    std::string Source;

    std::vector<Token> Tokens;

    __forceinline auto Advance() -> char
    {
        return StringExtensions::Shift(Source);
    }

    auto HandleMultiCharacterToken() -> void;

public:
    Lexer(std::string source)
        : Source(source)
    {
    }

    auto Tokenize() -> std::vector<Token>;
};