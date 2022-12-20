#pragma once
#include <type_traits>
#include <memory>
#include <cassert>
namespace Containers
{
	// I should prob move this to another file
	template<class t1, class t2>
	struct Pair
	{
		Pair(t1 item1, t2 item2)
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

		t1 Item1;
		t2 Item2;
	};

	template<class key, class t, class hash, class allocator = std::allocator<Pair<key, t>>>
	class UnorderedMap final
	{
	private:
		struct Bucket
		{
			Bucket() = default;
			Bucket(const Bucket& other);
			Bucket(Bucket&& other);
			Bucket& operator=(const Bucket& rhs);
			Bucket& operator=(Bucket&& rhs);

			Pair<key, t> pair;
			bool isAssigned = false;
			Bucket pNext; // We expect the hash function to have collisions so we store everything as a basic linked list
		};
	public:
#pragma region RuleOf5
		UnorderedMap(const UnorderedMap& other);
		UnorderedMap(UnorderedMap&& other);
		UnorderedMap& operator=(const UnorderedMap& other);
		UnorderedMap& operator=(UnorderedMap&& other);
		~UnorderedMap();
#pragma endregion
#pragma region Constructors

		UnorderedMap();
		UnorderedMap(uint32_t bucketCount, const hash& hash, const allocator& alloc);
		
#pragma endregion
#pragma region Capacity
		_NODISCARD bool IsEmpty() const;
		_NODISCARD uint32_t GetSize() const;
#pragma endregion
#pragma region Modifiers
		void Clear();
		// TODO add insert/insert_or_assign/emplace.e,placehint/try_emplace/erase/extract (everything that requires iterators)
		uint32_t Erase(const key& key);
		void Swap(UnorderedMap& other);
		Bucket* Extract(const key& key);
		// TODO add merge
#pragma endregion
#pragma region Lookup
		_NODISCARD t& At(const key& key);
		_NODISCARD const t& At(const key& key) const;
		_NODISCARD t& operator[](const key& key);
		_NODISCARD const t& operator[](const key& key) const;
		_NODISCARD uint32_t Count(const key& key) const;
		_NODISCARD uint32_t Count(const Key& key, uint32_t hash) const;
		_NODISCARD bool Contains(const key& key) const;
		_NODISCARD bool Contains(const key& key, uint32_t hash) const;
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
		allocator m_Allocator;
		Bucket* m_pBuckets;
		uint32_t m_NrBuckets;
		float m_MaxLoadFactor = 2.f;

		const static uint32_t DEFAULT_NR_BUCKETS = 10;

#pragma region Helper functions
		Bucket* CopyBuckets(const UnorderedMap& other);
		void DeleteBuckets();
		void DeleteBucketRecursive(Bucket* pBucket);
#pragma endregion
	};

	template<class key, class t, class hash, class allocator>
	inline UnorderedMap<key, t, hash, allocator>::UnorderedMap(const UnorderedMap& other)
		: m_Allocator{ other.m_Allocator }
		, m_pBuckets{nullptr}
		, m_NrBuckets{other.m_NrBuckets}
		, m_MaxLoadFactor{other.m_MaxLoadFactor}
	{
		m_pBuckets = CopyBuckets(other);
	}

	template<class key, class t, class hash, class allocator>
	inline UnorderedMap<key, t, hash, allocator>::UnorderedMap(UnorderedMap&& other)
		: m_Allocator{std::move(other.m_Alocator)}
		, m_pBuckets { other.m_pBuckets }
		, m_NrBuckets{ other.m_NrBuckets }
		, m_MaxLoadFactor{ other.m_MaxLoadFactor }
	{
		other.m_pBuckets = nullptr;
		other.m_NrBuckets = 0;
	}

	template<class key, class t, class hash, class allocator>
	inline UnorderedMap<key, t, hash, allocator>& UnorderedMap<key, t, hash, allocator>::operator=(const UnorderedMap& other)
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

	template<class key, class t, class hash, class allocator>
	inline UnorderedMap<key, t, hash, allocator>& UnorderedMap<key, t, hash, allocator>::operator=(UnorderedMap&& other)
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

	template<class key, class t, class hash, class allocator>
	inline UnorderedMap<key, t, hash, allocator>::~UnorderedMap()
	{
		DeleteBuckets();
	}

	template<class key, class t, class hash, class allocator>
	inline UnorderedMap<key, t, hash, allocator>::UnorderedMap()
		: m_Allocator{ allocator{} }
		, m_pBuckets{nullptr}
		, m_NrBuckets{DEFAULT_NR_BUCKETS}
		, m_MaxLoadFactor{2.f}
	{
		m_pBuckets = m_Allocator.allocate(m_NrBuckets);
	}

	template<class key, class t, class hash, class allocator>
	inline UnorderedMap<key, t, hash, allocator>::UnorderedMap(uint32_t bucketCount, const hash& hash, const allocator& alloc)
		: m_Allocator{alloc}
		, m_NrBuckets{bucketCount}
		, m_pBuckets{nullptr}
		, m_MaxLoadFactor{2.f}
	{
		m_pBuckets = m_Allocator.allocate(m_NrBuckets);
	}

	template<class key, class t, class hash, class allocator>
	inline bool UnorderedMap<key, t, hash, allocator>::IsEmpty() const
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

	template<class key, class t, class hash, class allocator>
	inline uint32_t UnorderedMap<key, t, hash, allocator>::GetSize() const
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

	template<class key, class t, class hash, class allocator>
	inline UnorderedMap<key, t, hash, allocator>::Bucket* CopyBuckets(const UnorderedMap<key, t, hash, allocator>& other)
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

	template<class key, class t, class hash, class allocator>
	inline void UnorderedMap<key, t, hash, allocator>::DeleteBuckets()
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

	template<class key, class t, class hash, class allocator>
	inline void UnorderedMap<key, t, hash, allocator>::DeleteBucketRecursive(Bucket* pBucket)
	{
		if (pBucket->pNext)
		{
			DeleteBucketRecursive(pBucket.pNext);
		}
		m_Allocator.deallocate(pBucket, 1);
	}
}