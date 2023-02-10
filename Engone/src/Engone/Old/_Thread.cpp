#include "Engone/Utilities/Thread.h"
#include "Engone/Logger.h"

namespace engone {
	Thread::~Thread() {
		cleanup();
	}

	void Thread::cleanup() {
		if (m_handle) {
			if (m_threadId == GetThisThreadId()) {
				assert((false, "Thread : Thread cannot clean itself"));
				return;
			}
			CloseHandle(m_handle);
			m_handle=NULL;
		}
	}
	void Thread::init(std::function<uint32_t(void*)> func, void* arg) {
		if (!m_handle) {
			//const uint32_t stackSize = pow(2, 20); // 1 MB
			m_handle = CreateThread(NULL, 0, (DWORD (*)(void*))&func, arg, 0,(DWORD*)&m_threadId);
			if (!m_handle) {
				int err = GetLastError();
				log::out << "Thread : Win. Error " << err << "\n";
			}

		}
	}
	void Thread::join() {
		if (!m_handle)
			return;
		int yes = WaitForSingleObject(m_handle, INFINITE);
		CloseHandle(m_handle);
		m_handle = NULL;
		if (!yes) {
			int err = GetLastError();
			log::out << "Thread : Win. Error " << err << "\n";
		}
	}
	bool Thread::isRunning() {
		return m_handle!=NULL;
	}
	ThreadId Thread::getId() {
		return m_threadId;
	}
	ThreadId Thread::GetThisThreadId() {
		return GetCurrentThreadId();
	}

}