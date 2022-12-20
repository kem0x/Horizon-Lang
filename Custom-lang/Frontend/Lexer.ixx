export module Lexer;

import<cctype>;
import<format>;
import Safety;
import Types.Core;
import Types.FlatMap;
import Extensions.Vector;
import Extensions.String;

export
{
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
        OpenBrace,
        CloseBrace,
        Equals,
        Comma,
        Colon,
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
                return std::format("{{\nType: '{}',\nValue: '{}'\n}}", std::to_string(int(Type)), Value);
            }
        };

    private:
        static constexpr FlatMap<StringView, LexerTokenType, 2> ReservedKeywords = {
            { { { "let", LexerTokenType::Let }, { "const", LexerTokenType::Const } } }
        };

        String Source;

        Vector<Token> Tokens;

        __forceinline auto Advance() -> char
        {
            return StringExtensions::Shift(Source);
        }

        auto HandleMultiCharacterToken() -> void
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
                Safety::Throw("Unexpected character: " + Source[0]);
            }
        }

    public:
        Lexer(String source)
            : Source(source)
        {
        }

        auto Tokenize() -> Vector<Token>
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
    };
}