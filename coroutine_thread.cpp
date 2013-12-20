#include "coroutine.hpp"

#include <thread>
#include <mutex>

struct coroutine_impl
{
	coroutine_impl(coroutine::body f);
	bool operator()();
	~coroutine_impl();

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

coroutine::coroutine()
{}

coroutine::coroutine(coroutine&& other)
	: p(std::move(other.p))
{}

coroutine::coroutine(coroutine::body f)
	: p(new coroutine_impl(std::move(f)))
{}

bool coroutine::operator()()
{
	if (!p)
		throw std::out_of_range("coroutine::operator()");

	auto ok = (*p)();
	if (!ok)
		p = {};

	return ok;
}

coroutine::~coroutine()
{}

coroutine_impl::coroutine_impl(coroutine::body f)
	: thread{[this, f]() {
		gen.wait();
		auto yield = [this]()
		{
			if (deleted)
				throw coroutine::stop();

			main.notify();
			gen.wait();

			if (deleted)
				throw coroutine::stop();
		};
		try
		{
			f(std::move(yield));
		}
		catch (coroutine::stop&) {}

		done = true;
		main.notify();
	}}
{}

bool coroutine_impl::operator()()
{
	gen.notify();
	main.wait();
	return !done;
}

coroutine_impl::~coroutine_impl()
{
	deleted = true;
	while (!done)
		(*this)();
	if (thread.joinable())
		thread.join();
}
