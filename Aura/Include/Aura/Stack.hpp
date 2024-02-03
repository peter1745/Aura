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
		Span<T> AllocateAligned(uint32_t count, uint32_t align)
		{
			if (count == 0)
			{
				return Span<T>{nullptr, 0u};
			}

			uint32_t size = count * sizeof(T);
			std::byte* ptr = Head;
			Head = AlignUp2(Head + size, align);
			memset(ptr, 0, size);
			return Span<T>{ reinterpret_cast<T*>(ptr), count };
		}

		template<typename T>
		Span<T> Allocate(uint32_t count)
		{
			return AllocateAligned<T>(count, 16);
		}
	};

	inline thread_local ThreadStack LargeStack;

	template<typename T>
	Span<T> StackAlloc(uint32_t count)
	{
		return LargeStack.Allocate<T>(count);
	}

	template<typename T>
	Span<T> StackAllocAligned(uint32_t count, uint32_t align)
	{
		return LargeStack.AllocateAligned<T>(count, align);
	}
}

#define AuraStackPoint() AuraScopeExit(head = ::Aura::LargeStack.Head){ ::Aura::LargeStack.Head = head; }
