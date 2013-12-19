#pragma once

#include "fwd.hpp"
#include <memory>
#include "coroutine.hpp"
#include "store.hpp"

template <class T>
class generator
{
	coroutine coro;
	store_t<T> value;

	template <class U>
	friend bool operator==(const generator<U>& a, const generator<U>& b);

public:
	generator() = default;
	generator(const generator_function<T>& gen);

	auto operator*() -> T&;
	auto operator*() const -> const T&;
	auto operator->() -> typename std::remove_reference<T>::type* {return &**this;}
	auto operator->() const -> const typename std::remove_reference<T>::type* {return &**this;}

	generator& operator++();

	explicit operator bool() {return bool(coro);}
};

template <class T>
generator<T>::generator(const generator_function<T>& gen)
	: coro{[this, gen](coroutine::yield&& yield) {
		gen([this, yield](T&& v){
			value = std::forward<T>(v);
			yield();
		});
	}}
{
	++(*this);
}

template <class T>
auto generator<T>::operator*() -> T&
{
	if (!coro)
		throw std::out_of_range("generator::operator*");

	return *value;
}

template <class T>
auto generator<T>::operator*() const -> const T&
{
	if (!coro)
		throw std::out_of_range("generator::operator*");

	return *value;
}

template <class T>
generator<T>& generator<T>::operator++()
{
	coro();
	return *this;
}

template <class T>
bool operator==(const generator<T>& a, const generator<T>& b) {return a.coro == b.coro;}

template <class T>
bool operator!=(const generator<T>& a, const generator<T>& b) {return !(a == b);}
