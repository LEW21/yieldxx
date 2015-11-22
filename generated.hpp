#pragma once

#include "generator.hpp"

namespace xx
{
	template <class T>
	class generated
	{
	public:
		using generator = xx::generator<T>;

		typename generator::body gen;

		generated(typename generator::body g): gen(std::move(g)) {}

		operator typename generator::body() {return gen;}

		generator begin() const {return generator(gen);}
		generator end() const {return generator();}
	};

	template <class T>
	int operator<<(typename generator<T>::yield& yield, const generated<T>& gen)
	{
		int yielded = 0;
		gen.gen([&](const T& v)
		{
			yield(v);
			++yielded;
		});
		return yielded;
	}
}
