
CXX=g++
CXX=clang++

.PHONY: test_thread test_boost speedtest_thread speedtest_boost

test: test_thread test_boost speedtest_thread speedtest_boost

test_thread: test_thread.bin
	./test_thread.bin

speedtest_thread: speedtest_thread.bin
	./speedtest_thread.bin

test_boost: test_boost.bin
	./test_boost.bin

speedtest_boost: speedtest_boost.bin
	./speedtest_boost.bin

test_thread.bin: test.cpp *.hpp coroutine_thread.cpp
	$(CXX) -std=c++14 test.cpp coroutine_thread.cpp -pthread -o test_thread.bin

speedtest_thread.bin: test.cpp *.hpp coroutine_thread.cpp
	$(CXX) -std=c++14 -O2 speedtest.cpp coroutine_thread.cpp -pthread -o speedtest_thread.bin

test_boost.bin: test.cpp *.hpp coroutine_boost.cpp
	$(CXX) -std=c++14 test.cpp coroutine_boost.cpp -lboost_coroutine -o test_boost.bin

speedtest_boost.bin: test.cpp *.hpp coroutine_boost.cpp
	$(CXX) -std=c++14 -O2 speedtest.cpp coroutine_boost.cpp -lboost_coroutine -o speedtest_boost.bin

clean:
	rm *.bin
