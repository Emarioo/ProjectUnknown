#include "Engone/Utilities/Locks.h"
#include "Engone/Logger.h"

#include "Engone/Utilities/Thread.h"

namespace engone {
	Semaphore::Semaphore(int initial, int max) {
		m_initial = initial;
		m_max = max;
	}
	Semaphore::~Semaphore() {
		cleanup();
	}
	void Semaphore::cleanup() {
		if(m_handle!=NULL)
			CloseHandle(m_handle);
	}
	void Semaphore::wait() {
		if (m_handle == NULL) {
			m_handle = CreateSemaphore(NULL, 0, 10, NULL);
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
		// GetLastError?
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
		// GetLastError?
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
			m_handle = CreateMutex(NULL,false,NULL);
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
		uint32_t thisThread = Thread::GetThisThreadId();
		m_depthMutex.lock();
		if (thisThread != m_mutex.getOwner()) {
			m_depthMutex.unlock();
			m_mutex.lock();
			m_depthMutex.lock();
			m_depth++;
			m_depthMutex.unlock();
		} else {
			m_depth++;
			m_depthMutex.unlock();
		}
	}
	void DepthMutex::unlock() {
		uint32_t thisThread = Thread::GetThisThreadId();
		m_depthMutex.lock();
		if (thisThread == m_mutex.getOwner()) {
			m_depth--;
			if (m_depth == 0)
				m_mutex.unlock();
		} else {
			log::out << log::RED << "DepthMutex : Thread " << thisThread << " tried to unlock mutex owned by thread " << m_mutex.getOwner() << "\n";
		}
		m_depthMutex.unlock();
	}
	uint32_t DepthMutex::getOwner() {
		return m_mutex.getOwner();
	}
	ReadWriteLock::ReadWriteLock() : m_semaphore(1,1) {

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