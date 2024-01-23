#pragma once

#include "Core.hpp"

#include <utility>

namespace Aura {

	template<typename T, typename TDeleter = DefaultDeleter<T>>
	class Unique
	{
	public:
		constexpr Unique() = default;

		constexpr Unique(T* instance) noexcept
			: m_Instance(instance) {}

		constexpr Unique(Unique&& other) noexcept
			: m_Instance(std::exchange(other.m_Instance, nullptr)) {}

		constexpr Unique& operator=(Unique&& other) noexcept
		{
			m_Instance = std::exchange(other.m_Instance, nullptr);
			return *this;
		}

		constexpr Unique& operator=(T* instance) noexcept
		{
			T* old = std::exchange(m_Instance, instance);

			if (old)
				TDeleter()(old);

			return *this;
		}

		template<typename U>
		requires std::convertible_to<U*, T*>
		constexpr Unique(Unique<U>&& other) noexcept
			: m_Instance(other.Release()) {}

		template<typename U>
		requires std::convertible_to<U*, T*>
		constexpr Unique& operator=(Unique<U>&& other) noexcept
		{
			m_Instance = std::exchange<T*, U*>(other.m_Instance, nullptr);
			return *this;
		}

		constexpr ~Unique()
		{
			if (m_Instance)
				TDeleter()(m_Instance);
		}

		Unique(const Unique&) = delete;
		Unique& operator=(const Unique&) = delete;

	public:
		constexpr T* Release() const
		{
			return std::exchange(m_Instance, nullptr);
		}

		operator T& () const & { return *m_Instance; }
		T* operator->() const { return m_Instance; }

	public:
		template<typename... Args>
		static Unique New(Args&&... args)
		{
			return Unique(new T(std::forward<Args>(args)...));
		}

	private:
		T* m_Instance = nullptr;
	};

}
