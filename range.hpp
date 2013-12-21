#pragma once

#include "generated.hpp"

// [min, max) - like in Python
template <class T>
auto range(T min, T max) -> generated<T>
{
	return generated<T>([=](typename generator<T>::yield&& yield) {
		for (T i = min; i < max; ++i)
			yield(i);
	});
}

template <class T, class TStep>
auto range(T min, T max, TStep step) -> generated<T>
{
	return generated<T>([=](typename generator<T>::yield&& yield) {
		for (T i = min; (step > 0) ? i < max : i > max; i += step)
			yield(i);
	});
}
