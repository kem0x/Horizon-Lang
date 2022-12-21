export module Logger;

import Types.Core;
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
        constexpr const char* TypeToString()
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
