# yieldxx
Prettiest C++ generator/coroutine implementation.

```c++
#include <iostream>
#include "yieldxx/generated.hpp"

template <class T>
auto range(T min, T max) -> xx::generated<T>
{
	return xx::generated<T>([=](typename xx::generator<T>::yield&& yield) {
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

## Usage
```sh
git submodule add https://github.com/LEW21/yieldxx.git
```

* Add coroutine_boost.cpp (with -lboost_coroutine) or coroutine_thread.cpp (with -pthread / whatever is your std::thread using) to your build system.

```c++
#include "yieldxx/generated.hpp"
```

## Requirements

* Fully C++14-compliant compiler. Tested on gcc 5.2 and clang 3.7.

## Performance

Duration of 1 iteration on AMD Phenom II X4 920:

* Thread backend - 10377ns
* Boost backend - 71ns

(Computed by taking a 25%-trimmed mean of results of 20 runs of speedtest.cpp compiled by g++ -O2.)

Yes, Boost backend is 146 times faster.

Thread backend is so slow because... switching threads is slow :P Boost backend is implemented without threads, using Boost.Coroutine and Boost.Context instead. To jump from one coroutine to another, it simply jumps to another stack - which is A LOT faster than switching threads.
