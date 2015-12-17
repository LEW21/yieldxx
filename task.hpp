#pragma once

#include "coroutine.hpp"

namespace xx
{
	struct task
	{
		using body = __private::cxx_function::unique_function<void(xx::task&&) &&>;

		static void spawn(body f);

		void yield() {y();}

		// When false is returned, the task is done, and it's object does not exist anymore.
		bool resume()
		{
			auto coro_ptr_copy = coro;
			return (*coro_ptr_copy)();
		}

		explicit operator bool() {return bool(coro);}

	private:
		coroutine::yield y;
		std::shared_ptr<coroutine> coro;

		task(std::shared_ptr<coroutine>&&, coroutine::yield&&);

		friend bool operator==(const task& a, const task& b);
	};

	inline void spawn_task(task::body body) { task::spawn(std::move(body)); }

	template <class T>
	bool operator==(const task& a, const task& b) {return a.coro == b.coro;}

	template <class T>
	bool operator!=(const task& a, const task& b) {return !(a == b);}

	inline task::task(std::shared_ptr<coroutine>&& coro, coroutine::yield&& yield)
		: y{std::move(yield)}
		, coro{std::move(coro)}
	{}

	inline void task::spawn(task::body b)
	{
		auto coro = std::make_shared<coroutine>();
		*coro = coroutine{[coro, b = std::move(b)](coroutine::yield&& yield) mutable{
			std::move(b)(task{std::move(coro), std::move(yield)});
		}};
		(*coro)();
	}
}
