#pragma once
#include "../Types/Core.h"

#include "../Safety.h"
#include "../Types/FlatMap.h"
#include "../Types/Extensions/String.h"

enum class LexerTokenType
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
        LexerTokenType Type;
        String Value;

        Token(LexerTokenType type, StringView value)
            : Type(type)
            , Value(value)
        {
        }

        Token(LexerTokenType type, char value)
            : Type(type)
            , Value(1, value)
        {
        }

        auto ToString() -> String
        {
            return ("Type: " + std::to_string((int)Type) + " | Value: " + Value);
        }
    };

private:
    const static constexpr FlatMap<StringView, LexerTokenType, 2> ReservedKeywords = {
        { { { "let", LexerTokenType::Let }, { "const", LexerTokenType::Const } } }
    };

    String Source;

    Vector<Token> Tokens;

    __forceinline auto Advance() -> char
    {
        return StringExtensions::Shift(Source);
    }

    auto HandleMultiCharacterToken() -> void;

public:
    Lexer(String source)
        : Source(source)
    {
    }

    auto Tokenize() -> Vector<Token>;
};