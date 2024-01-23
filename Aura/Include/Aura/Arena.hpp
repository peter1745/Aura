#pragma once

#include "Core.hpp"
#include "Unique.hpp"

#include <vector>
#include <ranges>

namespace Aura {

	class Arena;
	class ArenaPtr
	{
	public:
		ArenaPtr(Arena* arena)
			: m_Arena(arena) {}

		ArenaPtr(const ArenaPtr&) noexcept = default;
		ArenaPtr& operator=(const ArenaPtr&) noexcept = default;

		ArenaPtr(ArenaPtr&& other) noexcept
		{
			m_Arena = std::exchange(other.m_Arena, nullptr);
		}

		ArenaPtr& operator=(ArenaPtr&& other) noexcept;

		~ArenaPtr();

		Arena* operator->() { return m_Arena; }

	private:
		Arena* m_Arena;
	};

	class Arena
	{
	public:
		struct Block
		{
			std::byte* Begin;
			std::byte* Last;
			std::byte* End;

			template<typename T>
			T* As() const noexcept
			{
				if (Begin + sizeof(T) > End)
				{
					return nullptr;
				}

				return reinterpret_cast<T*>(Begin);
			}

			template<typename T>
			void Write(const T& value, uint64_t size = ~0u)
			{
				size = (std::min)(size, sizeof(T));
				AuraVerify(Last + size <= End);

				if constexpr (std::is_pointer_v<T>)
				{
					memcpy(Last, value, size);
				}
				else
				{
					memcpy(Last, &value, size);
				}

				Last += size;
			}
	
			template<typename T>
			T& Read(uint64_t offset = 0)
			{
				return *reinterpret_cast<T*>(Begin + offset);
			}

			template<typename T>
			const T& Read(uint64_t offset = 0) const
			{
				return *reinterpret_cast<const T*>(Begin + offset);
			}

			bool operator==(const Block&) const = default;

			template<typename T>
			static Block From(T* object)
			{
				auto* begin = reinterpret_cast<std::byte*>(object);
				return { begin, begin, begin + sizeof(T) };
			}
		};

		Arena() noexcept
			: m_Begin(nullptr), m_Last(nullptr), m_End(nullptr), m_Parent(this)
		{
		}

		Arena(uint64_t size) noexcept
			: m_Begin(new std::byte[size]), m_Last(m_Begin), m_End(m_Begin + size), m_Parent(this)
		{
			memset(m_Begin, 0, size);
		}

		Arena(Arena* parent, std::byte* begin, std::byte* end)
			: m_Begin(begin), m_Last(begin), m_End(end), m_Parent(parent)
		{
		}

		Arena(const Arena& other) noexcept = default;
		Arena& operator=(const Arena& other) noexcept = default;

		Arena(Arena&& other) noexcept
		{
			m_Begin = std::exchange(other.m_Begin, nullptr);
			m_Last = std::exchange(other.m_Last, nullptr);
			m_End = std::exchange(other.m_End, nullptr);
			m_FreeList = std::move(other.m_FreeList);
			m_Parent = std::exchange(other.m_Parent, nullptr);
		}

		Arena& operator=(Arena&& other) noexcept
		{
			if (m_Parent == this && m_Begin)
			{
				delete[] m_Begin;
			}

			m_Begin = std::exchange(other.m_Begin, nullptr);
			m_Last = std::exchange(other.m_Last, nullptr);
			m_End = std::exchange(other.m_End, nullptr);
			m_FreeList = std::move(other.m_FreeList);
			m_Parent = std::exchange(other.m_Parent, nullptr);

			return *this;
		}

		~Arena()
		{
			if (m_Parent == this)
			{
				delete[] m_Begin;
			}
			else
			{
				m_Parent->Free(Block{ reinterpret_cast<std::byte*>(this), nullptr, m_End });
			}
		}

		Block Allocate(uint64_t size)
		{
			auto* freeBlock = FindFreeBlock(size);

			if (freeBlock != nullptr)
			{
				m_FreeList.erase(std::ranges::find(m_FreeList, *freeBlock));
				freeBlock->Last = freeBlock->Begin;
				memset(freeBlock->Begin, 0, std::distance(freeBlock->Begin, freeBlock->End));
				return *freeBlock;
			}

			Block block = { m_Last, m_Last, m_Last + size };
			m_Last = AlignUp2(m_Last + size, 16);
			return block;
		}

		void Free(Block block)
		{
			m_FreeList.push_back(block);
		}

		ArenaPtr Split(uint64_t size)
		{
			// Allocate a block that can hold an Arena + the requested number of bytes
			auto block = Allocate(sizeof(Arena) + size);

			// Pointer to the start of sub-arenas allocation area
			std::byte* begin = block.Begin + sizeof(Arena);

			// Construct a sub-arena at the beginning of the block allocation
			return { std::construct_at(reinterpret_cast<Arena*>(block.Begin), this, begin, begin + size) };
		}

	private:
		Block* FindFreeBlock(uint64_t size)
		{
			if (m_FreeList.empty())
			{
				return nullptr;
			}

			auto blocks = m_FreeList | std::views::filter([size](const auto& arena)
			{
				return std::distance(arena.Begin, arena.End) >= size;
			});

			if (blocks.empty())
			{
				return nullptr;
			}

			auto& block = *std::ranges::min_element(blocks, [&](auto& arena, auto& arena1)
			{
				uint64_t diffA = std::distance(arena.Begin, arena.End) - size;
				uint64_t diffB = std::distance(arena1.Begin, arena1.End) - size;
				return diffA < diffB;
			});

			return &block;
		}

	private:
		std::byte* m_Begin;
		std::byte* m_Last;
		std::byte* m_End;

		Arena* m_Parent;

		std::vector<Block> m_FreeList;
	};

	inline ArenaPtr::~ArenaPtr()
	{
		if (m_Arena)
		{
			m_Arena->~Arena();
		}
	}

	inline ArenaPtr& ArenaPtr::operator=(ArenaPtr&& other) noexcept
	{
		if (m_Arena != other.m_Arena)
		{
			m_Arena->~Arena();
		}

		m_Arena = std::exchange(other.m_Arena, nullptr);
		return *this;
	}

}


