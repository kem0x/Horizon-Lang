;
export module Safety;

import Types.Core;
import Logger;

#define _CRTDBG_MAP_ALLOC
import <stdlib.h>;
import <crtdbg.h>;
import <cstdio>;

// #define WIN32_LEAN_AND_MEAN
// import<windows.h>;

// import<DbgHelp.h>;
// #pragma comment(lib, "DbgHelp.lib")

export
{
    namespace Safety
    {
        /* static long GenerateCrashDump(EXCEPTION_POINTERS* e)
        {
            char Name[MAX_PATH] { 0 };
            auto NameEnd = Name + GetModuleFileNameA(GetModuleHandleA(nullptr), Name, MAX_PATH);

            SYSTEMTIME t;
            GetSystemTime(&t);
            wsprintfA(NameEnd - strlen(".exe"), "_%4d%02d%02d_%02d%02d%02d.dmp", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

            HANDLE hFile = CreateFileA(Name, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
            if (hFile == INVALID_HANDLE_VALUE)
                return EXCEPTION_CONTINUE_SEARCH;

            MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
            exceptionInfo.ThreadId = GetCurrentThreadId();
            exceptionInfo.ExceptionPointers = e;
            exceptionInfo.ClientPointers = FALSE;

            MiniDumpWriteDump(
                GetCurrentProcess(),
                GetCurrentProcessId(),
                hFile,
                MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory | MiniDumpWithFullMemory),
                e ? &exceptionInfo : NULL,
                NULL,
                NULL);

            if (hFile)
            {
                CloseHandle(hFile);
                hFile = NULL;
            }
            return EXCEPTION_CONTINUE_SEARCH;
        }*/

        void Init()
        {
            // Generate a memory dump on crash
            // SetUnhandledExceptionFilter(GenerateCrashDump);

            // Enable CRT memory leak detection
            _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
            _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
            _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
        }

        //@temp
        //@known issue: throwing with this triggers memory leaks detection!
        template <typename T = void>
        T Throw(const String& message)
        {
            Log<Error>("ERROR: %s", message.c_str());

            Log<Error>("Press any key to exit!");

            (void)getchar();

            __debugbreak();

            exit(0);

            return T();
        }
    }
}
