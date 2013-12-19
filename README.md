cpp-yield
=========

Prettiest C++ generator/coroutine implementation.

```c++
#include <iostream>
#include <generated.hpp>

template <class T>
auto range(T min, T max) -> generated<T>
{
	return generated<T>([=](yield_t<T>&& yield) {
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

Requirements:
-------------

* Fully C++11-compliant compiler. It might work on not fully compliant, it might be ported to work on C++03 compilers, but... it's easier to find a decent compiler, and stop torturing yourself.

Problems:
---------

* It's implemented with threads. Yes, really. I said prettiest, not fastest. Want speed? Use boost.coroutine, or implement your own fiber support - it can be done without changing the API.
