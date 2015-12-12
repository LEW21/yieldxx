#include "coroutine.hpp"

#include <exception>
#include <cassert>

#include <boost/context/all.hpp>

namespace xx
{
	struct coroutine_context
	{
		using ctx_fn = void(void*);
		using ctx_fn_intptr = void(intptr_t);

		boost::context::stack_context sctx;
		boost::context::fcontext_t    fctx = nullptr;

		coroutine_context() = default;

		coroutine_context(ctx_fn* fn, boost::context::stack_context sctx)
			: sctx(sctx)
			, fctx(boost::context::make_fcontext(sctx.sp, sctx.size, (ctx_fn_intptr*) fn))
		{}

		void* jump(coroutine_context& other, void* param = 0)
		{
			#if defined(BOOST_USE_SEGMENTED_STACKS)
			__splitstack_getcontext(sctx.segments_ctx);
			__splitstack_setcontext(other.sctx.segments_ctx);
			#endif
			return (void*) boost::context::jump_fcontext(&fctx, other.fctx, (intptr_t) param);
		}
	};

	struct stack_tuple
	{
		# if defined(BOOST_USE_SEGMENTED_STACKS)
		using stack_allocator = boost::context::segmented_stack;
		#else
		using stack_allocator = boost::context::fixedsize_stack;
		#endif

		stack_allocator alloc;
		boost::context::stack_context ctx;

		stack_tuple()
			: ctx(alloc.allocate())
		{}

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

	coroutine& coroutine::operator=(coroutine&& other)
	{
		p = std::move(other.p);
		return *this;
	}

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

	void trampoline(void* vp);

	coroutine_impl::coroutine_impl(coroutine::body body)
		: body(std::move(body))
		, gen{trampoline, stack.ctx}
	{
	}

	// Run in callee.
	void trampoline(void* vp)
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


			gen.jump(main, {});

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
		gen.jump(main, {});
	}


	bool coroutine_impl::operator()()
	{
		assert(!done);
		// Passing this, as the trampoline expects it as a parameter. (Used only on the first call.)
		main.jump(gen, this);
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
