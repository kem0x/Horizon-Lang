import <fstream>;
import <filesystem>;

import Logger;
import Safety;
import Types.Core;

import Extensions.String;

import Parser;
import AST.Core;
import Runtime.ExecutionContext;
import Interpreter;

/*
void repl()
{
    auto parser = std::make_shared<Parser>();
    auto ctx = std::make_shared<ExecutionContext>();

    Log<Info>("Interpreter V0.1");

    while (true)
    {
        printf("\n> ");
        String input;
        std::getline(std::cin, input);

        if (input.empty() or input == "exit")
            break;

        if (input == "debug")
        {
            continue;
        }

        auto program = parser->ProduceAST(input);

        // Log<Info>("Program: %s", program->ToString().c_str());

        auto result = Evaluate(program, ctx);

        Log<Info>("Result: %s", result->ToString().c_str());
    }
}
*/

int main()
{
    Safety::Init();

    auto parser = std::make_shared<Parser>();
    auto ctx = std::make_shared<ExecutionContext>(std::nullopt, true);

    const auto path = std::filesystem::current_path() / "test.txt";

    const auto input = path | StringExtensions::ReadFile;

    Shared<Statement> program = parser->ProduceAST(input);

    const auto result = Evaluate(program, ctx);

    // Log<Info>("Result: %s", result->ToString().c_str());

    return 0;
}
