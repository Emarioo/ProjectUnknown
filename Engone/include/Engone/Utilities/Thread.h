#pragma once

namespace engone {

	class Thread {
	public:
		Thread() = default;
		~Thread();
		// The thread itself should not call this function
		void cleanup();

		void init(std::function<uint32_t(void*)> func, void* arg);
		void join();

		bool isRunning();

		static uint32_t GetThisThreadId();

		uint32_t getId();

	private:
		HANDLE m_handle=NULL;
		uint32_t m_threadId=0;
	};

	class ThreadPool {
	public:

	};
}