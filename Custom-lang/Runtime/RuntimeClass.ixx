export module Runtime.Class;

import <functional>;
import <format>;
import Types.Core;
import AST.Core;
import AST.Statements;
import AST.Expressions;
import Runtime.RuntimeValue;
import Runtime.NullValue;
import Runtime.ObjectValue;
import Runtime.Callables;

import Reflection;

export
{
    struct RuntimeClass : public Callable
    {
        String Name;
        Optional<Shared<FunctionDeclaration>> Constructor;
        Vector<Shared<FunctionDeclaration>> Methods;

        RuntimeClass(String name, Optional<Shared<FunctionDeclaration>> constructor, Vector<Shared<FunctionDeclaration>> methods)
            : Callable { CallableType::Class }
            , Name(name)
            , Constructor(constructor)
            , Methods(methods)
        {
        }

        virtual Shared<RuntimeValue> Call(Shared<struct ExecutionContext> context, const Vector<Shared<Expr>>& arguments);
    };
}