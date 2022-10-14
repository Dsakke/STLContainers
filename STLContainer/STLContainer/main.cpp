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
	TestDestructor(bool& isDestroyed)
		: m_IsDestroyed{isDestroyed}
	{
	}
private:
	bool& m_IsDestroyed;
};

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);

	{
		int result = Catch::Session().run(argc, argv);
	}

}

TEST_CASE("De/constructor Tests")
{
	Container::Vector<TestDestructor> testDestructor{};
}

TEST_CASE("Push/pop tests")
{

	Container::Vector<int> vec{};
	REQUIRE(vec.Size() == 0);
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
	for (int i{}; i < nrPushes; ++i)
	{
		vec.PopBack();
	}
	REQUIRE(vec.Size() == 0);
}
