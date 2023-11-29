#pragma once

#if defined(AURA_USE_ASSERTS)

	#if defined(_WIN32)
		#define AuraDebugBreak __debugbreak()
	#else
		#if __has_include(<signal.h>)
			#include <signal.h>
			#define AuraDebugBreak raise(SIGTRAP)
		#endif
	#endif

	#define AuraVerify(Expr, ...)\
		if (!(Expr)) [[unlikely]] \
		{\
			AuraDebugBreak;\
		}

#else

#define AuraDebugBreak
#define AuraVerify(Expr, ...)

#endif
