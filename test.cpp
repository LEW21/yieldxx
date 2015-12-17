#include "generated.hpp"
#include "range.hpp"
#include <cassert>
#include <iostream>
#include <thread>

using namespace xx;

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
		for (auto& t : generated<int>([](generator<int>::yield&& yield) {}))
			++x;
		assert(x == 0);
	}

	// Test subgenerator.
	{
		auto i = 0;
		for (auto& n : generated<int>([](generator<int>::yield&& yield) {
			assert((yield << range(0, 5)) == 5);
		}))
			assert(i++ == n);
		assert(i == 5);
	}

	// Test not-copyable type.
	{
		auto x = 0;
		for (auto& t : generated<std::thread>([](generator<std::thread>::yield&& yield) {
			yield(std::thread{});
		}))
			++x;
		assert(x == 1);
	}

	// Test reference.
	{
		auto x = 5;
		for (auto& i : generated<int&>([&x](generator<int&>::yield&& yield) {
			yield(x);
		}))
			i = 3;
		assert(x == 3);
	}

	// Test stopping generator.
	{
		auto gen = generated<int>([](generator<int>::yield&& yield) {
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
		auto gen = generated<int>([](generator<int>::yield&& yield) {
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
		auto gen = generated<int>([](generator<int>::yield&& yield) {
			yield(1);
		});

		auto it = std::begin(gen);
		assert(it);
		++it;
		assert(!it);
	}

	// Test move.
	{
		auto gen = generated<int>{[](generator<int>::yield&& yield) {
			yield(1);
			yield(2);
			yield(3);
		}};

		auto it = std::begin(gen);
		assert(*it == 1);
		auto it2 = std::move(it);
		++it2;
		assert(*it2 == 2);
	}

	// Test rethrow.
	{
		auto gen = generated<int>{[](generator<int>::yield&& yield) {
			yield(1);
			throw std::runtime_error{"So much error"};
		}};

		auto it = std::begin(gen);
		assert(*it == 1);
		try
		{
			++it;
			assert(false);
		}
		catch (std::runtime_error& e)
		{}
	}

	// Test single pass generator.
	{
		auto it = generator<int>{[val = 5](generator<int>::yield&& yield) mutable {
			yield(val);
			val = 0;
		}};

		assert(*it == 5);
		++it;
		assert(!it);

		// for (auto i : gen) // shall not compile
		//	assert(i == 5);
	}

	std::cout << "Passed!" << std::endl;
}
