#include "windowscrashhandler.h"

#include <tchar.h>
#include <stdio.h>
#include "StackWalker.h"

// minidump
#include <windows.h>
#include <TlHelp32.h>
//#include <tchar.h>
#include <dbghelp.h>
//#include <stdio.h>
#include <crtdbg.h>
#include <signal.h>

#include "../../xpiks-qt/Common/defines.h"
#include "../../xpiks-qt/Helpers/logger.h"


#if _MSC_VER < 1400
#define strcpy_s(dst, len, src) strcpy(dst, src)
#define strncpy_s(dst, len, src, maxLen) strncpy(dst, len, src)
#define strcat_s(dst, len, src) strcat(dst, src)
#define _snprintf_s _snprintf
#define _tcscat_s _tcscat
#endif

#define CR_SEH_EXCEPTION                0    //!< SEH exception.
#define CR_CPP_TERMINATE_CALL           1    //!< C++ terminate() call.
#define CR_CPP_UNEXPECTED_CALL          2    //!< C++ unexpected() call.
#define CR_CPP_PURE_CALL                3    //!< C++ pure virtual function call (VS .NET and later).
#define CR_CPP_NEW_OPERATOR_ERROR       4    //!< C++ new operator fault (VS .NET and later).
#define CR_CPP_SECURITY_ERROR           5    //!< Buffer overrun error (VS .NET only).
#define CR_CPP_INVALID_PARAMETER        6    //!< Invalid parameter exception (VS 2005 and later).
#define CR_CPP_SIGABRT                  7    //!< C++ SIGABRT signal (abort).
#define CR_CPP_SIGFPE                   8    //!< C++ SIGFPE signal (flotating point exception).
#define CR_CPP_SIGILL                   9    //!< C++ SIGILL signal (illegal instruction).
#define CR_CPP_SIGINT                   10   //!< C++ SIGINT signal (CTRL+C).
#define CR_CPP_SIGSEGV                  11   //!< C++ SIGSEGV signal (invalid storage access).
#define CR_CPP_SIGTERM                  12   //!< C++ SIGTERM signal (termination request).

// Specialized stackwalker-output classes
// Console (printf):
class StackWalkerToConsole : public StackWalker
{
protected:
  virtual void OnOutput(LPCSTR szText)
  {
    printf("%s", szText);
  }
};

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI
  MyDummySetUnhandledExceptionFilter(
  LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
  return NULL;
}

BOOL PreventSetUnhandledExceptionFilter()
{
    HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
    if (hKernel32 == NULL) return FALSE;
    void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
    if (pOrgEntry == NULL) return FALSE;

#ifdef _M_IX86
    // Code for x86:
    // 33 C0                xor         eax,eax
    // C2 04 00             ret         4
    unsigned char szExecute[] = { 0x33, 0xC0, 0xC2, 0x04, 0x00 };
#elif _M_X64
    // 33 C0                xor         eax,eax
    // C3                   ret
    unsigned char szExecute[] = { 0x33, 0xC0, 0xC3 };
#else
#error "The following code only works for x86 and x64!"
#endif

    SIZE_T bytesWritten = 0;
    BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
                                   pOrgEntry, szExecute, sizeof(szExecute), &bytesWritten);
    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// This function determines whether we need data sections of the given module
//

bool IsDataSectionNeeded( const WCHAR* pModuleName ) {
    // Check parameters

    if( pModuleName == 0 )
    {
        _ASSERTE( _T("Parameter is null.") );
        return false;
    }

    // Extract the module name

    WCHAR szFileName[_MAX_FNAME] = L"";

    _wsplitpath( pModuleName, NULL, NULL, szFileName, NULL );


    // Compare the name with the list of known names and decide

    // if contains "xpiks-tests-integration" in it's path
    if( wcsstr( pModuleName, _T(STRINGIZE(APPNAME)) ) != 0 )
    {
        return true;
    }
    else if( wcsicmp( szFileName, L"ntdll" ) == 0 )
    {
        return true;
    }
    else if( wcsstr( szFileName, _T("Qt5") ) != 0 )
    {
        return true;
    }

    // Complete

    return false;

}

BOOL CALLBACK MyMiniDumpCallback(
    PVOID                            pParam,
    const PMINIDUMP_CALLBACK_INPUT   pInput,
    PMINIDUMP_CALLBACK_OUTPUT        pOutput
)
{
    BOOL bRet = FALSE;

    // Check parameters

    if( pInput == 0 )
        return FALSE;

    if( pOutput == 0 )
        return FALSE;

    // Process the callbacks

    switch( pInput->CallbackType )
    {
        case IncludeModuleCallback:
        {
            // Include the module into the dump
            bRet = TRUE;
        }
        break;

        case IncludeThreadCallback:
        {
            // Include the thread into the dump
            bRet = TRUE;
        }
        break;

        case ModuleCallback:
        {
            // Are data sections available for this module ?

            if( pOutput->ModuleWriteFlags & ModuleWriteDataSeg )
            {
                // Yes, they are, but do we need them?

                if( !IsDataSectionNeeded( pInput->Module.FullPath ) )
                {
                    wprintf( L"Excluding module data sections: %s \n", pInput->Module.FullPath );

                    pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
                }
                else
                {
                    wprintf( L"Including %s module data sections \n", pInput->Module.FullPath );
                }
            }

            bRet = TRUE;
        }
        break;

        case ThreadCallback:
        {
            // Include all thread information into the minidump
            bRet = TRUE;
        }
        break;

        case ThreadExCallback:
        {
            // Include this information
            bRet = TRUE;
        }
        break;

        case MemoryCallback:
        {
            // We do not include any information here -> return FALSE
            bRet = FALSE;
        }
        break;

        case CancelCallback:
            break;
    }

    return bRet;

}

BOOL CreateMiniDump(EXCEPTION_POINTERS * pep)
{
    BOOL rv = FALSE;
    // Open the file

    HANDLE hFile = CreateFile( _T(STRINGIZE(APPNAME))_T(".dmp"), GENERIC_READ | GENERIC_WRITE,
                               0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

    if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) )
    {
        // Create the minidump

        MINIDUMP_EXCEPTION_INFORMATION mdei;

        mdei.ThreadId           = GetCurrentThreadId();
        mdei.ExceptionPointers  = pep;
        mdei.ClientPointers     = FALSE;

        MINIDUMP_CALLBACK_INFORMATION mci;

        mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback;
        mci.CallbackParam       = 0;

        MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory |
                                                  MiniDumpWithDataSegs |
                                                  MiniDumpWithHandleData |
                                                  MiniDumpWithFullMemoryInfo |
                                                  MiniDumpWithThreadInfo |
                                                  MiniDumpWithUnloadedModules );

        rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(),
                                     hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci );

        if( !rv )
            _tprintf( _T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError() );
        else
            _tprintf( _T("Minidump created.\n") );

        // Close the file

        CloseHandle( hFile );

    }
    else
    {
        _tprintf( _T("CreateFile failed. Error: %u \n"), GetLastError() );
    }

    fflush(stdout);
    fflush(stderr);

    return rv;
}

// The following code gets exception pointers using a workaround found in CRT code.
void GetExceptionPointers(DWORD dwExceptionCode,
                          EXCEPTION_POINTERS** ppExceptionPointers)
{
    // The following code was taken from VC++ 8.0 CRT (invarg.c: line 104)

    EXCEPTION_RECORD ExceptionRecord;
    CONTEXT ContextRecord;
    memset(&ContextRecord, 0, sizeof(CONTEXT));

#ifdef _X86_

    __asm {
        mov dword ptr [ContextRecord.Eax], eax
            mov dword ptr [ContextRecord.Ecx], ecx
            mov dword ptr [ContextRecord.Edx], edx
            mov dword ptr [ContextRecord.Ebx], ebx
            mov dword ptr [ContextRecord.Esi], esi
            mov dword ptr [ContextRecord.Edi], edi
            mov word ptr [ContextRecord.SegSs], ss
            mov word ptr [ContextRecord.SegCs], cs
            mov word ptr [ContextRecord.SegDs], ds
            mov word ptr [ContextRecord.SegEs], es
            mov word ptr [ContextRecord.SegFs], fs
            mov word ptr [ContextRecord.SegGs], gs
            pushfd
            pop [ContextRecord.EFlags]
    }

    ContextRecord.ContextFlags = CONTEXT_CONTROL;
#pragma warning(push)
#pragma warning(disable:4311)
    ContextRecord.Eip = (ULONG)_ReturnAddress();
    ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#pragma warning(pop)
    ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress()-1);


#elif defined (_IA64_) || defined (_AMD64_)

    /* Need to fill up the Context in IA64 and AMD64. */
    RtlCaptureContext(&ContextRecord);

#else  /* defined (_IA64_) || defined (_AMD64_) */

    ZeroMemory(&ContextRecord, sizeof(ContextRecord));

#endif  /* defined (_IA64_) || defined (_AMD64_) */

    ZeroMemory(&ExceptionRecord, sizeof(EXCEPTION_RECORD));

    ExceptionRecord.ExceptionCode = dwExceptionCode;
    ExceptionRecord.ExceptionAddress = _ReturnAddress();

    ///

    EXCEPTION_RECORD* pExceptionRecord = new EXCEPTION_RECORD;
    memcpy(pExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
    CONTEXT* pContextRecord = new CONTEXT;
    memcpy(pContextRecord, &ContextRecord, sizeof(CONTEXT));

    *ppExceptionPointers = new EXCEPTION_POINTERS;
    (*ppExceptionPointers)->ExceptionRecord = pExceptionRecord;
    (*ppExceptionPointers)->ContextRecord = pContextRecord;
}


void DoHandleCrash(EXCEPTION_POINTERS* pExPtrs)
{
    {
        Helpers::Logger &logger = Helpers::Logger::getInstance();
        logger.flush();
    }

    {
        CreateMiniDump(pExPtrs);
    }

    {
        StackWalkerToConsole sw;  // output to console
        sw.ShowCallstack(GetCurrentThread(), pExPtrs->ContextRecord);
        fflush(stdout);
    }
}

static BOOL s_bUnhandledExeptionFilterSet = FALSE;
static LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs)
{
#ifdef _M_IX86
  if (pExPtrs->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
  {
    static char MyStack[1024*128];  // be sure that we have enought space...
    // it assumes that DS and SS are the same!!! (this is the case for Win32)
    // change the stack only if the selectors are the same (this is the case for Win32)
    //__asm push offset MyStack[1024*128];
    //__asm pop esp;
  __asm mov eax,offset MyStack[1024*128];
  __asm mov esp,eax;
  }
#endif

  printf("UNHANDLED EXCEPTION FILTER\r\n");

  DoHandleCrash(pExPtrs);

  return EXCEPTION_CONTINUE_SEARCH;
}

static void InitUnhandledExceptionFilter()
{
  if (s_bUnhandledExeptionFilterSet == FALSE)
  {
    // set global exception handler (for handling all unhandled exceptions)
    SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);
#if defined _M_X64 || defined _M_IX86
    PreventSetUnhandledExceptionFilter();
#endif
    s_bUnhandledExeptionFilterSet = TRUE;
  }
}

void EnableCrashingOnCrashes()
{
    typedef BOOL (WINAPI *tGetPolicy)(LPDWORD lpFlags);
    typedef BOOL (WINAPI *tSetPolicy)(DWORD dwFlags);
    static const DWORD EXCEPTION_SWALLOWING = 0x1;

    const HMODULE kernel32 = LoadLibraryA("kernel32.dll");
    const tGetPolicy pGetPolicy = (tGetPolicy)GetProcAddress(kernel32, "GetProcessUserModeExceptionPolicy");
    const tSetPolicy pSetPolicy = (tSetPolicy)GetProcAddress(kernel32, "SetProcessUserModeExceptionPolicy");
    if(pGetPolicy && pSetPolicy)
    {
        DWORD dwFlags;
        if(pGetPolicy(&dwFlags))
        {
            // Turn off the filter
            pSetPolicy(dwFlags & ~EXCEPTION_SWALLOWING);
        }
    }
}

#ifndef _AddressOfReturnAddress
    // Taken from: http://msdn.microsoft.com/en-us/library/s975zw7k(VS.71).aspx
    #ifdef __cplusplus
        #define EXTERNC extern "C"
    #else
        #define EXTERNC
    #endif

    // _ReturnAddress and _AddressOfReturnAddress should be prototyped before use
    EXTERNC void * _AddressOfReturnAddress(void);
    EXTERNC void * _ReturnAddress(void);
#endif

WindowsCrashHandler::WindowsCrashHandler()
{
}

void WindowsCrashHandler::SetProcessExceptionHandlers()
{
    // SetErrorMode(SEM_FAILCRITICALERRORS);
    EnableCrashingOnCrashes();
    InitUnhandledExceptionFilter();

    // Catch pure virtual function calls.
    // Because there is one _purecall_handler for the whole process,
    // calling this function immediately impacts all threads. The last
    // caller on any thread sets the handler.
    // http://msdn.microsoft.com/en-us/library/t296ys27.aspx
    _set_purecall_handler(PureCallHandler);

    // Catch new operator memory allocation exceptions
    _set_new_handler(NewHandler);

    // Catch invalid parameter exceptions.
    _set_invalid_parameter_handler(InvalidParameterHandler);

    // Set up C++ signal handlers

    _set_abort_behavior(0, _WRITE_ABORT_MSG);
    _set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

    // Catch an abnormal program termination
    signal(SIGABRT, SigabrtHandler);

    // Catch illegal instruction handler
    signal(SIGINT, SigintHandler);

    // Catch a termination request
    signal(SIGTERM, SigtermHandler);

}

void WindowsCrashHandler::SetThreadExceptionHandlers()
{
    // Catch terminate() calls.
    // In a multithreaded environment, terminate functions are maintained
    // separately for each thread. Each new thread needs to install its own
    // terminate function. Thus, each thread is in charge of its own termination handling.
    // http://msdn.microsoft.com/en-us/library/t6fk7h29.aspx
    set_terminate(TerminateHandler);

    // Catch unexpected() calls.
    // In a multithreaded environment, unexpected functions are maintained
    // separately for each thread. Each new thread needs to install its own
    // unexpected function. Thus, each thread is in charge of its own unexpected handling.
    // http://msdn.microsoft.com/en-us/library/h46t5b69.aspx
    set_unexpected(UnexpectedHandler);

    // Catch a floating point error
    typedef void (*sigh)(int);
    signal(SIGFPE, (sigh)SigfpeHandler);

    // Catch an illegal instruction
    signal(SIGILL, SigillHandler);

    // Catch illegal storage access errors
    signal(SIGSEGV, SigsegvHandler);

}

// CRT terminate() call handler
void __cdecl WindowsCrashHandler::TerminateHandler()
{
    // Abnormal program termination (terminate() function was called)

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;
    if (pExceptionPtrs == nullptr) {
        GetExceptionPointers(CR_CPP_TERMINATE_CALL, &pExceptionPtrs);
    }

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT unexpected() call handler
void __cdecl WindowsCrashHandler::UnexpectedHandler()
{
    // Unexpected error (unexpected() function was called)

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;
    if (pExceptionPtrs == nullptr) {
        GetExceptionPointers(CR_CPP_UNEXPECTED_CALL, &pExceptionPtrs);
    }

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT Pure virtual method call handler
void __cdecl WindowsCrashHandler::PureCallHandler()
{
    // Pure virtual function call

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;
    if (pExceptionPtrs == nullptr) {
        GetExceptionPointers(CR_CPP_PURE_CALL, &pExceptionPtrs);
    }

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}


// CRT invalid parameter handler
void __cdecl WindowsCrashHandler::InvalidParameterHandler(
    const wchar_t* expression,
    const wchar_t* function,
    const wchar_t* file,
    unsigned int line,
    uintptr_t pReserved)
{
    pReserved;

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = NULL;
    GetExceptionPointers(CR_CPP_INVALID_PARAMETER, &pExceptionPtrs);

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT new operator fault handler
int __cdecl WindowsCrashHandler::NewHandler(size_t)
{
    // 'new' operator memory allocation exception

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = NULL;
    GetExceptionPointers(CR_CPP_NEW_OPERATOR_ERROR, &pExceptionPtrs);

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);

    // Unreacheable code
    return 0;
}

// CRT SIGABRT signal handler
void WindowsCrashHandler::SigabrtHandler(int)
{
    // Caught SIGABRT C++ signal

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;
    if (pExceptionPtrs == nullptr) {
        GetExceptionPointers(CR_CPP_SIGABRT, &pExceptionPtrs);
    }

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT SIGFPE signal handler
void WindowsCrashHandler::SigfpeHandler(int /*code*/, int subcode)
{
    // Floating point exception (SIGFPE)

    EXCEPTION_POINTERS* pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT sigill signal handler
void WindowsCrashHandler::SigillHandler(int)
{
    // Illegal instruction (SIGILL)

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;
    if (pExceptionPtrs == nullptr) {
        GetExceptionPointers(CR_CPP_SIGILL, &pExceptionPtrs);
    }

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT sigint signal handler
void WindowsCrashHandler::SigintHandler(int)
{
    // Interruption (SIGINT)

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;
    if (pExceptionPtrs == nullptr) {
        GetExceptionPointers(CR_CPP_SIGINT, &pExceptionPtrs);
    }

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT SIGSEGV signal handler
void WindowsCrashHandler::SigsegvHandler(int)
{
    // Invalid storage access (SIGSEGV)

    PEXCEPTION_POINTERS pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT SIGTERM signal handler
void WindowsCrashHandler::SigtermHandler(int)
{
    // Termination request (SIGTERM)

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;
    if (pExceptionPtrs == nullptr) {
        GetExceptionPointers(CR_CPP_SIGTERM, &pExceptionPtrs);
    }

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}


