#pragma once

#include <memory>

#if defined(_WIN32)
	#include <malloc.h>
	#define AuraAlloca _alloca
#endif

namespace Aura {

#if 0
	static constexpr uint32_t AllocaMarker = 0xAAAA;
	static constexpr uint32_t MallocMarker = 0xBBBB;
	static constexpr uint32_t MarkerSize = 16;
	static constexpr uint32_t AllocaThreshold = 1024;

	#if defined(AURA_SAFE_ALLOCA)

		template<typename T>
		inline T* Malloca(size_t count)
		{
			std::byte* ptr = nullptr;
			uint32_t marker = ~0U;
			size_t size = MarkerSize + (count * sizeof(T));

			if (size <= AllocaThreshold)
			{
				ptr = static_cast<std::byte*>(alloca(size));
				marker = AllocaMarker;
			}
			else
			{
				ptr = static_cast<std::byte*>(malloc(size));
				marker = MallocMarker;
			}

			// TODO(Peter): Assert that ptr is valid

			if (ptr == nullptr || marker == ~0U)
				return nullptr;

			*reinterpret_cast<uint32_t*>(ptr) = marker;
			ptr += MarkerSize;

			return reinterpret_cast<T*>(ptr);
		}

		template<typename T>
		inline void FreeA(T* ptr)
		{
			if (!ptr)
			{
				return;
			}

			ptr = reinterpret_cast<std::byte*>(ptr) - MarkerSize;

			uint32_t marker = *reinterpret_cast<uint32_t*>(ptr);

			if (marker != MallocMarker)
			{
				return;
			}

			free(ptr);
		}

	#else

		template<typename T>
		inline T* Malloca(size_t count)
		{
			size_t size = count * sizeof(T);

			if (size > AllocaThreshold)
			{
				// TODO(Peter): Assert
				return nullptr;
			}

			auto* ptr = static_cast<T*>(AuraAlloca(size));

			if (ptr == nullptr)
			{
				// TODO(Peter): Assert
				return nullptr;
			}

			return ptr;
		}

		template<typename T>
		inline void FreeA(T* ptr) {}
	#endif
#endif
}
