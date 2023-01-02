export module Runtime.Callables;

import <functional>;
import <format>;
import Types.Core;
import AST.Core;
import AST.Statements;
import AST.Expressions;
import Runtime.RuntimeValue;
import Runtime.ObjectValue;
import Runtime.NullValue;

import Reflection;

export
{
    enum class CallableType
    {
        Unknown,
        Class,
        Runtime,
        Native,
        Lambda,
    };

    struct Callable : public ObjectValue
    {
        CallableType CallType;

        Callable(CallableType callType)
            : ObjectValue { RuntimeValueType::Callable }
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
            : Callable { CallableType::Runtime }
            , Declaration(declaration)
        {
        }

        virtual Shared<RuntimeValue> Call(Shared<struct ExecutionContext> context, const Vector<Shared<Expr>>& arguments) override;
    };

    struct NativeFunction : Callable
    {
        std::function<Shared<RuntimeValue>(const Vector<Shared<RuntimeValue>>)> Function;

        NativeFunction(std::function<Shared<RuntimeValue>(const Vector<Shared<RuntimeValue>>)> function)
            : Callable { CallableType::Native }
            , Function(function)
        {
        }

        virtual Shared<RuntimeValue> Call(Shared<struct ExecutionContext> context, const Vector<Shared<Expr>>& arguments) override;
    };
}