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

Problems:
---------

* None.
