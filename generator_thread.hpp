#pragma once

#include "fwd.hpp"
#include "store.hpp"
#include <thread>
#include <mutex>
#include "store.hpp"
#include "optional.hpp"

template <class T>
struct generator_thread_impl
{
	generator_thread_impl(const generator_function<T>& gen);
	~generator_thread_impl();

	optional<T> operator()();

private:
	class binary_semaphore
	{
		std::mutex mtx;

	public:
		binary_semaphore() {mtx.lock();}

		void notify() {mtx.unlock();}
		void wait() {mtx.lock();}
	};

	store_t<T> value;
	bool deleted = false;
	bool done = false;

	binary_semaphore gen;
	binary_semaphore main;

	std::thread thread;
};

template <class T>
generator_thread_impl<T>::generator_thread_impl(const generator_function<T>& g)
	: thread{[this, g]() {
		gen.wait();
		auto yield = [this](T&& v)
		{
			if (deleted)
				throw generator_stop();

			value = std::forward<T>(v);
			main.notify();
			gen.wait();

			if (deleted)
				throw generator_stop();
		};
		try
		{
			g(std::move(yield));
		}
		catch (generator_stop&) {}

		done = true;
		main.notify();
	}}
{}

template <class T>
optional<T> generator_thread_impl<T>::operator()()
{
	gen.notify();
	main.wait();
	if (!done)
		return std::forward<T>(*value);
	else
		return {};
}

template <class T>
generator_thread_impl<T>::~generator_thread_impl()
{
	deleted = true;
	while (!done)
		(*this)();
	if (thread.joinable())
		thread.join();
}
