#pragma once

#include "generator_thread.hpp"
template <class T> using generator_impl = generator_thread_impl<T>;

// Only for reference.
template <class T>
struct generator_impl_interface
{
	generator_impl_interface(const generator_function<T>&);
	optional<T> operator()();
};
