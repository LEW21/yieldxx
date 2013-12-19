#pragma once

#include "fwd.hpp"
#include "coroutine.hpp"
#include "store.hpp"
#include "optional.hpp"

template <class T>
struct generator_impl
{
	generator_impl(const generator_function<T>& gen);

	optional<T> operator()();

private:
	coroutine coro;
	store_t<T> value;
};

template <class T>
generator_impl<T>::generator_impl(const generator_function<T>& gen)
	: coro{[this, gen](coroutine::yield&& yield) {
		gen([this, yield](T&& v){
			value = std::forward<T>(v);
			yield();
		});
	}}
{}

template <class T>
optional<T> generator_impl<T>::operator()()
{
	if (coro())
		return std::forward<T>(*value);
	else
		return {};
}
