#pragma once

#include <stdint.h>
#include <utility>

#include "Verify.hpp"

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

	template<typename From, typename To>
	requires std::convertible_to<From, To>
	constexpr To* PointerCast(From* from)
	{
		return static_cast<To*>(from);
	}

}

#define AuraConcatInternal(A, B) A##B
#define AuraConcat(A, B) AuraConcatInternal(A, B)

#define AuraScopeExit(...) ::Aura::ScopeExit AuraConcat(scopeExit, __LINE__) = [__VA_ARGS__]
