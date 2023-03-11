// #include "Engone/Rendering/RenderPipeline.h"

// namespace engone {
// 	RenderComponent::RenderComponent(const std::string& name, int priority)
// 		: m_name(name), m_priority(priority) {}

// 	// add getter & setter
// 	void RenderComponent::setName(const std::string& name) {
// 		m_name = name;
// 	}
// 	void RenderComponent::setPriority(int priority) {
// 		m_priority = priority;
// 	}
// 	const std::string& RenderComponent::getName() { return m_name; }
// 	int RenderComponent::getPriority() { return m_priority; }

// 	void RenderPipeline::forceAddComponent(RenderComponent* component) {
// 		m_components.push_back(component);
// 		auto find = m_quickAccess.find(component->getName());
// 		if (find == m_quickAccess.end()) {
// 			m_quickAccess[component->getName()] = component;
// 		} else {
// 			// don't add to map because it would override it
// 		}
// 	}
// 	void RenderPipeline::addComponent(RenderComponent* component, bool direct) {
// 		// watch out for multithreading?
// 		if (direct) {
// 			if (m_componentMutex.try_lock()) {
// 				forceAddComponent(component);
// 				m_componentMutex.unlock();
// 			}
// 		}
// 		m_queueMutex.lock();
// 		m_queue.push_back(component);
// 		m_queueMutex.unlock();
// 	}
// 	RenderComponent* RenderPipeline::removeAction(const std::string& name) {
// 		// how does engine know that action is owned by it
// 		log::out << log::RED << "REMOVE ACTION NOT IMPLEMENTED\n";
// 		return nullptr;
// 	}
// 	RenderComponent* RenderPipeline::getComponent(const std::string& name) {
// 		auto find = m_quickAccess.find(name);
// 		if (find == m_quickAccess.end()) {
// 			return nullptr;
// 		}else{
// 			return find->second;
// 		} 
// 	}
// 	std::vector<RenderComponent*> RenderPipeline::getComponents(const std::string& name) {
// 		std::vector<RenderComponent*> components;
// 		for (int i = 0; i < m_components.size(); i++) {
// 			if (m_components[i]->m_name == name)
// 				m_components.push_back(components[i]);
// 		}
// 		return components;
// 	}
// 	void RenderPipeline::applyChanges() {
// 		m_queueMutex.lock();
// 		std::vector<RenderComponent*> queue = std::move(m_queue);
// 		m_queue.clear();
// 		m_queueMutex.unlock();
// 		if (queue.size() != 0) {
// 			m_componentMutex.lock();
// 			for (auto comp : queue) {
// 				forceAddComponent(comp);
// 			}
// 			m_componentMutex.unlock();
// 		}
// 	}
// 	void RenderPipeline::render(LoopInfo& info) {
// 		applyChanges();

// 		//-- Bubble Sort (change it?)
// 		for (int i = 0; i < (int)m_components.size() - 1; i++) {
// 			for (int j = 0; j < (int)m_components.size() - 1 - i; j++) {
// 				RenderComponent* a0 = m_components[j];
// 				RenderComponent* a1 = m_components[j + 1];
// 				if (a0->m_priority < a1->m_priority) {
// 					m_components[j + 1] = a0;
// 					m_components[j] = a1;
// 				}
// 			}
// 		}

// 		//-- Render
// 		for (int i = 0; i < m_components.size(); i++) {
// 			RenderComponent* action = m_components[i];
// 			action->render(info);
// 		}
// 	}
// 	const std::vector<RenderComponent*>& RenderPipeline::getList() {
// 		return m_components;
// 	}
// 	static RenderPipeline* s_activePipeline = nullptr;
// 	void RenderPipeline::setActivePipeline() {
// 		s_activePipeline = this;
// 	}
// 	RenderPipeline* GetActivePipeline() {
// 		return s_activePipeline;
// 	}
// 	RenderPipeline::~RenderPipeline() {
// 		if (s_activePipeline == this)
// 			s_activePipeline = nullptr;
// 	}
// }