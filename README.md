cpp-yield
=========

Prettiest C++ generator/coroutine implementation.

```c++
#include <iostream>
#include <generated.hpp>

template <class T>
auto range(T min, T max) -> generated<T>
{
	return generated<T>([=](generator<T>::yield&& yield) {
		for (T i = min; i < max; ++i)
			yield(i);
	});
}

int main()
{
	for (auto i : range(0, 5))
		std::cout << i << std::endl;
}
```

Look at test.cpp for more features.

Requirements:
-------------

* Fully C++11-compliant compiler. It might work on not fully compliant, it might be ported to work on C++03 compilers, but... it's easier to find a decent compiler, and stop torturing yourself.

Performance:
------------

Duration of 1 iteration on AMD Phenom II X4 920:

* Thread backend - 10377ns
* Boost backend - 71ns

(Computed by taking a 25%-trimmed mean of results of 20 runs of speedtest.cpp compiled by g++ -O2.)

Yes, Boost backend is 146 times faster.

Thread backend is so slow because... switching threads is slow :P Boost backend is implemented without threads, using Boost.Coroutine and Boost.Context instead. To jump from one coroutine to another, it simply jumps to another stack - which is A LOT faster than switching threads.
