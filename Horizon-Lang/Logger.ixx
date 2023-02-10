export module Logger;

import Types.Core;
import <chrono>;
import <stdio.h>;

export
{
    enum LogType
    {
        Debug,
        Error,
        Warning,
        Info,
        Success,
        None
    };

    namespace Logger
    {
        template <LogType type>
        consteval const char* TypeToString()
        {
            switch (type)
            {
            case Debug:
                return "[#]";
            case Error:
                return "[-]";
            case Warning:
                return "[!]";
            case Info:
                return "[=]";
            case Success:
                return "[+]";
            }

            return "";
        }
    }

    template <LogType type, bool logType = true, bool newLine = true, typename... Args>
    constexpr void Log(const char* format, Args... args)
    {
        if constexpr (logType)
        {
            printf("%s ", Logger::TypeToString<type>());
        }

        printf(format, args...);

        if constexpr (newLine)
        {
            printf("\n");
        }
    }

    template <LogType type, bool logType = true, bool newLine = true, typename... Args>
    constexpr void Log(String format, Args... args)
    {
        Log<type, logType, newLine>(format.c_str(), args...);
    }

    //@todo: move this to proper file
    struct Timer
    {
        String Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> Start;

        Timer(String name)
            : Name(name)
            , Start(std::chrono::steady_clock::now())
        {
        }

        ~Timer()
        {
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - Start).count();

            Log<Info>("%s took %f ms", Name.c_str(), duration / 1000.0f);
        }
    };
}

/* #define LogCurrentFile                                                                               \
    []()                                                                                             \
    {                                                                                                \
        constexpr auto current_file = std::source_location::current();                               \
        constexpr auto file_name = StringExtensions::LEraseTillLast(current_file.file_name(), '\\'); \
        printf("[%s(%i)] ", file_name, current_file.line());                                         \
    }

#define LogWithFileName(type, ...) \
    LogCurrentFile();              \
    Log<type>(__VA_ARGS__) */
