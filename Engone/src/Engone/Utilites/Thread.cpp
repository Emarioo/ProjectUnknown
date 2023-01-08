#include "Engone/Utilities/Thread.h"
#include "Engone/Logger.h"

namespace engone {
	Thread::~Thread() {
		cleanup();
	}

	void Thread::cleanup() {
		if (m_handle)
			CloseHandle(m_handle);
	}
	void Thread::init(std::function<uint32_t(void*)> func, void* arg) {
		if (!m_handle) {
			const uint32_t stackSize = pow(2, 20); // 1 MB
			m_handle = CreateThread(NULL, stackSize, (DWORD (*)(void*))&func, arg, 0,(DWORD*)&m_threadId);
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
		if (!yes) {
			int err = GetLastError();
			log::out << "Thread : Win. Error " << err << "\n";
		}
	}
	uint32_t Thread::getId() {
		return m_threadId;
	}

}