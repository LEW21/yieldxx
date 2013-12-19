#pragma once

#include "fwd.hpp"
#include <memory>
#include "generator_impl.hpp"

template <class T>
class generator
{
	std::unique_ptr<generator_impl<T>> p;

	template <class U>
	friend bool operator==(const generator<U>& a, const generator<U>& b);

public:
	generator() = default;
	generator(const generator_function<T>& gen);

	T& operator*() const;
	typename std::remove_reference<T>::type* operator->() const {return &**this;}

	generator& operator++();
};

template <class T>
generator<T>::generator(const generator_function<T>& gen)
{
	bool not_empty;
	p.reset(new generator_impl<T>(gen, not_empty));
	if (!not_empty)
		p = {};
}

template <class T>
T& generator<T>::operator*() const
{
	if (!p)
		throw std::out_of_range("generator::operator*");

	return p->get();
}

template <class T>
generator<T>& generator<T>::operator++()
{
	if (!p)
		throw std::out_of_range("generator::operator++");

	if (!p->next())
		p = {};

	return *this;
}

template <class T>
bool operator==(const generator<T>& a, const generator<T>& b) {return a.p == b.p;}

template <class T>
bool operator!=(const generator<T>& a, const generator<T>& b) {return !(a == b);}
