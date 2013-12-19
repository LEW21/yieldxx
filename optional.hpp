#pragma once

template <class T>
class optional
{
	bool notempty;
	store_t<T> _v;

public:
	optional(): notempty{false} {}
	template <class U>
	optional(U&& v): notempty{true}, _v{std::forward<U>(v)} {}

	explicit operator bool() {return notempty;}

	T& operator*() {return *_v;}
};
