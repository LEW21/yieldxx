#pragma once

#include <functional>
#include <memory>

namespace xx
{
	struct coroutine_impl;

	struct coroutine
	{
		using yield = std::function<void()>;
		using body  = std::function<void(yield&&)>;

		class stop: public std::exception {};

		coroutine();
		coroutine(coroutine&& other);

		coroutine(body);
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
