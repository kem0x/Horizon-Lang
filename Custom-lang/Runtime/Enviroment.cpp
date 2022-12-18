#include "Enviroment.h"

auto Enviroment::DeclareVar(std::string name, RuntimeValuePtr value, bool constant) -> RuntimeValuePtr
{
    if (variables.has(name))
    {
        Throw(std::format("Variable '{}' was already declared in the current scope!", name.c_str()));
    }

    value->IsConstant = constant;

    variables.set(name, value);

    return value;
}

auto Enviroment::AssignVar(std::string name, RuntimeValuePtr value) -> RuntimeValuePtr
{
    auto env = Resolve(name);

    if (env->variables.at(name)->IsConstant)
    {
        Throw(std::format("Variable '{}' is constant and cannot be assigned to!", name.c_str()));
    }

    env->variables.set(name, value);

    return value;
}

auto Enviroment::LookupVar(std::string name) -> RuntimeValuePtr
{
    auto env = Resolve(name);

    return env->variables.at(name);
}

auto Enviroment::Resolve(std::string name) -> std::shared_ptr<Enviroment>
{
    if (variables.has(name))
    {
        return shared_from_this();
    }

    if (!parent.has_value())
    {
        Throw(std::format("Variable '{}' was not found, unable to resolve!", name.c_str()));
    }

    return parent.value()->Resolve(name);
}