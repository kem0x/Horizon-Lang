# Horizon programming language

This repo contains the source code for the experimental Horizon programming language's interpreter and base compiler.

## Disclaimer

This is a hobby project for educational purposes, and is not intended for production use. It is not feature complete, and is not guaranteed to be bug free. It is not intended to be a replacement for any other programming language, and is not intended to be used for any purpose other than for fun and learning.

## Features

- Statically typed
- Interpreted
- Javascript like syntax
- Fast execution speed

For a more detailed list of features, see features [document](FEATURES.MD)

## Examples

### Hello world

```haxe
print("Hello {}", "World");
```

### Fibonacci

```haxe
function fib(n: Int): Int
{
    if (n <= 1)
    {
        return n;
    }

    return fib(n - 1) + fib(n - 2);
}

print("fib(10) = {}", fib(10));
```

### FizzBuzz

```haxe
let i: Int = 1;
loop (99)
{
    if (i % 15 == 0)
    {
        print("FizzBuzz");
    }
    else if (i % 3 == 0)
    {
        print("Fizz");
    }
    else if (i % 5 == 0)
    {
        print("Buzz");
    }
    else
    {
        print("{}", i);
    }

    i = i + 1;
}
```

## Building

- Prerequisite
  - Latest Visual Studio 2022, with C++ platform toolset v143
  - LLVM 14.0.4

**Note: This project makes heavy use of C++23 features, and will not build with older versions of Visual Studio.**

**As of the current date (2/10/2023) intellisense is extremely broken on this project due to the use of c++ modules. This is not a problem with the project, but with Visual Studio itself, I personally use resharper++, it's broken aswell just more useable.**

## License

This project is licensed under the MIT license. See [LICENSE](LICENSE) for more details.

## Contributing

Contributions are welcome, but please open an issue first to discuss the change you wish to make.

## Acknowledgements

- [Tyler Laceby](https://www.youtube.com/@tylerlaceby) for his [Build a Custom Scripting](https://youtube.com/playlist?list=PL_2VhOvlMk4UHGqYCLWc6GO8FaPl8fQTh) series

- [Crafting Interpreters](https://craftinginterpreters.com/) for their fantastic book on writing interpreters

- [The LLVM Project](https://llvm.org/) for their C++ compiler framework

- [The Haxe Foundation](https://haxe.org/) for inspiration for the syntax
