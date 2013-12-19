#pragma once

#include "fwd.hpp"
#include <thread>
#include <mutex>

template <class T>
struct generator_thread_impl
{
	generator_thread_impl(const generator_function<T>& gen, bool& not_empty);
	~generator_thread_impl();

	bool next();
	T& get();

private:
	template <class U>
	class store_t
	{
		U _v;
	public:
		void store(U&& v) {_v = std::move(v);}
		U& load() {return _v;}
	};

	template <class U>
	class store_t<U&>
	{
		U* _v;
	public:
		void store(U& v) {_v = &v;}
		U& load() {return *_v;}
	};

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
	bool working = true;

	binary_semaphore gen;
	binary_semaphore main;

	std::thread thread;
};

template <class T>
generator_thread_impl<T>::generator_thread_impl(const generator_function<T>& g, bool& not_empty)
	: thread{[this, g]() {
		auto yield = [this](T&& v)
		{
			if (deleted)
				throw generator_stop();

			value.store(std::forward<T>(v));
			main.notify();
			gen.wait();
		};
		try
		{
			g(std::move(yield));
		}
		catch (generator_stop&) {}

		working = false;
		main.notify();
	}}
{
	main.wait();
	not_empty = working;
}

template <class T>
bool generator_thread_impl<T>::next()
{
	gen.notify();
	main.wait();
	return working;
}

template <class T>
T& generator_thread_impl<T>::get()
{
	return value.load();
}

template <class T>
generator_thread_impl<T>::~generator_thread_impl()
{
	deleted = true;
	while (working)
		next();
	if (thread.joinable())
		thread.join();
}
