export module Runtime.ExecutionContext;

import <thread>;
import <memory>;
import <format>;
import Safety;
import Types.Core;
import Extensions.Vector;

import Reflection;

import Runtime.RuntimeValue;
import Runtime.BoolValue;
import Runtime.NullValue;
import Runtime.NumberValue;
import Runtime.StringValue;
import Runtime.Callables;
import Runtime.ObjectValue;
import Runtime.ArrayValue;

String Format(const String& fmt, const Vector<String>& args)
{
    String Result;

    size_t i = 0;
    size_t j = 0;
    size_t ArgIndex = 0;

    while (i < fmt.size())
    {
        if (fmt[i] == '{')
        {
            Result += fmt.substr(j, i - j);
            i++;

            if (i < fmt.size() && fmt[i] == '{')
            {
                Result += '{';
                i++;
                j = i;
            }
            else
            {
                size_t EndIndex = fmt.find('}', i);
                if (EndIndex == String::npos)
                {
                    Safety::Throw("Unmatched '{' in format string");
                }

                String PlaceHolder = fmt.substr(i, EndIndex - i);

                size_t Index;
                if (PlaceHolder.empty())
                {
                    Index = ArgIndex;
                }
                else
                {
                    try
                    {
                        Index = std::stoi(PlaceHolder);
                    }
                    catch (const std::invalid_argument&)
                    {
                        Safety::Throw("Invalid argument index in format string");
                    }
                    catch (const std::out_of_range&)
                    {
                        Safety::Throw("Argument index out of range in format string");
                    }
                }

                if (Index < 0 || Index >= (int)args.size())
                {
                    Safety::Throw("Argument index out of range in format string");
                }

                Result += args[Index];
                i = EndIndex + 1;
                j = i;
                ArgIndex++;
            }
        }
        else
        {
            i++;
        }
    }

    Result += fmt.substr(j);
    return Result;
}

template <typename Container>
String Join(const Container& container, const String& separator)
{
    String result;
    auto it = container.begin();
    while (it != container.end())
    {
        result += *it;
        ++it;
        if (it != container.end())
        {
            result += separator;
        }
    }
    return result;
}

Shared<RuntimeValue> internalFormat(Vector<Shared<RuntimeValue>> args)
{
    if (args.size() == 0)
    {
        Safety::Throw("format() requires at least one argument");
    }

    Vector<String> ArgsStrings;

    for (auto& Arg : args)
    {
        switch (Arg->Type)
        {
        case RuntimeValueType::BoolValue:
        {
            ArgsStrings.emplace_back(Arg->As<BoolValue>()->Value ? "true" : "false");
            break;
        }
        case RuntimeValueType::NumberValue:
        {
            ArgsStrings.emplace_back(std::to_string(Arg->As<NumberValue>()->Value));
            break;
        }
        case RuntimeValueType::StringValue:
        {
            ArgsStrings.emplace_back(Arg->As<StringValue>()->Value);
            break;
        }
        case RuntimeValueType::NullValue:
        {
            ArgsStrings.emplace_back("null");
            break;
        }
        default:
        {
            Safety::Throw("Invalid argument type for print()");
            break;
        }
        }
    }

    String str;
    if (args[0]->Type == RuntimeValueType::StringValue)
    {
        return std::make_shared<StringValue>(Format(VectorExtensions::Shift(ArgsStrings), ArgsStrings));
    }

    return std::make_shared<NullValue>();
}

Shared<RuntimeValue> internalPrint(Vector<Shared<RuntimeValue>> args)
{
    Vector<String> ArgsStrings;
    ArgsStrings.reserve(args.size());

    for (auto& Arg : args)
    {
        switch (Arg->Type)
        {
        case RuntimeValueType::BoolValue:
        {
            ArgsStrings.emplace_back(Arg->As<BoolValue>()->Value ? "true" : "false");
            break;
        }
        case RuntimeValueType::NumberValue:
        {
            ArgsStrings.emplace_back(std::to_string(Arg->As<NumberValue>()->Value));
            break;
        }
        case RuntimeValueType::StringValue:
        {
            ArgsStrings.emplace_back(Arg->As<StringValue>()->Value);
            break;
        }
        case RuntimeValueType::NullValue:
        {
            ArgsStrings.emplace_back("null");
            break;
        }
        case RuntimeValueType::ObjectValue:
        {
            ArgsStrings.emplace_back(Arg->As<ObjectValue>()->ToString());
            break;
        }

        case RuntimeValueType::ArrayValue:
        {
            ArgsStrings.emplace_back(Arg->As<ArrayValue>()->ToString());
            break;
        }

        default:
        {
            Safety::Throw(std::format("Invalid argument of type {} for print()", Reflection::EnumToString(Arg->Type)));
            break;
        }
        }
    }

    String str;
    if (args[0]->Type == RuntimeValueType::StringValue)
    {
        str = Format(VectorExtensions::Shift(ArgsStrings), ArgsStrings);
    }
    else
    {
        str = Join(ArgsStrings, " ");
    }

    printf("%s\n", str.c_str());

    return std::make_shared<NullValue>();
}

Shared<RuntimeValue> internalSleep(Vector<Shared<RuntimeValue>> args)
{
    if (args.size() != 1)
    {
        Safety::Throw("sleep() requires one argument");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds((int)args[0]->As<NumberValue>()->Value));

    return std::make_shared<NullValue>();
}

export
{
    struct ExecutionContext : public std::enable_shared_from_this<ExecutionContext>
    {
        Optional<Shared<ExecutionContext>> Parent;

        UnorderedMap<String, Shared<RuntimeValue>> Variables;

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

                DeclareVar("true", std::make_shared<BoolValue>(true), true);
                DeclareVar("false", std::make_shared<BoolValue>(false), true);
                DeclareVar("null", std::make_shared<NullValue>(), true);

                DeclareVar("print", std::make_shared<NativeFunction>(internalPrint), true);
                DeclareVar("format", std::make_shared<NativeFunction>(internalFormat), true);
                DeclareVar("sleep", std::make_shared<NativeFunction>(internalSleep), true);

                DeclareVar("Object", ObjectValue::DefaultObject(), true);
                DeclareVar("Array", ArrayValue::DefaultObject(), true);
                DeclareVar("String", StringValue::DefaultObject(), true);
                DeclareVar("Number", NumberValue::DefaultObject(), true);
                DeclareVar("Bool", BoolValue::DefaultObject(), true);
                DeclareVar("Null", NullValue::DefaultObject(), true);
            }

            if (parent.has_value())
            {
                Parent = parent;
            }
        }

        Shared<RuntimeValue> DeclareVar(String name, Shared<RuntimeValue> value, bool constant)
        {
            if (Variables.contains(name))
            {
                Safety::Throw(std::format("Variable '{}' was already declared in the current scope!", name.c_str()));
            }

            value->IsConstant = constant;

            Variables.insert_or_assign(name, value);

            return value;
        }

        Shared<RuntimeValue> AssignVar(String name, Shared<RuntimeValue> value)
        {
            auto ctx = Resolve(name);

            /*if (ctx->Variables.at(name)->IsConstant)
            {
                Safety::Throw(std::format("Variable '{}' is constant and cannot be assigned to!", name.c_str()));
            }*/

            ctx->Variables.insert_or_assign(name, value);

            return value;
        }

        Shared<RuntimeValue> LookupVar(String name)
        {
            auto ctx = Resolve(name);

            return ctx->Variables.at(name);
        }

        Shared<ExecutionContext> Resolve(String name)
        {
            if (Variables.contains(name))
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
