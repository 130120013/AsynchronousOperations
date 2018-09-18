#include <iostream>
#include <future>
#include <thread>
#include <limits>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <iterator>
#include <type_traits>

#define ARRWIDTH 512
#define ARRHEIGHT 100

using namespace std;

template <class Iterator>
auto Min(Iterator arrBegin, Iterator arrEnd)
{
	auto tempMin = *(arrBegin);

	for (auto it = arrBegin; it < arrEnd; ++it)
	{
		if (*it < tempMin)
			tempMin = *it;
	}
	return tempMin;
}

template <class Iterator>
auto minTask(Iterator arrBegin, Iterator arrEnd)
{
	using value_type = typename std::iterator_traits<Iterator>::value_type;
	constexpr auto PARALLEL_THRESHOLD = 10;
	int threadCount = std::thread::hardware_concurrency();
	if (arrEnd - arrBegin < PARALLEL_THRESHOLD || threadCount < 2)
		return Min<Iterator>(arrBegin, arrEnd);
	std::vector<std::future<value_type>> futures;
	futures.reserve(threadCount);
	int modulo = (arrEnd - arrBegin) % threadCount; //остаток
	auto i = arrBegin;

	while (i < arrEnd)
	{
		int part = (arrEnd - arrBegin) / threadCount; //сколько элементов в одном потоке
		if (modulo != 0)
			part += 1;
		auto iEnd = i + part;

			futures.emplace_back(std::async(std::launch::async, Min<Iterator>, i, iEnd));

		if (modulo != 0)
			modulo -= 1;
		i += part;
	}

	std::vector<value_type> results;
	results.reserve(futures.size());
	for (auto& fut : futures)
		results.emplace_back(fut.get());
	return minTask(results.begin(), results.end());
}

int main() {
	auto arrsize = ARRHEIGHT * ARRWIDTH;
	double* heightmatrix = new double[arrsize];

	for (int k = 0; k < arrsize; ++k)
	{
		(k % 3 == 0) ? heightmatrix[k] = 2 * k - 6 : heightmatrix[k] = -1 * k + 4;
	}

	auto u = minTask(heightmatrix, heightmatrix + arrsize);
	std::cout << "\n__________\n" << u << "\n__________\n";

	for (int l = 0; l < ARRHEIGHT; ++l)
	{
		for (int k = 0; k < ARRWIDTH; ++k)
		{
			cout << heightmatrix[l * ARRWIDTH + k] << " ";
		}
		std::cout << "\n";
	}
	system("pause");
	return 0;
}