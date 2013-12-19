#include "generated.hpp"
#include "range.hpp"
#include <cassert>
#include <iostream>
#include <thread>

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

	// Test stopping generator.
	{
		auto gen = generated<int>([](yield_t<int>&& yield) {
			yield(1);
			yield(2);
			assert(0);
		});

		auto it = std::begin(gen);
		assert(*it == 1);
		++it;
		assert(*it == 2);
		// Here it DIES! (Iterator becomes destroyed. Assert(0) does not fire.)
	}

	// Test stopping generator in a prettier way.
	{
		auto gen = generated<int>([](yield_t<int>&& yield) {
			yield(1);
			yield(2);
			assert(0);
		});

		try
		{
			for (auto n : gen)
				if (n == 2)
					throw std::exception{};
			// Here it DIES! (Iterator becomes destroyed. Assert(0) does not fire.)
		}
		catch (std::exception&)
		{}
	}

	// Test operator bool().
	{
		auto gen = generated<int>([](yield_t<int>&& yield) {
			yield(1);
		});

		auto it = std::begin(gen);
		assert(it);
		++it;
		assert(!it);
	}

	std::cout << "Passed!" << std::endl;
}
