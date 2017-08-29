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

void EnumerateThreads( DWORD (WINAPI *inCallback)( HANDLE ), DWORD inExceptThisOne )
{
    // Create a snapshot of all the threads in the process, and walk over
    // them, calling the callback function on each of them, except for
    // the thread identified by the inExceptThisOne parameter.
    HANDLE hSnapshot = ::CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
    if (INVALID_HANDLE_VALUE != hSnapshot) {
        THREADENTRY32 thread;
        thread.dwSize = sizeof( thread );
        if (::Thread32First( hSnapshot, &thread )) {
            do {
                if (thread.th32OwnerProcessID == ::GetCurrentProcessId() &&
                    thread.th32ThreadID != inExceptThisOne &&
                    thread.th32ThreadID != ::GetCurrentThreadId()) {
                    // We're making a big assumption that this call will only
                    // be used to suspend or resume a thread, and so we know
                    // that we only require the THREAD_SUSPEND_RESUME access right.
                    HANDLE hThread = ::OpenThread( THREAD_SUSPEND_RESUME, FALSE, thread.th32ThreadID );
                    if (hThread) {
                        inCallback( hThread );
                        ::CloseHandle( hThread );
                    }
                }
            } while (::Thread32Next( hSnapshot, &thread ) );
        }

        ::CloseHandle( hSnapshot );
    }
}

BOOL PreventSetUnhandledExceptionFilter()
{
  HMODULE hKernel32 = LoadLibrary(_T("kernel32.dll"));
  if (hKernel32 == NULL) return FALSE;
  void *pOrgEntry = GetProcAddress(hKernel32,
    "SetUnhandledExceptionFilter");
  if(pOrgEntry == NULL) return FALSE;

  DWORD dwOldProtect = 0;
  SIZE_T jmpSize = 5;
#ifdef _M_X64
  jmpSize = 13;
#endif
  BOOL bProt = VirtualProtect(pOrgEntry, jmpSize,
    PAGE_EXECUTE_READWRITE, &dwOldProtect);
  BYTE newJump[20];
  void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
#ifdef _M_IX86
  DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;
  dwOrgEntryAddr += jmpSize; // add 5 for 5 op-codes for jmp rel32
  DWORD dwNewEntryAddr = (DWORD) pNewFunc;
  DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;
  // JMP rel32: Jump near, relative, displacement relative to next instruction.
  newJump[0] = 0xE9;  // JMP rel32
  memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
#elif _M_X64
  newJump[0] = 0x49;  // MOV R15, ...
  newJump[1] = 0xBF;  // ...
  memcpy(&newJump[2], &pNewFunc, sizeof (pNewFunc));
  //pCur += sizeof (ULONG_PTR);
  newJump[10] = 0x41;  // JMP R15, ...
  newJump[11] = 0xFF;  // ...
  newJump[12] = 0xE7;  // ...
#endif
  SIZE_T bytesWritten;
  BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
    pOrgEntry, newJump, jmpSize, &bytesWritten);

  if (bProt != FALSE)
  {
    DWORD dwBuf;
    VirtualProtect(pOrgEntry, jmpSize, dwOldProtect, &dwBuf);
  }
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

BOOL DoWriteMiniDump(EXCEPTION_POINTERS * pep)
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

DWORD CALLBACK MiniDumpThreadCallback( LPVOID inParam )
{
    EXCEPTION_POINTERS* pep = (EXCEPTION_POINTERS *)inParam;
    DWORD result = 0;

    BOOL rv = DoWriteMiniDump(pep);
    if (rv) { result = 1; }

    return result;
}

bool CreateMiniDumpWithThread( EXCEPTION_POINTERS* pep )
{
    DWORD threadId = 0;
    HANDLE hThread = ::CreateThread( NULL, 0, MiniDumpThreadCallback, pep, CREATE_SUSPENDED, &threadId );

    if (hThread) {
        // Having created the thread successfully, we need to put all of the other
        // threads in the process into a suspended state, making sure not to suspend
        // our newly-created thread.  We do this because we want this function to
        // behave as a snapshot, and that means other threads should not continue
        // to perform work while we're creating the minidump.
        EnumerateThreads( ::SuspendThread, threadId );

        // Now we can resume our worker thread
        ::ResumeThread( hThread );

        // Wait for the thread to finish working, without allowing the current
        // thread to continue working.  This ensures that the current thread won't
        // do anything interesting while we're writing the debug information out.
        // This also means that the minidump will show this as the current callstack.
        ::WaitForSingleObject( hThread, INFINITE );

        // The thread exit code tells us whether we were able to create the minidump
        DWORD code = 0;
        ::GetExitCodeThread( hThread, &code );
        ::CloseHandle( hThread );

        // If we suspended other threads, now is the time to wake them up
        EnumerateThreads( ::ResumeThread, threadId );

        return code != 0;
    }

    return false;
}

void CreateMiniDump(EXCEPTION_POINTERS* pep)
{
    DoWriteMiniDump(pep);
}

void DoHandleCrash(EXCEPTION_POINTERS* pExPtrs)
{
    {
        CreateMiniDump(pExPtrs);
    }

    {
        StackWalkerToConsole sw;  // output to console
        sw.ShowCallstack(GetCurrentThread(), pExPtrs->ContextRecord);
        fflush(stdout);
    }

    {
        Helpers::Logger &logger = Helpers::Logger::getInstance();
        logger.flush();
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
    SetErrorMode(SEM_FAILCRITICALERRORS);
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

// The following code gets exception pointers using a workaround found in CRT code.
void WindowsCrashHandler::GetExceptionPointers(DWORD dwExceptionCode,
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

// CRT terminate() call handler
void __cdecl WindowsCrashHandler::TerminateHandler()
{
    // Abnormal program termination (terminate() function was called)

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = NULL;
    GetExceptionPointers(0, &pExceptionPtrs);

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT unexpected() call handler
void __cdecl WindowsCrashHandler::UnexpectedHandler()
{
    // Unexpected error (unexpected() function was called)

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = NULL;
    GetExceptionPointers(0, &pExceptionPtrs);

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT Pure virtual method call handler
void __cdecl WindowsCrashHandler::PureCallHandler()
{
    // Pure virtual function call

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = NULL;
    GetExceptionPointers(0, &pExceptionPtrs);

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
    GetExceptionPointers(0, &pExceptionPtrs);

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
    GetExceptionPointers(0, &pExceptionPtrs);

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
    EXCEPTION_POINTERS* pExceptionPtrs = NULL;
    GetExceptionPointers(0, &pExceptionPtrs);

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
    EXCEPTION_POINTERS* pExceptionPtrs = NULL;
    GetExceptionPointers(0, &pExceptionPtrs);

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}

// CRT sigint signal handler
void WindowsCrashHandler::SigintHandler(int)
{
    // Interruption (SIGINT)

    // Retrieve exception information
    EXCEPTION_POINTERS* pExceptionPtrs = NULL;
    GetExceptionPointers(0, &pExceptionPtrs);

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
    EXCEPTION_POINTERS* pExceptionPtrs = NULL;
    GetExceptionPointers(0, &pExceptionPtrs);

    DoHandleCrash(pExceptionPtrs);

    // Terminate process
    TerminateProcess(GetCurrentProcess(), 1);
}


