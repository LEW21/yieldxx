#include "coroutine.hpp"

#include <exception>
#include <cassert>

#include <boost/coroutine/stack_context.hpp>
#include <boost/coroutine/stack_allocator.hpp>
#include <boost/coroutine/detail/coroutine_context.hpp>
#include <boost/coroutine/stack_traits.hpp>

using namespace boost::coroutines;
using boost::coroutines::detail::coroutine_context;

namespace xx
{
	struct stack_tuple
	{
		stack_context   ctx;
		stack_allocator alloc;

		stack_tuple()
		{ alloc.allocate(ctx, stack_traits::default_size()); }

		~stack_tuple()
		{ alloc.deallocate(ctx); }
	};

	struct coroutine_impl
	{
		coroutine_impl(coroutine::body);
		bool operator()();
		~coroutine_impl();

		void run();

	private:
		bool deleted = false;
		bool done = false;
		std::exception_ptr exception;

		stack_tuple stack;
		coroutine_context main;
		coroutine_context gen;
		coroutine::body body;
	};

	coroutine::coroutine()
	{}

	coroutine::coroutine(coroutine&& other)
		: p(std::move(other.p))
	{}

	coroutine::coroutine(coroutine::body f)
		: p(new coroutine_impl(std::move(f)))
	{}

	bool coroutine::operator()()
	{
		if (!p)
			throw std::out_of_range("coroutine::operator()");

		auto ok = (*p)();
		if (!ok)
			p = {};

		return ok;
	}

	coroutine::~coroutine()
	{}

	void trampoline(intptr_t vp);

	coroutine_impl::coroutine_impl(coroutine::body body)
		: body(std::move(body))
		, gen{trampoline, stack.ctx}
	{
	}

	// Run in callee.
	void trampoline(intptr_t vp)
	{
		reinterpret_cast<coroutine_impl*>(vp)->run();
	}

	// Run in callee.
	void coroutine_impl::run()
	{
		auto yield = [this]()
		{
			if (deleted)
				throw coroutine::stop();


			gen.jump(main, {}, true);

			if (deleted)
				throw coroutine::stop();
		};
		try
		{
			body(std::move(yield));
		}
		catch (coroutine::stop&) {}
		catch (...) { exception = std::current_exception(); }

		done = true;
		gen.jump(main, {}, true);
	}


	bool coroutine_impl::operator()()
	{
		assert(!done);
		// Passing this, as the trampoline expects it as a parameter. (Used only on the first call.)
		main.jump(gen, reinterpret_cast<intptr_t>(this), true);
		if (exception) std::rethrow_exception(exception);
		return !done;
	}

	coroutine_impl::~coroutine_impl()
	{
		deleted = true;
		while (!done)
			(*this)();
	}
}
