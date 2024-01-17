module;

#include <memory>

export module Aura:Stack;

import :Core;
import :Span;

inline static constexpr size_t LargeStackSize = 4 * 1024 * 1024;

export namespace Aura {

	struct ThreadStack
	{
		std::unique_ptr<std::byte[]> Memory = std::make_unique<std::byte[]>(LargeStackSize);
		std::byte* Head = Memory.get();
		std::byte* End = Memory.get() + LargeStackSize;

		template<typename T>
		Span<T> AllocateAligned(uint64_t count, uint64_t align)
		{
			uint64_t size = count * sizeof(T);
			//AuraVerify(std::distance(Head + size, End) > 0, "Stack overflow!");
			std::byte* ptr = Head;
			Head = AlignUp2(Head + size, align);
			return Span<T>{ reinterpret_cast<T*>(ptr), count };
		}

		template<typename T>
		Span<T> Allocate(uint64_t count)
		{
			return AllocateAligned<T>(count, 16);
		}
	};

	inline thread_local ThreadStack LargeStack;

	/*
	#define AuraStackPoint() AuraScopeExit(head = ::Aura::LargeStack.Head) { ::Aura::LargeStack.Head = head; }
#else

#define AuraStackPoint()

#endif
*/

}

/*#if defined(AURA_USE_LARGE_STACK)
	#define AuraStackAlloc(Type, Count) ::Aura::LargeStack.Allocate<Type>(Count)
#else
	#define AuraStackAllocImpl(Type, Count) static_cast<Type*>(AuraAlloca(sizeof(Type) * Count))
	#define AuraStackAlloc(Type, Count) ::Aura::Span{ AuraStackAllocImpl(Type, Count), Count }
#endif*/
