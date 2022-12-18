#pragma once
#include "Logger.h"

//@temporary
template <typename T = void>
static T Throw(std::string message)
{
    Log<Error>("ERROR: %s", message.c_str());

    Log<Error>("Press any key to exit!");

    (void)getchar();

    exit(0);

    return T();
}