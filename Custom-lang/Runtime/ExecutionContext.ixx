export module Runtime.ExecutionContext;

import <memory>;
import <format>;
import Safety;
import Types.Core;
import Types.FlatMap;
import Runtime.RuntimeValue;
import Runtime.BoolValue;
import Runtime.NullValue;

export
{
    struct ExecutionContext : public std::enable_shared_from_this<ExecutionContext>
    {
        Optional<Shared<ExecutionContext>> Parent;

        FlatMap<String, Shared<RuntimeValue>> Variables;

        bool IsGlobalContext = false;

        bool IsFunctionContext = false;
        bool ShouldReturn = false;

        bool IsLoopContext = false;
        bool ShouldBreak = false;
        bool ShouldContinue = false;

        ExecutionContext() = default;

        ExecutionContext(const Optional<Shared<ExecutionContext>>& parent, bool isGlobal = false)
        {
            if (isGlobal)
            {
                IsGlobalContext = true;
                DeclareVar("true", BoolValue(true).As<BoolValue>(), true);
                DeclareVar("false", BoolValue(false).As<BoolValue>(), true);
                DeclareVar("null", NullValue().As<NullValue>(), true);
            }

            if (parent.has_value())
            {
                Parent = parent;
            }
        }

        Shared<RuntimeValue> DeclareVar(String name, Shared<RuntimeValue> value, bool constant)
        {
            if (Variables.has(name))
            {
                Safety::Throw(std::format("Variable '{}' was already declared in the current scope!", name.c_str()));
            }

            value->IsConstant = constant;

            Variables.set(name, value);

            return value;
        }

        Shared<RuntimeValue> AssignVar(String name, Shared<RuntimeValue> value)
        {
            auto ctx = Resolve(name);

            if (ctx->Variables.at(name)->IsConstant)
            {
                Safety::Throw(std::format("Variable '{}' is constant and cannot be assigned to!", name.c_str()));
            }

            ctx->Variables.set(name, value);

            return value;
        }

        Shared<RuntimeValue> LookupVar(String name)
        {
            auto ctx = Resolve(name);

            return ctx->Variables.at(name);
        }

        Shared<ExecutionContext> Resolve(String name)
        {
            if (Variables.has(name))
            {
                return shared_from_this();
            }

            if (!Parent.has_value())
            {
                Safety::Throw(std::format("Variable '{}' was not found, unable to resolve!", name.c_str()));
            }

            return Parent.value()->Resolve(name);
        }
    };
}
