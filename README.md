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

* Add coroutine_boost.cpp (with -lboost_context) or coroutine_thread.cpp (with -pthread / whatever is your std::thread using) to your build system.

```c++
#include "yieldxx/generated.hpp"
```

## Requirements

* Fully C++14-compliant compiler. Tested on gcc 5.2 and clang 3.7.
* Boost backend: Boost.Context 1.58+

## Performance

Duration of 1 iteration on AMD Phenom II X4 920:

* Thread backend - 10377ns
* Boost backend - 71ns

(Computed by taking a 25%-trimmed mean of results of 20 runs of speedtest.cpp compiled by g++ -O2.)

Yes, Boost backend is 146 times faster.

Thread backend is so slow because... switching threads is slow :P Boost backend is implemented without threads, using Boost.Context instead. To jump from one coroutine to another, it simply jumps to another stack - which is A LOT faster than switching threads.

## Single pass vs multi pass generators
xx::generat**ed**<T> is designed for multi-pass generators - generators over which users can iterate multiple times (like range()). Therefore, this is a valid code:
```c++
auto r = range(0, 3);
for (auto i : r)
	std::cout << i << std::endl;
for (auto i : r)
	std::cout << i << std::endl;
```
and will print:
```
0
1
2
0
1
2
```

To guarantee this behavior, the generator function is required to be const - i.e. mutable lambdas are not accepted. Also, it's required to be copyable - as xx::generated is copyable.

If you need to modify the function during a pass, you can provide only a single-pass iterator - xx::generat**or**<T>. It's constructed from a unique_function<void(yield) &&> - which means the function is going to be called only once, as std::move(func)(yield); and that is never going to be copied, only moved. This way, you can store unique_ptrs and other movable, non-copyable objects in the function object; and the function object will be destroyed after the function finishes.
