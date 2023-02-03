#pragma once

namespace engone {

	typedef uint32 ThreadId;
	class Thread {
	public:
		Thread() = default;
		~Thread();
		// The thread itself should not call this function
		void cleanup();

		void init(std::function<uint32_t(void*)> func, void* arg);
		void join();

		bool isRunning();

		static ThreadId GetThisThreadId();

		ThreadId getId();

	private:
		HANDLE m_handle=NULL;
		ThreadId m_threadId=0;
	};

	class ThreadPool {
	public:

	};
}