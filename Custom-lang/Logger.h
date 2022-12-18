#pragma once
#include <iostream>
#include <fstream>
#include <source_location>

#include "Types/Extensions/String.h"

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
    constexpr auto TypeToString() -> const char*
    {
        switch (type)
        {
        case LogType::Debug:
            return "[#]";
        case LogType::Error:
            return "[-]";
        case LogType::Warning:
            return "[!]";
        case LogType::Info:
            return "[=]";
        case LogType::Success:
            return "[+]";
        }

        return "";
    }
}

template <LogType type, bool logType = true, bool newLine = true, typename... Args>
constexpr auto Log(const char* format, Args... args) -> void
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
constexpr void Log(std::string format, Args... args)
{
    Log<type, logType, newLine>(format.c_str(), args...);
}

#define LogCurrentFile                                                                               \
    []()                                                                                             \
    {                                                                                                \
        constexpr auto current_file = std::source_location::current();                               \
        constexpr auto file_name = StringExtensions::LEraseTillLast(current_file.file_name(), '\\'); \
        printf("[%s(%i)] ", file_name, current_file.line());                                         \
    }

#define LogWithFileName(type, ...) \
    LogCurrentFile();              \
    Log<type>(__VA_ARGS__)