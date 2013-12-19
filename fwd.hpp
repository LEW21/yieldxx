#pragma once

#include <functional>

template <class T> using yield_t = std::function<void (T)>;
template <class T> using generator_function = std::function<void (yield_t<T>&&)>;

template <class T> class generator;

class generator_stop: public std::exception {};
