#include "generated.hpp"
#include "range.hpp"
#include <cassert>
#include <iostream>

int main()
{
	// Test simple range. 5 times.
	{
		auto odd = range(1, 10, 2);
		int ok[] = {1, 3, 5, 7, 9};

		for (int i = 0; i < 5; ++i)
		{
			auto j = 0;
			for (auto n : odd)
				assert(ok[j++] == n);
			assert(j == 5);
		}
	}

	// Test empty generator.
	{
		auto x = 0;
		for (auto& t : generated<int>([](yield_t<int>&& yield) {}))
			++x;
		assert(x == 0);
	}

	// Test subgenerator.
	{
		auto i = 0;
		for (auto& n : generated<int>([](yield_t<int>&& yield) {
			assert((yield << range(0, 5)) == 5);
		}))
			assert(i++ == n);
		assert(i == 5);
	}

	// Test not-copyable type.
	{
		auto x = 0;
		for (auto& t : generated<std::thread>([](yield_t<std::thread>&& yield) {
			yield(std::thread{});
		}))
			++x;
		assert(x == 1);
	}

	// Test reference.
	{
		auto x = 5;
		for (auto& i : generated<int&>([&x](yield_t<int&>&& yield) {
			yield(x);
		}))
			i = 3;
		assert(x == 3);
	}

	std::cout << "Passed!" << std::endl;
}
