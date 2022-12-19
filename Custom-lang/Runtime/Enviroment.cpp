#include "Enviroment.h"

auto Enviroment::DeclareVar(String name, Shared<RuntimeValue> value, bool constant) -> Shared<RuntimeValue>
{
    if (variables.has(name))
    {
        Safety::Throw(std::format("Variable '{}' was already declared in the current scope!", name.c_str()));
    }

    value->IsConstant = constant;

    variables.set(name, value);

    return value;
}

auto Enviroment::AssignVar(String name, Shared<RuntimeValue> value) -> Shared<RuntimeValue>
{
    auto env = Resolve(name);

    if (env->variables.at(name)->IsConstant)
    {
        Safety::Throw(std::format("Variable '{}' is constant and cannot be assigned to!", name.c_str()));
    }

    env->variables.set(name, value);

    return value;
}

auto Enviroment::LookupVar(String name) -> Shared<RuntimeValue>
{
    auto env = Resolve(name);

    return env->variables.at(name);
}

auto Enviroment::Resolve(String name) -> Shared<Enviroment>
{
    if (variables.has(name))
    {
        return shared_from_this();
    }

    if (!parent.has_value())
    {
        Safety::Throw(std::format("Variable '{}' was not found, unable to resolve!", name.c_str()));
    }

    return parent.value()->Resolve(name);
}