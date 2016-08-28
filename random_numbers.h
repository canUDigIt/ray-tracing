#ifndef RANDOM_NUMBERS_H
#define RANDOM_NUMBERS_H

#include <random>
#include <chrono>

namespace {
		template<class T>
		T uniform_random() {
				static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
				static std::uniform_real_distribution<T> distribution(0, 1);
				return distribution( gen );
		}
}

#endif // RANDOM_NUMBERS_H
