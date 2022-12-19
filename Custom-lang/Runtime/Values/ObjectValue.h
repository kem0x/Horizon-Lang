#pragma once
#include "RuntimeValue.h"
#include "../../Types/FlatMap.h"

struct ObjectValue : public RuntimeValue
{
    FlatMap<String, Shared<RuntimeValue>> Properties;

    ObjectValue()
        : RuntimeValue { RuntimeValueType::ObjectValue }
    {
    }

    virtual auto ToString() -> String
    {
        String result = "{\n";
        for (auto i = 0; i < Properties.size(); i++)
        {
            result += std::format("\t{}: {},\n", Properties[i].first, Properties[i].second->ToString());
        }
        result += "}";
        return result;
    };
};