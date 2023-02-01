#include "Engone/Core/ExecutionControl.h"

#include "Engone/Logger.h"

namespace engone {
	Execution::Execution(std::string name, int priority) {
		m_executionName = name;
		m_priority = priority;
	}
	void Execution::setExecutionPriority(int priority) {
		m_priority = priority;
	}
	int Execution::getExecutionPriority() {
		return m_priority;
	}
	const std::string Execution::getExecutionName() {
		return m_executionName;
	}
	void ExecutionTimer::step() {
		now = engone::GetSystemTime();
		if (startTime == 0)
			startTime = now;
		if (lastTime == 0)
			delta = 1 / 60.f;
		else
			delta = now - lastTime;
		lastTime = now;

		accumulator += delta;
		runtime += delta;

		// Prevent accumulator from building up to much. If you were to accumulate 60 seconds while in debug mode then
		// the update function would run 60 * 60 = 3600 times.
		const int limit = 3;
		if (accumulator >= aimedDelta*limit)
			accumulator = aimedDelta*limit;
	}
	bool ExecutionTimer::accumulate() {
		if (aimedDelta <= 0)
			aimedDelta = 1;
		if (accumulator < aimedDelta)
			return false;
		accumulator -= aimedDelta;
		if (accumulator < 0) {
			DebugBreak();
		}
		return true;
	}
	ExecutionControl::~ExecutionControl() {
		cleanup();
	}
	void ExecutionControl::cleanup() {

	}
	void ExecutionControl::addExecution(Execution* exec) {
		m_mutex.lock();
		auto find = m_quickAccess.find(exec->getExecutionName());
		if (find != m_quickAccess.end()) {
			m_mutex.unlock();
			log::out << log::RED << "ExecutionControl : " << exec->getExecutionName() << " already exists\n";
			return;
		}
		m_quickAccess[exec->getExecutionName()] = exec;
		m_executions.push_back(exec);
		// executionIndex does not need to be modified
		m_mutex.unlock();
	}
	Execution* ExecutionControl::getExecution(const std::string& name) {
		m_mutex.lock();
		auto find = m_quickAccess.find(name);
		Execution* exec = nullptr;
		if (find != m_quickAccess.end()) {
			exec = find->second;
		}
		m_mutex.unlock();
		return exec;
	}
	void ExecutionControl::removeExecution(const std::string& name) {
		Execution* exec = getExecution(name);
		if(exec)
			removeExecution(exec);
	}
	void ExecutionControl::removeExecution(Execution* execution) {
		m_mutex.lock();
		for (int i = 0; i < m_executions.size();i++) {
			Execution* exec = m_executions[i];
			if (exec == execution) {
				m_executions.erase(m_executions.begin() + i);
				if (m_executionIndex > i)
					m_executionIndex--;
				break;
			}
		}
		m_mutex.unlock();
	}
	void ExecutionControl::execute(LoopInfo& info, int type) {
		sort();
		
		m_mutex.lock();
		m_executionIndex = 0; // should already be zero but in case it's not.
		m_mutex.unlock();

		while (true) { // loop through each execution
			m_mutex.lock();
			if (m_executionIndex >= m_executions.size()) {
				m_executionIndex = 0;
				m_mutex.unlock();
				break;
			}
			Execution* exec = m_executions[m_executionIndex];
			m_executionIndex++;
			m_mutex.unlock();
			if(type==UPDATE)
				exec->render(info);
			if(type==RENDER)
				exec->render(info);
		}
	}
	void ExecutionControl::sort() {
		// Todo: If sort takes long time then this would block update/render functions.
		//		A solution is to copy the exec. list and sort it then replace the old one.
		//		Calls to remove needs take affect while sorting or after sorting is done. Some mutexes and queues may be required. 
		m_mutex.lock();
		for (int i = 0; i < (int)m_executions.size() - 1; i++) {
			bool sorted = true;
			for (int j = 0; j < (int)m_executions.size() - 1 - i; j++) {
				Execution* a0 = m_executions[j];
				Execution* a1 = m_executions[j + 1];
				if (a0->m_priority < a1->m_priority) {
					m_executions[j + 1] = a0;
					m_executions[j] = a1;
					sorted = false;
				}
			}
			if (sorted)
				break;
		}
		m_mutex.unlock();
	}
}