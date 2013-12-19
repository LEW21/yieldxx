#pragma once

#include <functional>
#include <memory>

struct coroutine_impl;

struct coroutine
{
	using yield = std::function<void()>;
	using body  = std::function<void(yield&&)>;

	class stop: public std::exception {};

	coroutine(body&&);
	~coroutine();
	bool operator()();
	explicit operator bool() { return bool(p); }

private:
	std::unique_ptr<coroutine_impl> p;
};
