#pragma once
#include <cstdint>

template<typename valueType, typename container>
class RandomAccessIterator final
{
public:
	RandomAccessIterator(container& container, uint32_t idx = 0);
	RandomAccessIterator(const RandomAccessIterator<valueType, container>& other);
	inline valueType& operator*();
	inline RandomAccessIterator<valueType, container>& operator++();
	inline RandomAccessIterator<valueType, container> operator++(int);
	inline RandomAccessIterator<valueType, container>& operator--();
	inline RandomAccessIterator<valueType, container>& operator--(int);
	inline bool operator==(const RandomAccessIterator& other) const;
	inline bool operator!=(const RandomAccessIterator& other) const;
private:
	container& m_Container;
	uint32_t m_CurrentIdx;
};

template<typename valueType, typename container>
inline RandomAccessIterator<valueType, container>::RandomAccessIterator(container& container, uint32_t idx)
	: m_Container{ container }
	, m_CurrentIdx{ idx }
{
}

template<typename valueType, typename container>
inline RandomAccessIterator<valueType, container>::RandomAccessIterator(const RandomAccessIterator<valueType, container>& other)
	: m_Container{ other.m_Container }
	, m_CurrentIdx{ other.m_CurrentIdx }
{
}

template<typename valueType, typename container>
inline valueType& RandomAccessIterator<valueType, container>::operator*()
{
	return m_Container[m_CurrentIdx];
}

template<typename valueType, typename container>
inline RandomAccessIterator<valueType, container>& RandomAccessIterator<valueType, container>::operator++()
{
	++m_CurrentIdx;
	return this;
}

template<typename valueType, typename container>
inline RandomAccessIterator<valueType, container> RandomAccessIterator<valueType, container>::operator++(int)
{
	RandomAccessIterator<valueType, container> out = this;
	++m_CurrentIdx;
	return out;
}

template<typename valueType, typename container>
inline RandomAccessIterator<valueType, container>& RandomAccessIterator<valueType, container>::operator--()
{
	--m_CurrentIdx;
	return this;
}

template<typename valueType, typename container>
inline RandomAccessIterator<valueType, container>& RandomAccessIterator<valueType, container>::operator--(int)
{
	RandomAccessIterator<valueType, container> out = this;
	--m_CurrentIdx;
	return out;
}

template<typename valueType, typename container>
inline bool RandomAccessIterator<valueType, container>::operator==(const RandomAccessIterator<valueType, container>& other) const
{
	return &m_Container == &other.m_Container && m_CurrentIdx == other.m_CurrentIdx;
}

template<typename valueType, typename container>
inline bool RandomAccessIterator<valueType, container>::operator!=(const RandomAccessIterator& other) const
{
	return &m_Container != &other.m_Container || m_CurrentIdx != other.m_CurrentIdx;
}