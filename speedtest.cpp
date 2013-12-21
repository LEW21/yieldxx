#include "range.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main()
{
	// warmup
	for (auto n : range(0, 1000)) {}

	auto r = range(0, 100000);

	auto pre = steady_clock::now();
	for (auto n : r) {}
	auto post = steady_clock::now();

	auto elapsed = duration_cast<duration<double, milli>>(post - pre);
	cout << "100 000 iterations in: " << elapsed.count() << "ms" << endl;

	cout << "Single iteration in: " << (duration_cast<duration<double, nano>>(elapsed)/100000).count() << "ns" << endl;
}
