#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "Vector.h"
#include <iostream>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

class TestDestructor
{
public:
	~TestDestructor()
	{
		if (m_pIsDestroyed)
		{
			*m_pIsDestroyed = true;
		}
	}
	bool* m_pIsDestroyed;
};

class TestMove
{
public:
	TestMove(int val)
		: m_Val{ val }
	{}
	int m_Val;
};

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);

	{
		int result = Catch::Session().run(argc, argv);
	}

}

TEST_CASE("Vector De/constructor Tests")
{
	Container::Vector<int> defaultTest{};
	REQUIRE(defaultTest.Size() == 0);
	const int testVal{44};
	const uint32_t size{100};
	Container::Vector<int> sizeTest{ size, testVal };
	REQUIRE(sizeTest.Size() == size);
	bool rightVal = true;
	for (int i{}; i < size; ++i)
	{
		rightVal = rightVal && sizeTest[i] == testVal;
	}
	REQUIRE(rightVal);
	Container::Vector<int> capacityTest{ size };
	REQUIRE(capacityTest.Capacity() == size);
	bool destructorFired[size]{ false };

	{
		Container::Vector<TestDestructor> destructorTest{ size, TestDestructor{} };
		for (uint32_t i{}; i < size; ++i)
		{
			destructorTest[i].m_pIsDestroyed = destructorFired + i;
		}
	}
	bool allDestructorsFired = true;
	for (int i{}; i < size; ++i)
	{
		allDestructorsFired = allDestructorsFired && destructorFired[i];
	}
	REQUIRE(allDestructorsFired);
}

TEST_CASE("Vector Accessors tests")
{
	const int size{ 100 };
	Container::Vector<int> vec{ size };
	for (int i{}; i < size; i++)
	{
		vec.PushBack(i);
	}

	// At test
	bool valsRight = true;
	for (int i{}; i < size; i++)
	{
		valsRight = valsRight && vec.At(i) == i;
	}
	REQUIRE(valsRight);

	// operator[] test
	valsRight = true;
	for (int i{}; i < size; i++)
	{
		valsRight = valsRight && vec[i] == i;
	}
	REQUIRE(valsRight);

	//Front Test
	REQUIRE(vec.Front() == 0);
	// Back Test
	REQUIRE(vec.Back() == size - 1);
	// Data Test
	valsRight = true;
	int* pData = vec.Data();
	for (int i{}; i < size; i++)
	{
		valsRight = valsRight && pData[i] == i;
	}
	REQUIRE(valsRight);
}

TEST_CASE("Vector Modifier tests")
{

	Container::Vector<int> vec{};
	vec.PushBack(0);
	REQUIRE(vec.Size() == 1);
	vec.PopBack();
	REQUIRE(vec.Size() == 0);
	const int nrPushes = 1000;
	for (int i{}; i < nrPushes; ++i)
	{
		vec.PushBack(i);
	}
	REQUIRE(vec.Size() == nrPushes);
	bool rightVal = true;
	for (int i{}; i < nrPushes; ++i)
	{
		rightVal = rightVal && vec[i] == i;
	}
	REQUIRE(rightVal);

	for (int i{}; i < nrPushes; ++i)
	{
		vec.PopBack();
	}
	REQUIRE(vec.Size() == 0);

	int i{ 5 };
	vec.PushBack(std::move(i));
	REQUIRE(vec.Size() == 1);
	REQUIRE(vec.Front() == 5);
	const int size = 100;
	bool destructorFired[size]{ false };

	Container::Vector<TestDestructor> clearTest{ size, TestDestructor{} };
	REQUIRE(clearTest.Size() == size);
	for (uint32_t i{}; i < size; ++i)
	{
		clearTest[i].m_pIsDestroyed = destructorFired + i;
	}
	clearTest.Clear();
	bool allDestructorsFired = true;
	for (int i{}; i < size; ++i)
	{
		allDestructorsFired = allDestructorsFired && destructorFired[i];
	}
	REQUIRE(allDestructorsFired);

	Container::Vector<int> vec1{};
	Container::Vector<int> vec2{};
	int* pInts1 = vec1.Data();
	int* pInts2 = vec2.Data();
	vec1.Swap(vec2);
	REQUIRE(pInts1 == vec2.Data());
	REQUIRE(pInts1 != vec1.Data());
	REQUIRE(pInts2 == vec1.Data());
	REQUIRE(pInts2 != vec2.Data());
}


