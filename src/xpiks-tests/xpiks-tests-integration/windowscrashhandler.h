#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <new.h>
#include <signal.h>
#include <exception>
#include <sys/stat.h>
#include <psapi.h>
#include <rtcapi.h>
#include <Shellapi.h>
#include <dbghelp.h>

class WindowsCrashHandler
{
public:
    WindowsCrashHandler();

public:
    // code from code project
    // Sets exception handlers that work on per-process basis
    void SetProcessExceptionHandlers();

    // Installs C++ exception handlers that function on per-thread basis
    void SetThreadExceptionHandlers();

    // Collects current process state.
    static void GetExceptionPointers(
        DWORD dwExceptionCode,
        EXCEPTION_POINTERS** pExceptionPointers);

    /* Exception handler functions. */

    static void __cdecl TerminateHandler();
    static void __cdecl UnexpectedHandler();

    static void __cdecl PureCallHandler();

    static void __cdecl InvalidParameterHandler(const wchar_t* expression,
        const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);

    static int __cdecl NewHandler(size_t);

    static void SigabrtHandler(int);
    static void SigfpeHandler(int /*code*/, int subcode);
    static void SigintHandler(int);
    static void SigillHandler(int);
    static void SigsegvHandler(int);
    static void SigtermHandler(int);
};

#endif // CRASHHANDLER_H
