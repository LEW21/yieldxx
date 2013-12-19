#pragma once

#include "fwd.hpp"
#include <thread>
#include <mutex>

struct coroutine
{
	using yield = std::function<void()>;

	coroutine(std::function<void(yield&&)>&&);
	~coroutine();

	bool operator()();

private:
	class binary_semaphore
	{
		std::mutex mtx;

	public:
		binary_semaphore() {mtx.lock();}

		void notify() {mtx.unlock();}
		void wait() {mtx.lock();}
	};

	bool deleted = false;
	bool done = false;

	binary_semaphore gen;
	binary_semaphore main;

	std::thread thread;
};

coroutine::coroutine(std::function<void(coroutine::yield&&)>&& f)
	: thread{[this, f]() {
		gen.wait();
		auto yield = [this]()
		{
			if (deleted)
				throw generator_stop();

			main.notify();
			gen.wait();

			if (deleted)
				throw generator_stop();
		};
		try
		{
			f(std::move(yield));
		}
		catch (generator_stop&) {}

		done = true;
		main.notify();
	}}
{}

bool coroutine::operator()()
{
	gen.notify();
	main.wait();
	return !done;
}

coroutine::~coroutine()
{
	deleted = true;
	while (!done)
		(*this)();
	if (thread.joinable())
		thread.join();
}
