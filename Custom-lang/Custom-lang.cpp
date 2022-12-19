#include "Safety.h"

#include "Types/Core.h"
#include "Frontend/Parser.h"
#include "Runtime/Interpreter.h"
#include "Runtime/Values/BoolValue.h"

int main()
{
    Safety::Init();

    auto parser = std::make_shared<Parser>();
    auto env = std::make_shared<Enviroment>();

    env->DeclareVar("true", BoolValue(true).As<BoolValue>(), true);
    env->DeclareVar("false", BoolValue(false).As<BoolValue>(), true);
    env->DeclareVar("null", NullValue().As<NullValue>(), true);

    Log<Info>("Interpreter V0.1");

    while (true)
    {
        printf("\n> ");
        String input;
        std::getline(std::cin, input);

        if (input.empty() || input == "exit")
            break;

        if (input == "debug")
        {
            continue;
        }

        auto program = parser->ProduceAST(input);

        // Log<Info>("Program: %s", program->ToString().c_str());

        auto result = Evaluate(program, env);

        Log<Info>("Result: %s", result->ToString().c_str());
    }

    return 0;
}