#pragma once
#include "Values/RuntimeValue.h"

#include "../Safety.h"
#include "../Types/FlatMap.h"

struct Enviroment : public std::enable_shared_from_this<Enviroment>
{
    Optional<Shared<Enviroment>> parent;

    FlatMap<String, Shared<RuntimeValue>> variables;

    Enviroment() = default;

    Enviroment(const Optional<Shared<Enviroment>>& Parent)
    {
        if (Parent.has_value())
        {
            parent = Parent;
        }
    }

    auto DeclareVar(String name, Shared<RuntimeValue> value, bool constant) -> Shared<RuntimeValue>;

    auto AssignVar(String name, Shared<RuntimeValue> value) -> Shared<RuntimeValue>;

    auto LookupVar(String name) -> Shared<RuntimeValue>;

    auto Resolve(String name) -> Shared<Enviroment>;
};