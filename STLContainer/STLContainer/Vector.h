#pragma once
#include <type_traits>
#include <memory>
#include <cassert>

namespace Container
{
#pragma region Iterator Classes
	template<class type>
	class ConstIterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		virtual ~ConstIterator() = default;
		ConstIterator(type* pVal);

		_NODISCARD const type& operator*() const;
		_NODISCARD const type& operator->() const;
		ConstIterator& operator++();
		ConstIterator operator++(int);
		ConstIterator& operator--();
		ConstIterator operator--(int);
		ConstIterator& operator+=(int32_t rhs);
		ConstIterator& operator-=(int32_t rhs);
		ConstIterator operator+(int32_t rhs);
		ConstIterator operator-(int32_t rhs);
		bool operator==(const ConstIterator& rhs) const;
		bool operator!=(const ConstIterator& rhs) const;

		type* m_pValue;
	};


	template<class type>
	class Iterator final : public ConstIterator<type>
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		Iterator() = delete;
		Iterator(type* start);

		_NODISCARD type& operator*();
		_NODISCARD type& operator->();
		Iterator& operator++();
		Iterator operator++(int);
		Iterator& operator--();
		Iterator operator--(int);
		Iterator& operator+=(int32_t rhs);
		Iterator& operator-=(int32_t rhs);
		Iterator operator+(int32_t rhs);
		Iterator operator-(int32_t rhs);
		bool operator==(const Iterator& rhs) const;
		bool operator!=(const Iterator& rhs) const;
	private:

	};

#pragma endregion

	template<typename type, typename allocator = std::allocator<type>>
	class Vector final
	{
	public:
#pragma region member types
		using iterator = Iterator<type>;
		using const_iterator = ConstIterator<type>;
#pragma endregion
#pragma region Type Requirments
		static_assert(std::is_copy_assignable<type>::value);
		static_assert(std::is_copy_constructible<type>::value);
#pragma endregion
#pragma region Deleted Functions
#pragma endregion
#pragma region Iterators
#pragma region Iterator Functions
		_NODISCARD iterator Begin();
		_NODISCARD iterator End();
		_NODISCARD const_iterator CBegin() const;
		_NODISCARD const_iterator CEnd() const;
#pragma endregion
#pragma endregion
#pragma region De/Constructors
		Vector();
		Vector(uint32_t size, const type& type);
		Vector(uint32_t capacity);
		Vector(const Vector& other);
		Vector(Vector&& other);
		Vector& operator=(const Vector& other);
		Vector& operator=(Vector&& other);
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
		iterator Insert(const_iterator pos, const type& value);
		iterator Insert(const_iterator pos, type&& value);
		iterator Insert(const_iterator pos, uint32_t count, const type& value);
		template<class inIt>
		iterator Insert(const_iterator pos, inIt first, inIt last);
		template<class... ARGS>
		iterator Emplace(const_iterator pos, ARGS&&... arsgs);
		iterator Erase(const_iterator pos);
		iterator Erase(const_iterator first, const_iterator last);
		void PushBack(const type& type);
		void PushBack(type&& type);
		template<class... ARGS>
		void EmplaceBack(ARGS&&... args);
		void PopBack();
		void Resize(uint32_t newSize);
		void Swap(Vector& other);
#pragma endregion

	private:
		void Reallocate(uint32_t newCapacity);


		type* m_pData;
		uint32_t m_Size;
		uint32_t m_Capacity;
		allocator m_Allocator = allocator{};

		static const uint32_t m_DefaultSize = 4;
		static const uint32_t m_CapacityGrowth = 2;

		public:

	};

	template<typename type>
	inline Iterator<type>::Iterator(type* start)
		: ConstIterator<type>{ start }
	{
	}

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
		m_pData = m_Allocator.allocate(capacity);
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>::Vector(const Vector& other)
		: m_pData{nullptr}
		, m_Size{other.m_Size}
		, m_Capacity{other.m_Capacity}
		, m_Allocator{}
	{
		m_pData = m_Allocator.allocate(m_Capacity);
		for (uint32_t i = 0; i < m_Size; ++i)
		{
			m_pData[i] = other.m_pData[i];
		}
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>::Vector(Vector&& other)
		: m_pData {other.m_pData}
		, m_Size{other.m_Size}
		, m_Capacity{other.m_Capacity}
		, m_Allocator{static_cast<allocator&&>(other.m_Allocator)}
	{
		other.m_pData = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>& Vector<type, allocator>::operator=(const Vector& other)
	{
		Clear();

		m_Size = other.m_Size;
		m_Capacity = other.m_Capacity;
		m_Allocator = allocator{};
		m_pData = m_Allocator.allocate(m_Capacity);
		memcpy(m_pData, other.m_pData, m_Size * sizeof(type));
		return *this;
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>& Vector<type, allocator>::operator=(Vector&& other)
	{
		Clear();

		m_Size = other.m_Size;
		other.m_Size = 0;
		m_Capacity = other.m_Capacity;
		other.m_Capacity = 0;
		m_pData = other.m_pData;
		other.m_pData = nullptr;
		m_Allocator = static_cast<allocator&&>(other.m_Allocator);
		return *this;
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>::~Vector()
	{
		Clear();
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
		return m_pData[pos];
	}

	template<typename type, typename allocator>
	inline type& Vector<type, allocator>::operator[](uint32_t pos)
	{
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
	inline typename Vector<type, allocator>::iterator Vector<type, allocator>::Begin()
	{
		return Vector<type, allocator>::iterator{m_pData};
	}

	template<typename type, typename allocator>
	inline typename Vector<type, allocator>::iterator Vector<type, allocator>::End()
	{
		return Vector<type, allocator>::iterator{m_pData + m_Size};
	}

	template<typename type, typename allocator>
	inline typename Vector<type, allocator>::const_iterator Vector<type, allocator>::CBegin() const
	{
		return const_iterator(m_pData);
	}

	template<typename type, typename allocator>
	inline typename Vector<type, allocator>::const_iterator Vector<type, allocator>::CEnd() const
	{
		return const_iterator(m_pData + m_Size);
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
		if constexpr (!std::is_trivially_destructible<type>::value)
		{
			for (uint32_t i{}; i < m_Size; ++i)
			{
				m_pData[i].~type();
			}
		}
		m_Size = 0;
	}

	template<typename type, typename allocator>
	inline typename Vector<type, allocator>::iterator Vector<type, allocator>::Insert(const_iterator pos, const type& value)
	{
		return Emplace(pos, value);
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>::iterator Vector<type, allocator>::Insert(const_iterator pos, type&& value)
	{
		return Emplace(pos, value);
	}

	template<typename type, typename allocator>
	template<class ...ARGS>
	inline void Vector<type, allocator>::EmplaceBack(ARGS && ...args)
	{
		Emplace(CEnd(), args...);
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>::iterator Vector<type, allocator>::Insert(const_iterator pos, uint32_t count, const type& value)
	{
		if (count == 0)
		{
			return pos;
		}

		type* location = pos.m_pValue;
		assert(location >= m_pData && location <= m_pData + m_Size);
		uint32_t distanceToStart = location - m_pData;
		uint32_t distanceToEnd = m_Size - distanceToStart;
		m_Size += count;

		if (m_Size > m_Capacity)
		{
			Reserve(m_Size + count);
		}

		std::move(m_pData + distanceToStart + count, m_pData + distanceToStart, distanceToEnd * sizeof(type));

		for (int i{}; i < count; ++i)
		{
			m_pData[distanceToStart + i] = value;
		}

		return iterator(m_pData + distanceToStart);
	}

	template<typename type, typename allocator>
	template<class inIt>
	inline Vector<type, allocator>::iterator
 Vector<type, allocator>::Insert(const_iterator pos, inIt first, inIt last)
	{
		if (first == last)
		{
			return pos;
		}

		type* location = pos.m_pValue;
		assert(location >= m_pData && location <= m_pData + m_Size);
		uint32_t distanceToStart = location - m_pData;
		uint32_t distanceToEnd = m_Size - distanceToStart;
		auto distance = std::distance(first, last);
		m_Size += distance;

		if (m_Size > m_Capacity)
		{
			Reserve(m_Size + distance);
		}

		std::memmove(m_pData + distanceToStart + distance, m_pData + distanceToStart, distanceToEnd * sizeof(type));

		for (uint32_t i = 0; i < distance; ++i)
		{
			m_pData[distanceToStart + i] = first;
			++first;
		}

		return iterator(m_pData + distanceToStart);
	}

	template<typename type, typename allocator>
	template<class... ARGS>
	inline typename Vector<type, allocator>::iterator Vector<type, allocator>::Emplace(const_iterator pos, ARGS&&... args)
	{
		type* location = pos.m_pValue;
		assert(location >= m_pData && location <= m_pData + m_Size);
		uint32_t distanceToStart = location - m_pData;
		uint32_t distanceToEnd = m_Size - distanceToStart;
		++m_Size;
		if (m_Size == m_Capacity)
		{
			Reallocate(m_Capacity * m_CapacityGrowth); // this invalidates the iterator, this is why we use distances instead of the actual allocator to emplace
		}

		std::memmove(m_pData + distanceToStart + 1, m_pData + distanceToStart, distanceToEnd * sizeof(type)); // memmove because the src and dest will overlap
		m_pData[distanceToStart] = type(args...);
		return iterator(m_pData + distanceToStart);
	}

	template<typename type, typename allocator>
	inline Vector<type, allocator>::iterator Vector<type, allocator>::Erase(const_iterator pos)
	{
		type* location = pos.m_pValue;

		assert(location >= m_pData && location <= m_pData + m_Size);
		uint32_t distanceToStart = location - m_pData;
		uint32_t distanceToEnd = m_Size - distanceToStart;
		if constexpr (!std::is_trivially_destructible<type>::value)
		{
			location->~type();
		}

		std::memmove(m_pData + distanceToStart, m_pData + distanceToStart + 1, distanceToEnd * sizeof(type));
		--m_Size;

		return iterator(pos.m_pValue);
	}

	template<typename type, typename allocator>
	Vector<type, allocator>::iterator Vector<type, allocator>::Erase(const_iterator first, const_iterator last)
	{
		type* firstLoc = first.m_pValue;
		type* lastLoc = last.m_pValue;

		uint32_t distanceToFirst = firstLoc - m_pData;
		uint32_t distanceToLast = lastLoc - m_pData;
		uint32_t eraseCount = distanceToLast - distanceToFirst;
		uint32_t sizeToMove = (m_Size - distanceToLast) * sizeof(type);

		if constexpr (!std::is_trivially_destructible<type>::value)
		{
			for (const_iterator it{first}; it != last; ++it)
			{
				it->~type();
			}
		}

		m_Size -= eraseCount;
		// memmove because locations might overlap
		std::memmove(firstLoc, lastLoc, sizeToMove);
		return iterator{ firstLoc };
	}


	template<typename type, typename allocator>
	inline void Vector<type, allocator>::PushBack(const type& type)
	{
		if (m_Size == m_Capacity)
		{
			Reallocate(m_Capacity * m_CapacityGrowth);
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
		if constexpr (!std::is_trivially_destructible<type>::value)
		{
			Back().~type();
		}

		--m_Size;
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::Resize(uint32_t newSize)
	{
		static_assert(std::is_default_constructible<type>::value, "type needs to be default constructable");
		if constexpr (!std::is_trivially_destructible<type>::value)
		{
			if (newSize < m_Size)
			{
				for (size_t i{ newSize }; i < m_Size; ++i)
				{
					m_pData[i].~type();
				}
			}
		}

		if (newSize > m_Capacity)
		{
			uint32_t oldCap = m_Capacity;
			Reallocate(newSize);
			for (uint32_t i{ m_Size }; i < m_Capacity; ++i)
			{
				m_pData[i] = type{};
			}
		}
		
		m_Size = newSize;
	}

	template<typename type, typename allocator>
	inline void Vector<type, allocator>::Swap(Vector& other)
	{
		std::swap(m_Capacity, other.m_Capacity);
		std::swap(m_Size, other.m_Size);
		std::swap(m_pData, other.m_pData);
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

	template<typename type>
	inline type& Iterator<type>::operator*()
	{
		return *(this->m_pValue);
	}

	template<typename type>
	inline type& Iterator<type>::operator->()
	{
		return *(this->m_pValue);
	}

	template<typename type>
	inline Iterator<type> Iterator<type>::operator+(int32_t rhs)
	{
		return Iterator(this->m_pValue + rhs);
	}

	template<typename type>
	inline  Iterator<type> Iterator<type>::operator-(int32_t rhs)
	{
		return Iterator(this->m_pValue - rhs);
	}


	template<typename type>
	inline  Iterator<type>& Iterator<type>::operator++()
	{
		++(this->m_pValue);
		return *this;
	}

	template<typename type>
	inline  Iterator<type> Iterator<type>::operator++(int)
	{
		Iterator temp = *this;
		++(this->m_pValue);
		return temp;
	}

	template<typename type>
	inline  Iterator<type>& Iterator<type>::operator--()
	{
		--(this->m_pValue);
		return *this;
	}

	template<typename type>
	inline  Iterator<type> Iterator<type>::operator--(int)
	{
		Iterator temp = *this;
		--(this->m_pValue);
		return temp;
	}

	template<typename type>
	inline  Iterator<type>& Iterator<type>::operator+=(int32_t rhs)
	{
		this->m_pValue += rhs;
		return *this;
	}

	template<typename type>
	inline ConstIterator<type>::ConstIterator(type* pVal)
		: m_pValue{pVal}
	{
	}

	template<typename type>
	inline  Iterator<type>& Iterator<type>::operator-=(int32_t rhs)
	{
		this->m_pValue -= rhs;
		return *this;
	}

	template<typename type>
	inline bool Iterator<type>::operator==(const Iterator& rhs) const
	{
		return this->m_pValue == rhs.m_pValue;
	}

	template<typename type>
	inline bool Iterator<type>::operator!=(const Iterator& rhs) const
	{
		return this->m_pValue != rhs.m_pValue;
	}

	template<typename type>
	inline const type& ConstIterator<type>::operator*() const
	{
		return *m_pValue;
	}

	template<typename type>
	inline const type& ConstIterator<type>::operator->() const
	{
		return *m_pValue;
	}

	template<typename type>
	inline ConstIterator<type>& ConstIterator<type>::operator++()
	{
		++m_pValue;
		return *this;
	}

	template<typename type>
	inline  ConstIterator<type> ConstIterator<type>::operator+(int32_t rhs)
	{
		return ConstIterator{m_pValue + rhs};
	}

	template<typename type>
	inline  ConstIterator<type> ConstIterator<type>::operator-(int32_t rhs)
	{
		return ConstIterator{ m_pValue - rhs };
	}

	template<typename type>
	inline  ConstIterator<type> ConstIterator<type>::operator++(int)
	{
		ConstIterator temp = *this;
		++m_pValue;
		return temp;
	}

	template<typename type>
	inline  ConstIterator<type>& ConstIterator<type>::operator--()
	{
		--m_pValue;
		return *this;
	}

	template<typename type>
	inline  ConstIterator<type> ConstIterator<type>::operator--(int)
	{
		ConstIterator temp = *this;
		--m_pValue;
		return temp;
	}

	template<typename type>
	inline  ConstIterator<type>& ConstIterator<type>::operator+=(int32_t rhs)
	{
		m_pValue += rhs;
		return *this;
	}

	template<typename type>
	inline  ConstIterator<type>& ConstIterator<type>::operator-=(int32_t rhs)
	{
		m_pValue -= rhs;
		return *this;
	}

	template<typename type>
	inline bool ConstIterator<type>::operator==(const ConstIterator& rhs) const
	{
		return m_pValue == rhs.m_pValue;
	}

	template<typename type>
	inline bool ConstIterator<type>::operator!=(const ConstIterator& rhs) const
	{
		return m_pValue != rhs.m_pValue;
	}
}