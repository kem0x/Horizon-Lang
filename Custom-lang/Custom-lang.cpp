#include <iostream>
#include "Frontend/Parser.h"
#include "Logger.h"
#include "Runtime/Interpreter.h"
#include "Runtime/Values/BoolValue.h"

int main()
{
    auto parser = Parser();
    auto env = std::make_shared<Enviroment>();

    env->DeclareVar("true", BoolValue(true).GetAsShared(), true);
    env->DeclareVar("false", BoolValue(false).GetAsShared(), true);
    env->DeclareVar("null", NullValue().GetAsShared(), true);

    Log<Info>("Interpreter V0.1");

    while (true)
    {
        printf("\n> ");
        std::string input;
        std::getline(std::cin, input);

        if (input.empty() || input == "exit")
            break;

        auto program = parser.ProduceAST(input);

        // Log<Info>("Program: %s", program->ToString().c_str());

        auto result = Evaluate(program, env);

        Log<Info>("Result: %s", result->ToString().c_str());
    }

    return 0;
}