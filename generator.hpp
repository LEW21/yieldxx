#pragma once

#include <memory>
#include "coroutine.hpp"
#include "store.hpp"
#include <functional>

namespace xx
{
	template <class T>
	struct generator
	{
		using yield = std::function<void(T)>;
		using body  = std::function<void(yield&&)>;

		generator() = default;
		generator(body gen);

		auto operator*() -> T&;
		auto operator*() const -> const T&;
		auto operator->() -> typename std::remove_reference<T>::type* {return &**this;}
		auto operator->() const -> const typename std::remove_reference<T>::type* {return &**this;}

		generator& operator++() { coro(); return *this; }

		explicit operator bool() {return bool(coro);}

	private:
		std::unique_ptr<store_t<T>> value;
		coroutine coro;

		template <class U>
		friend bool operator==(const generator<U>& a, const generator<U>& b);
	};

	template <class T>
	generator<T>::generator(generator<T>::body gen)
		: value{std::make_unique<store_t<T>>()}
		, coro{[value = &*value, gen](coroutine::yield&& yield) {
			gen([value, yield](T&& v){
				*value = std::forward<T>(v);
				yield();
			});
		}}
	{
		coro();
	}

	template <class T>
	auto generator<T>::operator*() -> T&
	{
		if (!coro)
			throw std::out_of_range("generator::operator*");

		return **value;
	}

	template <class T>
	auto generator<T>::operator*() const -> const T&
	{
		if (!coro)
			throw std::out_of_range("generator::operator*");

		return **value;
	}

	template <class T>
	bool operator==(const generator<T>& a, const generator<T>& b) {return a.coro == b.coro;}

	template <class T>
	bool operator!=(const generator<T>& a, const generator<T>& b) {return !(a == b);}
}
