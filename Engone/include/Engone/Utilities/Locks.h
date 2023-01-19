#pragma once

namespace engone {
	class Semaphore {
	public:
		// Do research about what semaphores are if you don't know what initial or max count is.
		Semaphore(int initial=1, int max=1);
		~Semaphore();
		void cleanup();

		void wait();
		// count is how much to increase the semaphores count by.
		void signal(int count=1);

	private:
		HANDLE m_handle = NULL;
		int m_initial = 1;
		int m_max = 1;
	};
	class Mutex {
	public:
		Mutex() = default;
		~Mutex();
		void cleanup();

		void lock();
		void unlock();

		uint32_t getOwner();
	private:
		uint32_t m_ownerThread = 0;
		HANDLE m_handle = NULL;
	};
	class DepthMutex {
	public:
		DepthMutex() = default;
		~DepthMutex();
		void cleanup();

		void lock();
		void unlock();

		uint32_t getOwner();
	private:
		uint32_t m_ownerThread = 0;
		uint32_t m_depth=0;
		HANDLE m_handle = NULL;
	};
	// multiple readers, one writer
	// this lock is not flawless.
	// if readers constantly read then the writer will never have an opportunity
	class ReadWriteLock {
	public:
		ReadWriteLock();
		~ReadWriteLock();
		void cleanup();
		
		// lock as reader
		void lockReader();
		// lock as writer
		void lockWriter();
		void unlockReader();
		void unlockWriter();

	private:
		Semaphore m_semaphore;
		Mutex m_mutex;
		int m_readers = 0;
	};
}