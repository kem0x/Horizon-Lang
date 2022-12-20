import<fstream>;
import<filesystem>;

import Logger;

import Parser;
import Enviroment;
import Interpreter;
import Safety;

std::string readFile(std::filesystem::path path)
{
    std::ifstream f(path, std::ios::in | std::ios::binary);

    const auto size = std::filesystem::file_size(path);
    std::string result(size, '\0');

    f.read(result.data(), size);

    return result;
}

/*
void repl()
{
    auto parser = std::make_shared<Parser>();
    auto env = std::make_shared<Enviroment>();

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
}
*/

int main()
{
    Safety::Init();

    auto parser = Parser();
    auto env = std::make_shared<Enviroment>(std::nullopt, true);

    const auto input = "let foo = 30; foo / 2"; // readFile(std::filesystem::current_path() / "test.txt");

    auto program = parser.ProduceAST(input);

    const auto result = Evaluate(program, env);

    Log<Info>("Result: %s", result->ToString().c_str());

    return 0;
}