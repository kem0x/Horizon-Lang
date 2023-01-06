export module Lexer;

import <cctype>;
import <format>;
import Safety;
import Types.Core;
import Types.FlatMap;
import Extensions.Vector;
import Extensions.String;
import Reflection;

export
{
    enum class LexerTokenType
    {
        Unknown,
        Debug, // debug

        Number, // 123
        Identifier, // abc
        String, // "abc"

        Class, // class
        Super, // super
        Enum, // enum
        Function, // function
        Return, // return
        Let, // let
        Const, // const
        If, // if
        Else, // else
        Or, // "or" or ||
        And, // "and" or &&
        Loop, // loop
        Break, // break
        Continue, // continue
        Sync, // sync

        OpenParen, // (
        CloseParen, // )
        OpenBrace, // {
        CloseBrace, // }
        OpenBracket, // [
        CloseBracket, // ]

        Equal, // =
        EqualEqual, // ==
        Bang, // !
        BangEqual, // !=
        Less, // <
        LessEqual, // <=
        Greater, // >
        GreaterEqual, // >=
        Comma, // ,
        Dot, // .
        Colon, // :
        Semicolon, // ;
        BinaryOperator, // + - * / %

        EoF
    };

    class Lexer
    {
    public:
        struct Token
        {
            LexerTokenType Type;
            size_t Line;
            String Value;

            Token(LexerTokenType type, size_t line, StringView value)
                : Type(type)
                , Line(line)
                , Value(value)
            {
            }

            Token(LexerTokenType type, size_t line, char value)
                : Type(type)
                , Line(line)
                , Value(1, value)
            {
            }

            String ToString()
            {
                return std::format("{{\nType: '{}',\nValue: '{}'\n}}", Reflection::EnumToString(Type), Value);
            }
        };

    private:
        static constexpr const FlatMap<StringView, LexerTokenType, 18> ReservedKeywords = {
            { {
                { "debug", LexerTokenType::Debug },
                { "class", LexerTokenType::Class },
                { "super", LexerTokenType::Super },
                { "enum", LexerTokenType::Enum },
                { "function", LexerTokenType::Function },
                { "return", LexerTokenType::Return },
                { "let", LexerTokenType::Let },
                { "const", LexerTokenType::Const },
                { "if", LexerTokenType::If },
                { "else", LexerTokenType::Else },
                { "or", LexerTokenType::Or },
                { "and", LexerTokenType::And },
                { "same", LexerTokenType::EqualEqual },
                { "not", LexerTokenType::BangEqual },
                { "loop", LexerTokenType::Loop },
                { "break", LexerTokenType::Break },
                { "continue", LexerTokenType::Continue },
                { "sync", LexerTokenType::Sync },
            } }
        };

        String Source;

        size_t Line = 0;

        Vector<Token> Tokens;

        bool IsAtEnd() const
        {
            return Source.empty();
        }

        char Current()
        {
            return Source[0];
        }

        char Next()
        {
            return Source[1];
        }

        bool MatchNext(char c)
        {
            const bool matched = !Source.empty() and Source[1] == c;

            if (matched)
                Advance();

            return matched;
        }

        char Advance()
        {
            return Source | StringExtensions::Shift;
        }

        void AddToken(LexerTokenType type)
        {
            Tokens.emplace_back(type, Line, Advance());
        }

        void AddToken(LexerTokenType type, StringView value)
        {
            Tokens.emplace_back(type, Line, value);
        }

        void HandleCommentOrSubstraction()
        {
            if (MatchNext('/'))
            {
                while (!IsAtEnd() and Current() != '\n')
                {
                    Advance();
                }
            }
            else
            {
                AddToken(LexerTokenType::BinaryOperator);
            }
        }

        void HandleStringLitearls()
        {
            String StringValue;

            Advance();
            while (!IsAtEnd() and Current() != '"')
            {
                StringValue += Advance();
            }
            if (IsAtEnd())
            {
                throw std::runtime_error(std::format("Unterminated string at line {}", Line));
            }
            Advance();

            AddToken(LexerTokenType::String, StringValue);
        }

        void HandleNumaricLiteral()
        {
            String Number;
            while (!IsAtEnd() and std::isdigit(Current()))
            {
                Number += Advance();
            }

            if (Current() == '.' and std::isdigit(Next()))
            {
                Advance();

                while (!IsAtEnd() and std::isdigit(Current()))
                {
                    Number += Advance();
                }
            }

            AddToken(LexerTokenType::Number, Number);
        }

        void HandleKeywordOrIdentifier()
        {
            String Identifier;

            while (!IsAtEnd() and std::isalpha(Current()))
            {
                Identifier += Advance();
            }

            if (ReservedKeywords.has(Identifier))
            {
                AddToken(ReservedKeywords[Identifier], Identifier);
            }
            else
            {
                AddToken(LexerTokenType::Identifier, Identifier);
            }
        }

        void HandleMultiCharacterToken()
        {
            if (Current() == '/')
            {
                HandleCommentOrSubstraction();
            }
            else if (Current() == '"')
            {
                HandleStringLitearls();
            }
            else if (Current() == '&' and Next() == '&')
            {
                Advance();
                AddToken(LexerTokenType::And);
            }
            else if (Current() == '|' and Next() == '|')
            {
                Advance();
                AddToken(LexerTokenType::Or);
            }
            else if (std::isdigit(Current()))
            {
                HandleNumaricLiteral();
            }
            else if (std::isalpha(Current()))
            {
                HandleKeywordOrIdentifier();
            }
            else if (std::isspace(Current()))
            {
                Advance();
            }
            else
            {
                Safety::Throw("Unexpected character: " + Current());
            }
        }

    public:
        Lexer(String source)
            : Source(source)
        {
        }

        Vector<Token> Tokenize()
        {
            while (!IsAtEnd())
            {
                switch (Current())
                {
                case '!':
                    AddToken(MatchNext('=') ? LexerTokenType::BangEqual : LexerTokenType::Bang);
                    break;
                case '=':
                    AddToken(MatchNext('=') ? LexerTokenType::EqualEqual : LexerTokenType::Equal);
                    break;
                case '<':
                    AddToken(MatchNext('=') ? LexerTokenType::LessEqual : LexerTokenType::Less);
                    break;
                case '>':
                    AddToken(MatchNext('=') ? LexerTokenType::GreaterEqual : LexerTokenType::Greater);
                    break;
                case ',':
                    AddToken(LexerTokenType::Comma);
                    break;
                case '.':
                    AddToken(LexerTokenType::Dot);
                    break;
                case ':':
                    AddToken(LexerTokenType::Colon);
                    break;
                case ';':
                    AddToken(LexerTokenType::Semicolon);
                    break;
                case '(':
                    AddToken(LexerTokenType::OpenParen);
                    break;
                case ')':
                    AddToken(LexerTokenType::CloseParen);
                    break;
                case '{':
                    AddToken(LexerTokenType::OpenBrace);
                    break;
                case '}':
                    AddToken(LexerTokenType::CloseBrace);
                    break;
                case '[':
                    AddToken(LexerTokenType::OpenBracket);
                    break;
                case ']':
                    AddToken(LexerTokenType::CloseBracket);
                    break;
                case '+':
                case '-':
                case '*':
                case '%':
                    AddToken(LexerTokenType::BinaryOperator);
                    break;
                case '\n':
                    Line++;
                    Advance();
                    break;
                default:
                    HandleMultiCharacterToken();
                    break;
                }
            }

            AddToken(LexerTokenType::EoF, "");
            return Tokens;
        }
    };
}
