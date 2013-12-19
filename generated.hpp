#pragma once

#include "generator.hpp"

template <class T>
class generated
{
public:
	generator_function<T> gen;

	generated(const generator_function<T>& g): gen(g) {}
	generated(generator_function<T>&& g): gen(std::move(g)) {}

	operator generator_function<T>() {return gen;}

	generator<T> begin() const {return generator<T>(gen);}
	generator<T> end() const {return generator<T>();}
};

template <class T>
int operator<<(yield_t<T>& yield, const generated<T>& gen)
{
	int yielded = 0;
	gen.gen([&](const T& v)
	{
		yield(v);
		++yielded;
	});
	return yielded;
}
