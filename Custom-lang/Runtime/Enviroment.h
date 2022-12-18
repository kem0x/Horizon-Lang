#pragma once
#include <format>
#include <optional>
#include "Values/RuntimeValue.h"

#include "../Error.h"
#include "../Types/FlatMap.h"

struct Enviroment : public std::enable_shared_from_this<Enviroment>
{
    std::optional<std::shared_ptr<Enviroment>> parent;

    FlatMap<std::string, RuntimeValuePtr> variables;

    Enviroment() = default;

    Enviroment(const std::optional<std::shared_ptr<Enviroment>>& Parent)
    {
        if (Parent.has_value())
        {
            parent = Parent;
        }
    }

    auto DeclareVar(std::string name, RuntimeValuePtr value, bool constant) -> RuntimeValuePtr;

    auto AssignVar(std::string name, RuntimeValuePtr value) -> RuntimeValuePtr;

    auto LookupVar(std::string name) -> RuntimeValuePtr;

    auto Resolve(std::string name) -> std::shared_ptr<Enviroment>;
};
typedef std::shared_ptr<Enviroment> EnviromentPtr;