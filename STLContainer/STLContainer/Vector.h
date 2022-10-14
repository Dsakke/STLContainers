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

