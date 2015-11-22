#pragma once

namespace xx
{
	template <class T>
	class store_t
	{
		T _v;
	public:
		store_t() {}
		store_t(T&& v): _v(std::move(v)) {}
		T& operator*() {return _v;}
	};

	template <class T>
	class store_t<T&>
	{
		T* _v;
	public:
		store_t() {}
		store_t(T& v): _v(&v) {}
		T& operator*() {return *_v;}
	};
}
