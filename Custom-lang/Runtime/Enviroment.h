#pragma once
#include "Values/RuntimeValue.h"
#include "Values/NumberValue.h"
#include "Values/BoolValue.h"
#include "Values/NullValue.h"

#include "../Safety.h"
#include "../Types/FlatMap.h"

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

    auto DeclareVar(String name, Shared<RuntimeValue> value, bool constant) -> Shared<RuntimeValue>;

    auto AssignVar(String name, Shared<RuntimeValue> value) -> Shared<RuntimeValue>;

    auto LookupVar(String name) -> Shared<RuntimeValue>;

    auto Resolve(String name) -> Shared<Enviroment>;
};