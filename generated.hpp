#pragma once

#include "generator.hpp"

namespace xx
{
	template <class T>
	class generated
	{
	public:
		using generator = xx::generator<T>;
		using yield = std::function<void(T)>;
		using body  = __private::cxx_function::function<void(yield&&) const>;

		body gen;

		generated(body g): gen(std::move(g)) {}

		operator body() {return gen;}

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
