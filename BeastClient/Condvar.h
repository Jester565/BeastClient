#pragma once
#include <mutex>

namespace bcli {
	class Condvar {
	public:
		Condvar(bool ready = false)
			:ready(ready)
		{
		}

		void wait() {
			std::unique_lock<std::mutex> lk(mu);
			cv.wait(lk, [&] {return ready; });
			ready = false;
		}

		void wait(std::chrono::milliseconds waitMillis) {
			std::unique_lock<std::mutex> lk(mu);
			cv.wait_for(lk, waitMillis, [&] {return ready; });
			ready = false;
		}

		void set() {
			std::lock_guard<std::mutex> lk(mu);
			ready = true;
			cv.notify_one();
		}

	private:
		std::condition_variable cv;
		std::mutex mu;
		bool ready;
	};
}