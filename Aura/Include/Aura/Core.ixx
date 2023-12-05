module;

#include <cstdint>
#include <utility>

export module Aura:Core;

export namespace Aura {

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

	template<typename T>
	struct HandleImpl;

	template<typename T>
	struct Handle
	{
		using Impl = HandleImpl<T>;

		Handle() = default;
		Handle(Impl* impl)
			: m_Impl(impl) {}

		T Unwrap() const noexcept { return T(m_Impl); }
		operator T() const noexcept { return Unwrap(); }

		operator bool() const noexcept { return m_Impl; }
		Impl* operator->() const noexcept { return m_Impl; }

	protected:
		Impl* m_Impl = nullptr;
	};

}

/*
#define AuraConcatInternal(A, B) A##B
#define AuraConcat(A, B) AuraConcatInternal(A, B)

#define AuraScopeExit(...) ::Aura::ScopeExit AuraConcat(scopeExit, __LINE__) = [__VA_ARGS__]
*/
