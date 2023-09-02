#pragma once
#include <type_traits>
#include <memory>
#include <cassert>
#include <functional>

namespace Containers
{
	// I should prob move this to another file
	template<class _T1, class _T2>
	struct Pair
	{
		Pair(_T1 item1, _T2 item2)
			: Item1{item1}
			, Item2{item2}
		{}

		Pair(const Pair& other)
			: Item1{other.Item1}
			, Item2{other.Item2}
		{}
	
		Pair& operator=(const Pair& other)
		{
			Item1 = other.Item1;
			Item2 = other.Item2;
		}

		_T1 Item1;
		_T2 Item2;
	};

	template<class _Key, class _T, class _Hash, class _Allocator = std::allocator<Pair<_Key, _T>>>
	class UnorderedMap final
	{
		static_assert(std::is_integral < decltype(std::function{ _Hash::Operator })::result_type > ::value);

	public:
		struct Bucket
		{
			Bucket() = default;
			Bucket(const Bucket& other);
			Bucket(Bucket&& other);
			Bucket& operator=(const Bucket& rhs);
			Bucket& operator=(Bucket&& rhs);

			Pair<_Key, _T> pair;
			bool isAssigned = false;
			Bucket* pNext; // We expect the hash function to have collisions so we store everything as a basic linked list
		};

#pragma region RuleOf5
		UnorderedMap(const UnorderedMap& other);
		UnorderedMap(UnorderedMap&& other);
		UnorderedMap& operator=(const UnorderedMap& other);
		UnorderedMap& operator=(UnorderedMap&& other);
		~UnorderedMap();
#pragma endregion
#pragma region Constructors

		UnorderedMap();
		UnorderedMap(uint32_t bucketCount, const _Hash& hash, const _Allocator& alloc);
		
#pragma endregion
#pragma region Capacity
		_NODISCARD bool IsEmpty() const;
		_NODISCARD uint32_t GetSize() const;
#pragma endregion
#pragma region Modifiers
		void Clear();
		// TODO add insert/insert_or_assign/emplace.e,placehint/try_emplace/erase/extract (everything that requires iterators)
		uint32_t Erase(const _Key& key);
		void Swap(UnorderedMap& other);
		Bucket* Extract(const _Key& key);
		// TODO add merge
#pragma endregion
#pragma region Lookup
		_NODISCARD _T& At(const _Key& key);
		_NODISCARD const _T& At(const _Key& key) const;
		_NODISCARD _T& operator[](const _Key& key);
		_NODISCARD const _T& operator[](const _Key& key) const;
		_NODISCARD uint32_t Count(const _Key& key) const;
		_NODISCARD uint32_t Count(const Key& key, uint32_t hash) const;
		_NODISCARD bool Contains(const _Key& key) const;
		_NODISCARD bool Contains(const _Key& key, uint32_t hash) const;
		// TODO: add equal_range
#pragma endregion
#pragma region Hash Policy
		_NODISCARD float LoadFactor() const;
		_NODISCARD float MaxLoadFactor() const;
		void SetMaxLoadFactor();
		void Rehash(uint32_t count);
		void Reserve(uint32_t count);
#pragma endregion
	private:
		_Allocator m_Allocator;
		Bucket* m_pBuckets;
		uint32_t m_NrBuckets;
		float m_MaxLoadFactor = 2.f;

		const static uint32_t DEFAULT_NR_BUCKETS = 10;

#pragma region Helper functions
		Bucket* CopyBuckets(const UnorderedMap& other);
		void DeleteBuckets();
		void DeleteBucketRecursive(Bucket* pBucket);
		Bucket* FindBucket(const _Key& val);
#pragma endregion
	};

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline UnorderedMap<_Key, _T, _Hash, _Allocator>::UnorderedMap(const UnorderedMap& other)
		: m_Allocator{ other.m_Allocator }
		, m_pBuckets{nullptr}
		, m_NrBuckets{other.m_NrBuckets}
		, m_MaxLoadFactor{other.m_MaxLoadFactor}
	{
		m_pBuckets = CopyBuckets(other);
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline UnorderedMap<_Key, _T, _Hash, _Allocator>::UnorderedMap(UnorderedMap&& other)
		: m_Allocator{std::move(other.m_Alocator)}
		, m_pBuckets { other.m_pBuckets }
		, m_NrBuckets{ other.m_NrBuckets }
		, m_MaxLoadFactor{ other.m_MaxLoadFactor }
	{
		other.m_pBuckets = nullptr;
		other.m_NrBuckets = 0;
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline UnorderedMap<_Key, _T, _Hash, _Allocator>& UnorderedMap<_Key, _T, _Hash, _Allocator>::operator=(const UnorderedMap& other)
	{
		if (m_pBuckets)
		{
			DeleteBuckets();
		}

		m_pBuckets = CopyBuckets(other);
		m_NrBuckets = other.m_NrBuckets;
		m_Allocator = other.m_Allocator;
		m_MaxLoadFactor = other.m_MaxLoadFactor;
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline UnorderedMap<_Key, _T, _Hash, _Allocator>& UnorderedMap<_Key, _T, _Hash, _Allocator>::operator=(UnorderedMap&& other)
	{
		if (m_pBuckets)
		{
			DeleteBuckets();
		}

		m_pBuckets = other.m_pBuckets;
		m_NrBuckets = other.m_NrBuckets;
		m_Allocator = std::move(other.m_Allocator);
		m_MaxLoadFactor = other.m_MaxLoadFactor;

		other.m_pBuckets = nullptr;
		other.m_NrBuckets = nullptr;
		// No need to set the other values to null, as they won't cause problems
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline UnorderedMap<_Key, _T, _Hash, _Allocator>::~UnorderedMap()
	{
		DeleteBuckets();
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline UnorderedMap<_Key, _T, _Hash, _Allocator>::UnorderedMap()
		: m_Allocator{ _Allocator{} }
		, m_pBuckets{nullptr}
		, m_NrBuckets{DEFAULT_NR_BUCKETS}
		, m_MaxLoadFactor{2.f}
	{
		m_pBuckets = m_Allocator.allocate(m_NrBuckets);
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline UnorderedMap<_Key, _T, _Hash, _Allocator>::UnorderedMap(uint32_t bucketCount, const _Hash& hash, const _Allocator& alloc)
		: m_Allocator{alloc}
		, m_NrBuckets{bucketCount}
		, m_pBuckets{nullptr}
		, m_MaxLoadFactor{2.f}
	{
		m_pBuckets = m_Allocator.allocate(m_NrBuckets);
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline bool UnorderedMap<_Key, _T, _Hash, _Allocator>::IsEmpty() const
	{
		for (uint32_t i{}; i < m_NrBuckets; ++i)
		{
			if(m_pBuckets.isAssigned)
			{
				return false;
			}
		}
		return true;
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline uint32_t UnorderedMap<_Key, _T, _Hash, _Allocator>::GetSize() const
	{
		uint32_t totalCount{};
		for (uint32_t i{}; i < m_NrBuckets; ++i)
		{
			if (m_pBuckets[i].isAssigned)
			{
				continue;
			}

			++totalCount;
			Bucket* pBucket = m_pBuckets[i].pNext;
			while (pBucket)
			{
				if (!pBucket.isAssigned)
				{
					break;
				}
				++totalCount;
				pBucket = pBucket.pNext;
			}
		}
		return totalCount;
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline void UnorderedMap<_Key, _T, _Hash, _Allocator>::Clear()
	{
		for (int i = 0; i < m_NrBuckets; ++i)
		{
			if (m_pBuckets[i].pNext)
			{
				m_pBuckets[i].isAssigned = false;

				// could also set buckets within the same bucket to isAssigned = false
				// This is more memory intensive but should be faster
				DeleteBucketRecursive(m_pBuckets[i].pNext);
			}
		}
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline uint32_t UnorderedMap<_Key, _T, _Hash, _Allocator>::Erase(const _Key& key)
	{
		auto hash = _Hash{}(val);

		auto bucketIdx = hash % m_NrBuckets;

		Bucket* pCurrentBucket = m_pBuckets[bucketIdx];
		Bucket* pPrevBucket = nullptr;
		while (pCurrentBucket)
		{
			if (pCurrentBucket->pair.Item1 == val)
			{
				break;
			}

			pPrevBucket = pCurrentBucket;
			pCurrentBucket = pCurrentBucket->pNext;
		}

		if (pCurrentBucket)
		{
			if (pPrevBucket)
			{
				pPrevBucket->pNext = pCurrentBucket->pNext;
				m_Allocator.deallocate(pCurrentBucket, 1);
			}
			else
			{
				if (pCurrentBucket->pNext)
				{
					*pCurrentBucket = *(pCurrentBucket->pNext)
				}
			}

			return 1;
		}
		return 0;
	}

	template<class _Key, class t, class _Hash, class _Allocator>
	inline void UnorderedMap<_Key, t, _Hash, _Allocator>::Swap(UnorderedMap& other)
	{
		_Allocator tempAlloc = other.m_Allocator;
		Bucket* pTempBuckets = other.m_pBuckets;
		uint32_t tempNrBuckets = other.m_NrBuckets;
		float tempMaxLoadFactor = other.m_MaxLoadFactor;

		other.m_Allocator = m_Allocator;
		other.m_pBuckets = m_pBuckets;
		other.m_NrBuckets = m_NrBuckets;
		other.m_MaxLoadFactor = m_MaxLoadFactor;

		m_Allocator = tempAlloc;
		m_pBuckets = pTempBuckets;
		m_NrBuckets = tempNrBuckets;
		m_MaxLoadFactor = tempMaxLoadFactor;
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline UnorderedMap<_Key, _T, _Hash, _Allocator>::Bucket* UnorderedMap<_Key, _T, _Hash, _Allocator>::Extract(const _Key& key)
	{
		auto hash = _Hash{}(val);

		auto bucketIdx = hash % m_NrBuckets;

		Bucket* pCurrentBucket = m_pBuckets[bucketIdx];
		Bucket* pPrevBucket = nullptr;
		while (pCurrentBucket)
		{
			if (pCurrentBucket->pair.Item1 == val)
			{
				break;
			}

			pPrevBucket = pCurrentBucket;
			pCurrentBucket = pCurrentBucket->pNext;
		}

		pPrevBucket->pNext = pCurrentBucket->pNext;
		pCurrentBucket->pNext = nullptr;
		return pCurrentBucket;
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline UnorderedMap<_Key, _T, _Hash, _Allocator>::Bucket* CopyBuckets(const UnorderedMap<_Key, _T, _Hash, _Allocator>& other)
	{
		Bucket* pOut = m_Allocator.allocate(other.m_NrBuckets);
		for (uint32_t i{}; i < other.m_NrBuckets; ++i)
		{
			pOut[i] = Bucket{};
		}

		for (uint32_t i{}; i < other.m_NrBuckets; ++i)
		{
			pOut[i] = other.m_pBuckets[i];
			Bucket* pNext = other.m_pBuckets[i].pNext;
			Bucket* pCurrent = pOut + i;
			while (pNext)
			{
				m_pBuckets[i].pNext = new Bucket{ *pNext };
				pCurrent = pOut[i].pNext;
				pNext = other.m_pBuckets[i].pNext;
			}
		}
		return pOut;
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline void UnorderedMap<_Key, _T, _Hash, _Allocator>::DeleteBuckets()
	{
		for (uint32_t i{}; i < m_NrBuckets; ++i)
		{
			if (m_pBuckets[i].pNext)
			{
				DeleteBucketRecursive(m_pBuckets[i].pNext); // In a loop this would be better but I can't think of a clean way to do it
			}
		}
		m_Allocator.deallocate(m_pBuckets, m_NrBuckets);
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline void UnorderedMap<_Key, _T, _Hash, _Allocator>::DeleteBucketRecursive(Bucket* pBucket)
	{
		if (pBucket->pNext)
		{
			DeleteBucketRecursive(pBucket.pNext);
		}
		m_Allocator.deallocate(pBucket, 1);
	}

	template<class _Key, class _T, class _Hash, class _Allocator>
	inline UnorderedMap<_Key, _T, _Hash, _Allocator>::Bucket* UnorderedMap<_Key, _T, _Hash, _Allocator>::FindBucket(const _Key& val)
	{
		static_assert(std::is_integral < decltype(std::function{ _Hash::Operator })::result_type >::value )
		auto hash = _Hash{}(val);

		auto bucketIdx = hash % m_NrBuckets;

		Bucket* pCurrentBucket = m_pBuckets[bucketIdx];
		while (pCurrentBucket)
		{
			if (pCurrentBucket->pair.Item1 == val)
			{
				return pCurrentBucket;
			}

			pCurrentBucket = pCurrentBucket->pNext;
		}

		return nullptr;
	}
}