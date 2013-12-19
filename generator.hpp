#pragma once

#include "fwd.hpp"
#include <memory>
#include "generator_impl.hpp"
#include "store.hpp"

template <class T>
class generator
{
	std::unique_ptr<generator_impl<T>> p;
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

	explicit operator bool() {return bool(p);}
};

template <class T>
generator<T>::generator(const generator_function<T>& gen)
	: p(new generator_impl<T>(gen))
{
	++(*this);
}

template <class T>
auto generator<T>::operator*() -> T&
{
	if (!p)
		throw std::out_of_range("generator::operator*");

	return *value;
}

template <class T>
auto generator<T>::operator*() const -> const T&
{
	if (!p)
		throw std::out_of_range("generator::operator*");

	return *value;
}

template <class T>
generator<T>& generator<T>::operator++()
{
	if (!p)
		throw std::out_of_range("generator::operator++");

	auto v = (*p)();
	if (!v)
		p = {};
	else
		value = std::forward<T>(*v);

	return *this;
}

template <class T>
bool operator==(const generator<T>& a, const generator<T>& b) {return a.p == b.p;}

template <class T>
bool operator!=(const generator<T>& a, const generator<T>& b) {return !(a == b);}
