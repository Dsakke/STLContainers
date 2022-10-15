#pragma once
#include <type_traits>
#include <memory>
#include <cassert>

namespace Container
{
	template<typename type, typename allocator = std::allocator<type>>
	class Vector final
	{
	public:
#pragma region Deleted Functions
		Vector(const Vector&) = delete;
		Vector(Vector&&) = delete;
		Vector& operator=(const Vector&) = delete;
		Vector& operator=(Vector&&) = delete;
#pragma endregion
#pragma region De/Constructors
		Vector();
		Vector(uint32_t size, const type& type);
		Vector(uint32_t capacity);
		~Vector();
#pragma endregion
#pragma region Accessors
		_NODISCARD const type& At(uint32_t pos) const;
		_NODISCARD type& At(uint32_t pos);
		_NODISCARD const type& operator[](uint32_t pos) const;
		_NODISCARD type& operator[](uint32_t pos);
		_NODISCARD const type& Front() const;
		_NODISCARD type& Front();
		_NODISCARD const type& Back() const;
		_NODISCARD type& Back();
		_NODISCARD type* Data();
		_NODISCARD const type* Data() const;
#pragma endregion
#pragma region Iterators
		// TODO figure out how to make a custom iterator
#pragma endregion
#pragma region Capacity
		_NODISCARD bool Empty() const;
		_NODISCARD uint32_t Size() const;
		_NODISCARD constexpr uint32_t MaxElements() const;
		void Reserve(uint32_t newReserve);
		_NODISCARD uint32_t Capacity() const;
		void ShrinkToFit();
#pragma endregion
#pragma region Modifiers
		void Clear();
		// add insert
		// add emplace
		// add erase
		void PushBack(const type& type);
		void PushBack(type&& type);
		// add emplace back
		void PopBack();
		void Resize(uint32_t newSize);
		void Swap(Vector<type, allocator>& other);
#pragma endregion
	private:
		void Reallocate(uint32_t newCapacity);


		type* m_pData;
		uint32_t m_Size;
		uint32_t m_Capacity;
		allocator m_Allocator = allocator{};

		static const uint32_t m_DefaultSize = 4;
		static const uint32_t m_CapacityGrowth = 2;
	};

	template<typename type, typename allocator>
	inline Vector<type, allocator>::Vector()
		: m_pData{nullptr}
		, m_Size{0}
		, m_Capacity{m_DefaultSize}
	{
		m_pData = m_Allocator.allocate(m_DefaultSize);
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>::Vector(uint32_t size, const type& type)
		: m_pData{nullptr}
		, m_Size{size}
		, m_Capacity{size}
	{
		m_pData = m_Allocator.allocate(size);
		m_Capacity = size;
		m_Size = size;
		for (uint32_t i{}; i < size; ++i)
		{
			m_pData[i] = type;
		}
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>::Vector(uint32_t capacity)
		: m_pData { nullptr }
		, m_Size{ 0 }
		, m_Capacity{ capacity }
	{
		m_pData = m_pData = m_Allocator.allocate(capacity);
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>::~Vector()
	{
		for (uint32_t i{}; i < m_Size; ++i)
		{
			m_Allocator.destroy(m_pData + i);
		}
		m_Allocator.deallocate(m_pData, m_Capacity);
	}

	template<typename type, typename allocator>
	inline const type& Vector<type, allocator>::At(uint32_t pos) const
	{
		assert(m_Size > pos);
		return m_pData[pos];
	}
	template<typename type, typename allocator>
	inline type& Vector<type, allocator>::At(uint32_t pos)
	{
		assert(m_Size > pos);
		return m_pData[pos];
	}
	template<typename type, typename allocator>
	inline const type& Vector<type, allocator>::operator[](uint32_t pos) const
	{
		assert(m_Size > pos);
		return m_pData[pos];
	}

	template<typename type, typename allocator>
	inline type& Vector<type, allocator>::operator[](uint32_t pos)
	{
		assert(m_Size > pos);
		return m_pData[pos];
	}

	template<typename type, typename allocator>
	inline const type& Vector<type, allocator>::Front() const
	{
		assert(m_Size > 0);
		return m_pData[0];
	}

	template<typename type, typename allocator>
	inline type& Vector<type, allocator>::Front()
	{
		assert(m_Size > 0);
		return m_pData[0];
	}

	template<typename type, typename allocator>
	inline const type& Vector<type, allocator>::Back() const
	{
		assert(m_Size > 0);
		return m_pData[m_Size - 1];
	}

	template<typename type, typename allocator>
	inline type& Vector<type, allocator>::Back()
	{
		assert(m_Size > 0);
		return m_pData[m_Size - 1];
	}

	template<typename type, typename allocator>
	inline type* Vector<type, allocator>::Data()
	{
		return m_pData;
	}

	template<typename type, typename allocator>
	inline const type* Vector<type, allocator>::Data() const
	{
		return m_pData;
	}

	template<typename type, typename allocator>
	inline bool Vector<type, allocator>::Empty() const
	{
		return m_Size > 0;
	}

	template<typename type, typename allocator>
	inline uint32_t Vector<type, allocator>::Size() const
	{
		return m_Size;
	}

	template<typename type, typename allocator>
	inline constexpr uint32_t Vector<type, allocator>::MaxElements() const
	{
		return UINT32_MAX;
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::Reserve(uint32_t newCapacity)
	{
		if (m_Capacity > newCapacity)
		{
			return;
		}
		Reallocate(newCapacity);
	}

	template<typename type, typename allocator>
	inline uint32_t Vector<type, allocator>::Capacity() const
	{
		return m_Capacity;
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::ShrinkToFit()
	{
		Reallocate(m_Size);
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::Clear()
	{
		for (uint32_t i{}; i < m_Size; ++i)
		{
			m_Allocator.destroy(m_pData + i);
		}
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::PushBack(const type& type)
	{
		if (m_Size == m_Capacity)
		{
			Reserve(m_Capacity * 2);
		}

		m_pData[m_Size] = type;
		++m_Size;
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::PushBack(type&& type)
	{
		if (m_Size == m_Capacity)
		{
			Reallocate(m_Capacity * m_CapacityGrowth);
		}

		m_pData[m_Size] = std::move(type);
		++m_Size;
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::PopBack()
	{
		m_Allocator.destroy(&Back());
		--m_Size;
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::Resize(uint32_t newSize)
	{
		if (newSize < m_Size)
		{
			for (int i{ newSize }; i < m_Size; ++i)
			{
				m_Allocator.destroy(m_pData);
			}
		}

		if (newSize > m_Capacity)
		{
			Reallocate(m_Capacity * m_CapacityGrowth);
		}
		
		m_Size = newSize;
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::Swap(Vector<type, allocator>& other)
	{
		m_Capacity = m_Capacity ^ other.m_Capacity;
		other.m_Capacity = m_Capacity ^ other.m_Capacity;
		m_Capacity = m_Capacity ^ other.m_Capacity;

		m_Size = m_Size ^ other.m_Size;
		other.m_Size = m_Size ^ other.m_Size;
		m_Size = m_Size ^ other.m_Size;

		type* pData = m_pData;
		m_pData = other.m_pData;
		other.m_pData = pData;
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::Reallocate(uint32_t newCapacity)
	{
		type* pOldData = m_pData;
		m_pData = m_Allocator.allocate(newCapacity);
		memcpy(m_pData, pOldData, m_Size * sizeof(type));
		m_Allocator.deallocate(pOldData, m_Capacity);
		m_Capacity = newCapacity;
	}
}