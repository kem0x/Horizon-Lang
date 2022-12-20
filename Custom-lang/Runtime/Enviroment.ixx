export module Runtime.Enviroment;

import<memory>;
import<format>;
import Safety;
import Types.Core;
import Types.FlatMap;
import Runtime.RuntimeValue;
import Runtime.BoolValue;
import Runtime.NullValue;

export
{
    struct Enviroment : public std::enable_shared_from_this<Enviroment>
    {
        Optional<Shared<Enviroment>> Parent;

        FlatMap<String, Shared<RuntimeValue>> Variables;

        Enviroment() = default;

        Enviroment(const Optional<Shared<Enviroment>>& parent, bool isGlobal = false)
        {
            if (isGlobal)
            {
                DeclareVar("true", BoolValue(true).As<BoolValue>(), true);
                DeclareVar("false", BoolValue(false).As<BoolValue>(), true);
                DeclareVar("null", NullValue().As<NullValue>(), true);
            }

            if (parent.has_value())
            {
                Parent = parent;
            }
        }

        auto DeclareVar(String name, Shared<RuntimeValue> value, bool constant) -> Shared<RuntimeValue>
        {
            if (Variables.has(name))
            {
                Safety::Throw(std::format("Variable '{}' was already declared in the current scope!", name.c_str()));
            }

            value->IsConstant = constant;

            Variables.set(name, value);

            return value;
        }

        auto AssignVar(String name, Shared<RuntimeValue> value) -> Shared<RuntimeValue>
        {
            auto env = Resolve(name);

            if (env->Variables.at(name)->IsConstant)
            {
                Safety::Throw(std::format("Variable '{}' is constant and cannot be assigned to!", name.c_str()));
            }

            env->Variables.set(name, value);

            return value;
        }

        auto LookupVar(String name) -> Shared<RuntimeValue>
        {
            auto env = Resolve(name);

            return env->Variables.at(name);
        }

        auto Resolve(String name) -> Shared<Enviroment>
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