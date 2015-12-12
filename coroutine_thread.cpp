#include "coroutine.hpp"

#include <thread>
#include <mutex>
#include <cassert>

namespace xx
{
	struct coroutine_impl
	{
		coroutine_impl(coroutine::body);
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
		std::exception_ptr exception;

		binary_semaphore gen;
		binary_semaphore main;

		std::thread thread;
	};

	coroutine::coroutine()
	{}

	coroutine::coroutine(coroutine&& other)
		: p(std::move(other.p))
	{}

	coroutine& coroutine::operator=(coroutine&& other)
	{
		p = std::move(other.p);
		return *this;
	}

	coroutine::coroutine(coroutine::body f)
		: p(std::make_unique<coroutine_impl>(std::move(f)))
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

	coroutine_impl::coroutine_impl(coroutine::body body)
		: thread{[this, body]() {
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
				body(std::move(yield));
			}
			catch (coroutine::stop&) {}
			catch (...) { exception = std::current_exception(); }

			done = true;
			main.notify();
		}}
	{}

	bool coroutine_impl::operator()()
	{
		assert(!done);
		gen.notify();
		main.wait();
		if (exception) std::rethrow_exception(exception);
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
}
