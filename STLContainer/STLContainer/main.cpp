#define Testing
//#define Benchmarking

#ifdef Testing
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // Testing
#ifdef Benchmarking
#include <vector>
#endif // Benchmarking


#include "Vector.h"
#include <stdlib.h>
#include <chrono>
#include <iostream>

#ifdef Testing
#pragma region Vector Tests
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

// TODO rewrite test to use a class that has a ptr that gets deleted in the destructor
// That would be the more accurate way to track whether or not the vector throws memory leaks or call the destructor twice on classes
TEST_CASE("Vector De/constructor Tests")
{
	Container::Vector<int> defaultTest{};
	REQUIRE(defaultTest.Size() == 0);
	const int testVal{ 44 };
	const uint32_t size{ 100 };
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

	Container::Vector<int> vec{ 100, 0 };
	for (int i{}; i < size; ++i)
	{
		vec[i] = i;
	}
	Container::Vector<int> vecCopy{ vec };
	rightVal = true;
	for (int i{}; i < size; ++i)
	{
		rightVal = rightVal && vecCopy[i] == i;
	}
	REQUIRE(rightVal);
	REQUIRE(vec.Data() != vecCopy.Data());
	int* pVecData{ vec.Data() };
	Container::Vector<int> vec1{ std::move(vec) };

	REQUIRE(pVecData == vec1.Data());
	REQUIRE(!vec.Data());

	rightVal = true;
	vec = vec1;
	for (int i{}; i < size; ++i)
	{
		rightVal = rightVal && vec[i] == i;
	}

	REQUIRE(rightVal);
	REQUIRE(vec.Data() != vecCopy.Data());

	pVecData = vec.Data();
	vec1 = std::move(vec);

	REQUIRE(vec1.Data() == pVecData);
	REQUIRE(!vec.Data());
}

TEST_CASE("Iterator tests")
{
	Container::Vector<int> vec0{};

	// In an empty vec the begin and the end should be the same
	REQUIRE(vec0.Begin() == vec0.End());
	REQUIRE(vec0.CBegin() == vec0.CEnd());

	vec0.PushBack(0);
	REQUIRE(vec0.Begin() != vec0.End());
	REQUIRE(vec0.CBegin() != vec0.CEnd());
	vec0.Clear();

	const int size = 100;
	for (int i{}; i < size; ++i)
	{
		vec0.PushBack(i);
	}

	// A begin iterator + the vector size should always == it's end
	REQUIRE((vec0.Begin() + vec0.Size()) == vec0.End());
	REQUIRE((vec0.CBegin() + vec0.Size()) == vec0.CEnd());
	auto it = vec0.Begin();
	auto cIt = vec0.CBegin();
	it += vec0.Size();
	cIt += vec0.Size();
	REQUIRE(it == vec0.End());
	REQUIRE(cIt == vec0.CEnd());
	it -= vec0.Size();
	cIt -= vec0.Size();
	REQUIRE(cIt == vec0.CBegin());
	REQUIRE(it == vec0.Begin());

	it = vec0.Begin();
	cIt = vec0.CBegin();
	bool valuesCorrect = true;
	bool cValuesCorrect = true;
	for (int i{}; i < size; ++i)
	{
		valuesCorrect = vec0[i] == *it && valuesCorrect;
		cValuesCorrect = vec0[i] == *cIt && cValuesCorrect;
		++it;
		++cIt;
	}
	REQUIRE(valuesCorrect);
	REQUIRE(cValuesCorrect);
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
	
	Container::Vector<bool> vec3{};
	vec3.PushBack(false);
	auto it = vec3.Begin();
	vec3.Emplace(it, true);
	REQUIRE(vec3[0]);
	vec3[0] = false;
	vec3.Emplace(vec3.End(), true);
	REQUIRE(vec3.Back());

	Container::Vector<bool> vec4{};
	vec4.PushBack(false);
	it = vec4.Begin();
	vec4.Insert(it, true);
	REQUIRE(vec4[0]);
	vec4[0] = false;
	vec4.Insert(vec4.End(), true);
	REQUIRE(vec4.Back());

	Container::Vector<int> vec5{};
	for (int i = 0; i < size; ++i)
	{
		vec5.EmplaceBack(i);
	}
	REQUIRE(vec5.Size() == size);
	bool correctValues = true;
	for (int i = 0; i < size; ++i)
	{
		correctValues = vec5[i] == i && correctValues;
	}
	REQUIRE(correctValues);

	vec5.Erase(vec5.Begin());
	REQUIRE(vec5.Front() == 1);
	REQUIRE(vec5.Size() == size - 1);
	auto it5 = vec5.Begin();
	const int increment = 5;
	it5 += increment;
	vec5.Erase(it5, vec5.End());

	// check if it deleted the right amoung of values
	REQUIRE(vec5.Size() == increment);

	correctValues = true;
	for (int i{ 0 }; i < increment; ++i)
	{
		correctValues = vec5[i] == i + 1 && correctValues;
	}

	REQUIRE(correctValues);




}

#pragma endregion
#endif // Testing

#ifdef Benchmarking
void PushBackBench();
double CalcAverage(double* pTimes, const int count, double& totalTimeOut);

class Timer
{

public:
	Timer()
		: m_StartPoint{}
	{

	}

	~Timer()
	{
	}

	void Start()
	{
		m_StartPoint = std::chrono::high_resolution_clock::now();
	}

	double Stop()
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();

		auto startTime = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartPoint).time_since_epoch().count();
		auto endTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
		auto duration = endTime - startTime;
		double ms = duration * 0.001;
		return ms;
	}
private:

	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartPoint;
};

void PushBackBench()
{
#pragma region  Pushback Benchmarking
	{
		const int nrTests = 10000;
		const int nrPushes = 1000;


		Timer timer{};
		double* pMyVecTimes = new double[nrTests]();
		for (int test = 0; test < nrTests; ++test)
		{
			Container::Vector<int> vec{  };
			vec.Reserve(nrPushes);
			timer.Start();
			for (int i = 0; i < nrPushes; ++i)
			{
				vec.PushBack(i);
			}
			pMyVecTimes[test] = timer.Stop();
		}
		double myTotalTime{};
		std::cout << "My Vector average:\t" << CalcAverage(pMyVecTimes, nrTests, myTotalTime) << std::endl;
		std::cout << "My Vector total time:\t" << myTotalTime << std::endl;

		double* pStlVecTimes = new double[nrTests]();

		for (int test = 0; test < nrTests; ++test)
		{
			std::vector<int> vec{ };
			vec.reserve(nrPushes);
			timer.Start();
			for (int i = 0; i < nrPushes; ++i)
			{
				vec.push_back(i);
			}
			pStlVecTimes[test] = timer.Stop();
		}

		double stlTotalTime{};
		std::cout << "STL vector average:\t" << CalcAverage(pStlVecTimes, nrTests, stlTotalTime) << std::endl;
		std::cout << "STL Vector total time:\t" << stlTotalTime << std::endl;
	}
#pragma endregion
}

double CalcAverage(double* pTimes, const int count, double& totalTimeOut)
{
	totalTimeOut = 0.0;
	for (int i = 0; i < count; ++i)
	{
		totalTimeOut += pTimes[i];
	}

	return totalTimeOut / count;
}


int main()
{
	PushBackBench();
}

#endif // Benchmarking


