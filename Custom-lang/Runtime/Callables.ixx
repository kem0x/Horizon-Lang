export module Runtime.Callables;

import <functional>;
import <format>;
import Types.Core;
import AST.Core;
import AST.Statements;
import AST.Expressions;
import Runtime.RuntimeValue;
import Runtime.NullValue;

import Reflection;

export
{
    enum class CallabeType
    {
        Unknown,
        Runtime,
        Native,
        Lambda,
    };

    struct Callable : public RuntimeValue
    {
        CallabeType CallType;

        Callable(CallabeType callType)
            : RuntimeValue { RuntimeValueType::Callable }
            , CallType(callType)
        {
        }

        virtual Shared<RuntimeValue> Call(Shared<struct ExecutionContext> context, const Vector<Shared<Expr>>& arguments)
        {
            return nullptr;
        };

        virtual String ToString() override
        {
            return std::format("{{\nType: '{}',\nCallType: '{}'\n}}", Reflection::EnumToString(Type), Reflection::EnumToString(CallType));
        };
    };

    struct RuntimeFunction : Callable
    {
        Shared<FunctionDeclaration> Declaration;

        RuntimeFunction(Shared<FunctionDeclaration> declaration)
            : Callable { CallabeType::Runtime }
            , Declaration(std::move(declaration))
        {
        }

        virtual Shared<RuntimeValue> Call(Shared<struct ExecutionContext> context, const Vector<Shared<Expr>>& arguments) override;
    };

    struct NativeFunction : Callable
    {
        std::function<Shared<RuntimeValue>(const Vector<Shared<RuntimeValue>>)> Function;

        NativeFunction(std::function<Shared<RuntimeValue>(const Vector<Shared<RuntimeValue>>)> function)
            : Callable { CallabeType::Native }
            , Function(function)
        {
        }

        virtual Shared<RuntimeValue> Call(Shared<struct ExecutionContext> context, const Vector<Shared<Expr>>& arguments) override;
    };
}