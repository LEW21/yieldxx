#pragma once

#include <functional>
#include <memory>

#include "cxx_function.hpp"

namespace xx
{
	struct coroutine_impl;

	struct coroutine
	{
		using yield = std::function<void()>;
		using body  = __private::cxx_function::unique_function<void(yield&&) &&>;

		class stop: public std::exception {};

		coroutine();
		coroutine(body);

		coroutine(coroutine&&);
		coroutine& operator=(coroutine&&);
		~coroutine();

		bool operator()();
		explicit operator bool() const { return bool(p); }

	private:
		std::unique_ptr<coroutine_impl> p;
		friend bool operator==(const coroutine& a, const coroutine& b);
	};

	inline bool operator==(const coroutine& a, const coroutine& b) {return a.p == b.p;}
	inline bool operator!=(const coroutine& a, const coroutine& b) {return !(a == b);}
}
