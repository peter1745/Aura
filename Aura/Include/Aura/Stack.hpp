#pragma once

#include "Core.hpp"
#include "Span.hpp"

#include <cstddef>
#include <memory>

namespace Aura {

	inline constexpr size_t LargeStackSize = 4 * 1024 * 1024;

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

	template<typename T>
	Span<T> StackAlloc(uint32_t count)
	{
		return LargeStack.Allocate<T>(count);
	}

	template<typename T>
	Span<T> StackAllocAligned(uint64_t count, uint64_t align)
	{
		return LargeStack.AllocateAligned<T>(count, align);
	}

	inline thread_local ThreadStack LargeStack;
}

#define AuraStackPoint() AuraScopeExit(head = ::Aura::LargeStack.Head){ ::Aura::LargeStack.Head = head; }
