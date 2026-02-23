#include <algorithm>
#include <chrono>

#include "clock.hpp"

namespace Backend {

	void Clock::restart() {
		last_time = std::chrono::high_resolution_clock::now();
	}

	Clock::Clock() {
		Clock::restart();
	}

	double Clock::getTimeDelta() const {
		std::chrono::duration<double> ret = (std::chrono::high_resolution_clock::now() - last_time);
		return ret.count();
	}

}
