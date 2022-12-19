#include "Enviroment.h"

auto Enviroment::DeclareVar(String name, Shared<RuntimeValue> value, bool constant) -> Shared<RuntimeValue>
{
    if (Variables.has(name))
    {
        Safety::Throw(std::format("Variable '{}' was already declared in the current scope!", name.c_str()));
    }

    value->IsConstant = constant;

    Variables.set(name, value);

    return value;
}

auto Enviroment::AssignVar(String name, Shared<RuntimeValue> value) -> Shared<RuntimeValue>
{
    auto env = Resolve(name);

    if (env->Variables.at(name)->IsConstant)
    {
        Safety::Throw(std::format("Variable '{}' is constant and cannot be assigned to!", name.c_str()));
    }

    env->Variables.set(name, value);

    return value;
}

auto Enviroment::LookupVar(String name) -> Shared<RuntimeValue>
{
    auto env = Resolve(name);

    return env->Variables.at(name);
}

auto Enviroment::Resolve(String name) -> Shared<Enviroment>
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