#pragma once

#include <cstdint>
#include <utility>

namespace Aura {

	template<typename T>
	constexpr T AlignUp2(T value, uint64_t align)
	{
		return reinterpret_cast<T>((reinterpret_cast<uint64_t>(value) + align - 1) & ~(align - 1));
	}

	template<typename T>
	struct ScopeExit
	{
		T Func;

		ScopeExit(T&& func) noexcept
			: Func(std::move(func))
		{
		}

		ScopeExit(const ScopeExit&) = delete;
		ScopeExit& operator=(const ScopeExit&) = delete;
		ScopeExit(ScopeExit&&) noexcept = delete;
		ScopeExit& operator=(ScopeExit&&) noexcept = delete;

		~ScopeExit()
		{
			Func();
		}
	};

	template<typename T>
	struct DefaultDeleter
	{
		void operator()(T* instance) const
		{
			delete instance;
		}
	};

	using SizeType = uint32_t;
}

#define AuraConcatInternal(A, B) A##B
#define AuraConcat(A, B) AuraConcatInternal(A, B)

#if defined(_MSC_VER)
	#define AuraDebugBreak __debugbreak()
	#define AuraNoUniqueAddress [[msvc::no_unique_address]]
#else
	#define AuraNoUniqueAddress [[no_unique_address]]
#endif

#define AuraVerify(Expr)    \
do {                        \
	if (!(Expr))            \
	{                       \
		AuraDebugBreak;     \
	}                       \
} while (false)


#define AuraScopeExit(...) ::Aura::ScopeExit AuraConcat(scopeExit, __LINE__) = [__VA_ARGS__]
