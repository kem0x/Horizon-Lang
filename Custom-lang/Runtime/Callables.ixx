export module Runtime.Callables;

import <optional>;
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
        Ctor,
        Class,
        Runtime,
        Native,
        Lambda,
    };

    struct Callable : public ObjectValue
    {
        CallableType CallType;

        Callable(CallableType callType, String typeName)
            : ObjectValue { RuntimeValueType::Callable, typeName, true }
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
        Optional<Shared<RuntimeValue>> Parent;

        RuntimeFunction(Shared<FunctionDeclaration> declaration, Optional<Shared<RuntimeValue>> parent = std::nullopt)
            : Callable { CallableType::Runtime, "Function" }
            , Declaration(declaration)
            , Parent(parent)
        {
        }

        virtual Shared<RuntimeValue> Call(Shared<struct ExecutionContext> context, const Vector<Shared<Expr>>& arguments) override;
    };

    struct CtorFunction : public Callable
    {
        String Name;
        Optional<Shared<FunctionDeclaration>> Declaration;
        Vector<Shared<FunctionDeclaration>> Methods;

        CtorFunction(String name, Optional<Shared<FunctionDeclaration>> declaration, Vector<Shared<FunctionDeclaration>> methods)
            : Callable { CallableType::Ctor, "CtorFunction" }
            , Name(name)
            , Declaration(declaration)
            , Methods(methods)
        {
        }

        virtual Shared<RuntimeValue> Call(Shared<struct ExecutionContext> context, const Vector<Shared<Expr>>& arguments);
    };

    struct NativeFunction : Callable
    {
        std::function<Shared<RuntimeValue>(const Vector<Shared<RuntimeValue>>)> Function;

        NativeFunction(std::function<Shared<RuntimeValue>(const Vector<Shared<RuntimeValue>>)> function)
            : Callable { CallableType::Native, "NativeFunction" }
            , Function(function)
        {
        }

        virtual Shared<RuntimeValue> Call(Shared<struct ExecutionContext> context, const Vector<Shared<Expr>>& arguments) override;
    };
}
