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

int main(int argc, char** argv)
{

#ifdef _DEBUG
    Log<Info>("Debug build");
#endif

    Safety::Init();

    std::filesystem::path path;

    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
        if (argc == 1)
        {
            if (entry.path().extension() == ".hz")
            {
                path = entry.path();
                break;
            }
        }
        else if (argc == 2)
        {
            if (entry.path().filename() == argv[1])
            {
                path = entry.path();
                break;
            }
        }
    }

    if (path.empty())
    {
        Log<Error>("No files found in current directory!");
        return 1;
    }

    auto parser = std::make_shared<Parser>();
    auto ctx = std::make_shared<ExecutionContext>(std::nullopt, true);

    const auto input = path | StringExtensions::ReadFile;

    Shared<Statement> program = parser->ProduceAST(input);

    const auto result = Evaluate(program, ctx);

    // Log<Info>("Result: %s", result->ToString().c_str());

    return 0;
}
