// ILoveToCrash.cpp : Defines the exported functions for the DLL application.
//

#define WIN32_LEAN_AND_MEAN

#include "Header.h"

#include <cstdio>
#include <stdlib.h>
#include <windows.h>
#include <processthreadsapi.h>
#include <csignal>
#include <windows.h>


void AccessViolation()
{
	char* c = nullptr;
	*c = 0;
}

void StackOverflow()
{
	StackOverflow();
}


LPTOP_LEVEL_EXCEPTION_FILTER old = nullptr;

long WINAPI lUnhandledExceptionFilter(_EXCEPTION_POINTERS *ep)
{
	if (!ep || !ep->ExceptionRecord) {
		return EXCEPTION_CONTINUE_SEARCH;
	}

	printf("Olá do nativo\n");

	const char * ret;
	switch (ep->ExceptionRecord->ExceptionCode) {
	case EXCEPTION_ACCESS_VIOLATION:
		ret = "The thread tried to read from or write to a virtual address for which it does not have the appropriate access.";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		ret = "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.";
		break;
	case EXCEPTION_BREAKPOINT:
		ret = "A breakpoint was encountered.";
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		ret = "The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		ret = "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		ret = "The thread tried to divide a floating-point value by a floating-point divisor of zero.";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		ret = "The result of a floating-point operation cannot be represented exactly as a decimal fraction.";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		ret = "This exception represents any floating-point exception not included in this list.";
		break;
	case EXCEPTION_FLT_OVERFLOW:
		ret = "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		ret = "The stack overflowed or underflowed as the result of a floating-point operation.";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		ret = "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		ret = "The thread tried to execute an invalid instruction.";
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		ret = "The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		ret = "The thread tried to divide an integer value by an integer divisor of zero.";
		break;
	case EXCEPTION_INT_OVERFLOW:
		ret = "The result of an integer operation caused a carry out of the most significant bit of the result.";
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		ret = "An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		ret = "The thread tried to continue execution after a noncontinuable exception occurred.";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		ret = "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.";
		break;
	case EXCEPTION_SINGLE_STEP:
		ret = "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.";
		break;
	case EXCEPTION_STACK_OVERFLOW:
		ret = "The thread used up its stack.";
		break;
	case EXCEPTION_GUARD_PAGE:
		ret = "The thread has received an EXCEPTION_GUARD_PAGE signal.";
		break;
	case EXCEPTION_INVALID_HANDLE:
		ret = "The thread has received an EXCEPTION_INVALID_HANDLE signal.";
		break;
		/*
		case EXCEPTION_POSSIBLE_DEADLOCK:
		ret = "The thread has received an EXCEPTION_POSSIBLE_DEADLOCK signal.";
		break;
		*/
	case CONTROL_C_EXIT:
		ret = "The thread has received a CONTROL_C_EXIT signal.";
		break;
	default:
		ret = "The thread has received an unknown signal.";
		printf("%H\n", ep->ExceptionRecord->ExceptionCode);
		break;
	}

	printf(ret);

	return !old ? EXCEPTION_CONTINUE_SEARCH : old(ep);
}

void __cdecl lHandlePureVirtualFunctionCall()
{
	printf("lHandlePureVirtualFunctionCall");
	// Execute a hardcoded breakpoint instruction in order to crash the process, or break into the debugger if one is attached.
	// This avoids hanging the process with an unnoticed dialog (important for server applications) and it ensures that a crash dump will be created.
	__debugbreak();
	TerminateProcess(GetCurrentProcess(), 10);
}

void __cdecl lHandleInvalidParameter(const wchar_t *, const wchar_t *, const wchar_t *, unsigned int, uintptr_t)
{
	printf("lHandleInvalidParameter");
	__debugbreak();
	TerminateProcess(GetCurrentProcess(), 10);
}

void __cdecl lAbortHandler(int)
{
	printf("lAbortHandler");
	__debugbreak();
	TerminateProcess(GetCurrentProcess(), 10);
}

void Setup()
{
	auto a = old = SetUnhandledExceptionFilter(lUnhandledExceptionFilter);
	auto b = _set_purecall_handler(lHandlePureVirtualFunctionCall);
	auto c = _set_invalid_parameter_handler(lHandleInvalidParameter);
	auto d = _set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);
	auto e = signal(SIGABRT, &lAbortHandler);

	SetErrorMode
	(
		SEM_FAILCRITICALERRORS	//Do not display critical-error-handler message box
		|
		SEM_NOGPFAULTERRORBOX	//Do not display Windows Error Reporting dialog
	);

	printf("%d %d %d %d %d\n", old, b, c, d, e);
}
