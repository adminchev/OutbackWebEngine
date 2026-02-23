#ifndef BACKEND_CLOCK
#define BACKEND_CLOCK

#include <chrono>

namespace Backend {
	class Clock {		
		private:
			std::chrono::time_point<std::chrono::high_resolution_clock> last_time;

		public:
			Clock();
			void restart(void);
			double getTimeDelta(void) const;
	};
}

#endif
