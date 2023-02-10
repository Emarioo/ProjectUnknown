#include "Engone/Utilities/Locks.h"
#include "Engone/Logger.h"

#include "Engone/Utilities/Thread.h"

#ifdef gone

namespace engone {
	Semaphore::Semaphore(int initial, int max) {
		m_initial = initial;
		m_max = max;
	}
	Semaphore::~Semaphore() {
		cleanup();
	}
	void Semaphore::cleanup() {
		if (m_handle != NULL)
			CloseHandle(m_handle);
	}
	void Semaphore::wait() {
		if (m_handle == NULL) {
			m_handle = CreateSemaphore(NULL, m_initial, m_max, NULL);
			if (m_handle == NULL) {
				int err = GetLastError();
				log::out << log::RED << "Semaphore : Win Error " << err << "\n";
				return;
			}
		}
		if (m_handle != NULL) {
			DWORD res = WaitForSingleObject(m_handle, INFINITE);
			if (res == WAIT_FAILED) {
				int err = GetLastError();
				log::out << log::RED << "Semaphore : Win Error " << err << "\n";
			}
		}
	}
	void Semaphore::signal(int count) {
		if (m_handle != NULL) {
			bool yes = ReleaseSemaphore(m_handle, count, NULL);
			if (!yes) {
				int err = GetLastError();
				log::out << log::RED << "Semaphore : Win Error " << err << "\n";
				return;
			}
		}
	}
	Mutex::~Mutex() {
		cleanup();
	}
	void Mutex::cleanup() {
		if (m_handle != NULL) {
			m_handle = CreateMutex(NULL, false, NULL);
			if (m_handle == NULL) {
				int err = GetLastError();
				log::out << log::RED << "Mutex : Win. Error " << err << "\n";
				return;
			}
		}
		if (m_handle != NULL)
			CloseHandle(m_handle);
	}
	void Mutex::lock() {
		if (m_handle == NULL) {
			m_handle = CreateMutex(NULL, false, NULL);
			if (m_handle == NULL) {
				int err = GetLastError();
				log::out << log::RED << "Mutex : Win Error " << err << "\n";
				return;
			}
		}
		if (m_handle != NULL) {
			DWORD res = WaitForSingleObject(m_handle, INFINITE);
			uint32_t newId = Thread::GetThisThreadId();
			if (m_ownerThread != 0) {
				log::out << "Mutex : Locking twice, old owner: " << m_ownerThread << ", new owner: " << newId << "\n";
			}
			m_ownerThread = newId;
			if (res == WAIT_FAILED) {
				int err = GetLastError();
				log::out << log::RED << "Mutex : Win Error " << err << "\n";
			}
		}
	}
	void Mutex::unlock() {
		if (m_handle != NULL) {
			m_ownerThread = 0;
			bool yes = ReleaseMutex(m_handle);
			if (!yes) {
				int err = GetLastError();
				log::out << log::RED << "Mutex : Win Error " << err << "\n";
				return;
			}
		}
	}
	uint32_t Mutex::getOwner() {
		return m_ownerThread;
	}
	DepthMutex::~DepthMutex() {
		cleanup();
	}
	void DepthMutex::cleanup() {

	}
	void DepthMutex::lock() {
		if (m_handle == NULL) {
			m_handle = CreateMutex(NULL, false, NULL);
			if (m_handle == NULL) {
				int err = GetLastError();
				log::out << log::RED << "DepthMutex : Win Error " << err << "\n";
				return;
			}
		}
		if (m_handle == NULL) return;

		DWORD res = WaitForSingleObject(m_handle, INFINITE);
		uint32_t newId = Thread::GetThisThreadId();

		m_ownerThread = newId;
		if (res == WAIT_FAILED) {
			int err = GetLastError();
			log::out << log::RED << "DepthMutex : Win Error " << err << "\n";
		} else {
			m_depth++; // should not be done if m_mutex failed

			if (m_depth > 4)
				log::out << log::YELLOW<< "DepthMutex : Depth is at " << m_depth << ". Is it a bug?\n";
		}
	}
	void DepthMutex::unlock() {
		if (m_handle == NULL) return;

		m_depth--;
		if (m_depth != 0) return;

		m_ownerThread = 0;
		bool yes = ReleaseMutex(m_handle);
		if (!yes) {
			int err = GetLastError();
			log::out << log::RED << "DepthMutex : Win Error " << err << "\n";
			return;
		}
	}
	uint32_t DepthMutex::getOwner() {
		return m_ownerThread;
	}
	ReadWriteLock::ReadWriteLock() : m_semaphore(1, 1) {

	}
	ReadWriteLock::~ReadWriteLock() {
		cleanup();
	}
	void ReadWriteLock::cleanup() {

	}
	void ReadWriteLock::lockReader() {
		m_mutex.lock();
		if (m_readers == 0)
			m_semaphore.wait();
		m_readers++;
		m_mutex.unlock();
	}
	void ReadWriteLock::lockWriter() {
		m_semaphore.wait();
	}
	void ReadWriteLock::unlockReader() {
		m_semaphore.signal();
	}
	void ReadWriteLock::unlockWriter() {
		m_mutex.lock();
		m_readers--;
		m_semaphore.signal();
		m_mutex.unlock();
	}
}
#endif