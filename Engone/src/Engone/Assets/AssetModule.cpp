#include "Engone/AssetModule.h"

//#define ENGONE_DEBUG(x) x
#define ENGONE_DEBUG(x)

namespace engone {
	
	AssetStorage::AssetStorage() {
		m_running = true;
	}
	AssetStorage::~AssetStorage() {
		m_running = false;
		cleanup();
	}
	void AssetStorage::cleanup() {
		ENGONE_DEBUG(log::out << "AssetStorage::cleanup - begun\n";)
		m_ioProcMutex.lock();
		for (int i = 0; i < m_ioProcessors.size(); i++) {
			delete m_ioProcessors[i];
		}
		m_ioProcessors.clear();
		m_ioProcMutex.unlock();

		m_procMutex.lock();
		for (int i = 0; i < m_processors.size(); i++) {
			delete m_processors[i];
		}
		m_processors.clear();
		m_procMutex.unlock();

		if (m_glProcessor) {
			m_glProcMutex.lock();
			delete m_glProcessor;
			m_glProcessor = nullptr;
			m_glProcMutex.unlock();
		}

		m_assetMutex.lock();
		for (auto& map : m_assets) {
			for (auto& pair : map.second) {
				GetTracker().untrack(Asset::GetTrackerId(pair.second->type), pair.second);
				//GetTracker().untrack(, pair.second);
				delete pair.second; // ISSUE: BIG ISSUE THIS WILL NOT DELETE OPENGL BUFFERS UNLESS CLEANUP IS DONE ON THE RENDER THREAD!
			}
		}
		m_assets.clear();
		// Possible fix is to make a vector of assets which needs context and give it to render thread somehow.
		// Storage can then be cleaned properly. Problems may arise if a model asset want to delete collider assets. but they would already have been deleted
		// if they existed on here. So then, all assets which was created by model assets will not be deleted here. Ownership system for all assets.
		// Most owned by asset storage.
		// When window is deleted, the context is deleted and so the buffers.
		// But say you want to only clean AssetStorage without destroying the window, then this WOULD be an issue.
		//log::out << log::RED << "AssetStorage::cleanup - some assets are not deleted on the opengl context!\n";
		m_assetMutex.unlock();
		ENGONE_DEBUG(log::out << "AssetStorage::cleanup - finished\n";)
	}
	void AssetStorage::addProcessor() {
		m_procMutex.lock();
		if (m_running) {
			m_processors.push_back(new AssetProcessor(this, Asset::LoadData));
			m_processors.back()->start();
		}
		m_procMutex.unlock();
	}
	void AssetStorage::addIOProcessor() {
		m_ioProcMutex.lock();
		if (m_running) {
			m_ioProcessors.push_back(new AssetProcessor(this, Asset::LoadIO));
			m_ioProcessors.back()->start();
		}
		m_ioProcMutex.unlock();
	}
	void AssetStorage::printTasks() {
		int totalTasks = 0;
		m_ioProcMutex.lock();
		m_procMutex.lock();
		m_glProcMutex.lock();

		for (auto p : m_ioProcessors) {
			totalTasks += p->numQueuedTasks();
		}
		for (auto p : m_processors) {
			totalTasks += p->numQueuedTasks();
		}
		totalTasks += m_glProcessor->numQueuedTasks();

		log::out << "Total Tasks: " << totalTasks<<"\n";
		log::out << "Processors: io:" << m_ioProcessors.size() << " dt: " << m_processors.size() << " gl: 1"<< "\n";
		m_ioProcMutex.unlock();
		m_procMutex.unlock();
		m_glProcMutex.unlock();
	}
	void AssetStorage::processTask(AssetTask task) {
		//if (task.asset->m_path.empty()) return;
		if (task.asset->m_state == Asset::Failed) return;

		if (task.asset->m_flags & Asset::LoadIO) {
			m_ioProcMutex.lock();
			if (m_ioProcessors.size() == 0) {
				m_ioProcessors.push_back(new AssetProcessor(this, Asset::LoadIO));
				m_ioProcessors.back()->start();
			}
			int index = m_ioDistr;
			m_ioDistr = (m_ioDistr + 1) % m_ioProcessors.size();
			m_ioProcMutex.unlock();

			m_ioProcessors[index]->queue(task);
			return; // Note that we return because a task can only be processed by one processor at a time.
		}
		if (task.asset->m_flags & Asset::LoadData) {
			m_procMutex.lock();
			if (m_processors.size() == 0) {
				m_processors.push_back(new AssetProcessor(this, Asset::LoadData));
				m_processors.back()->start();
			}
			int index = m_distr;
			m_distr = (m_distr + 1) % m_processors.size();
			m_procMutex.unlock();

			m_processors[index]->queue(task);
			return;
		}
		if (task.asset->m_flags & Asset::LoadGL) {
			m_glProcMutex.lock();
			if (!m_glProcessor) {
				m_glProcessor = new AssetProcessor(this, Asset::LoadGL);
				//m_glProcessor->start(); // <- don't want to start it. It should run in OpenGL context thread
			}
			m_glProcMutex.unlock();
			m_glProcessor->queue(task);
			return;
		}
	}
	AssetProcessor::AssetProcessor(AssetStorage* storage, Asset::LoadFlag type) 
		: m_storage(storage), m_processType(type) {
	}
	AssetProcessor::~AssetProcessor() {
		cleanup();
	}
	void AssetProcessor::cleanup() {
		if (m_thread.joinable()) {
			stop();
			m_thread.join();
		} else {
			// this is fine if it is gl processor, gl processor doesn't create a thread
			//log::out << log::RED << "AssetProcessor::stop - thread isn't joinable!\n";
		}
	}
	void AssetProcessor::start() {
		if (!m_running) {
			m_running = true;

			m_thread = std::thread([this]() {
				ENGONE_DEBUG(log::out << "AssetProcessor::start - started thread\n";)
				while (m_running) {

					process();

					std::unique_lock<std::mutex> lock(m_waitMutex);

					//m_queueMutex.lock();
					//int num = m_queue.size();
					//m_queueMutex.unlock();

					//if (num == 0) { // wait a little if there are no tasks in queue
					m_wait.wait(lock, [this]() {return m_queue.size() != 0||!m_running; });
					ENGONE_DEBUG(log::out << "AssetProcessor - stopped waiting\n";)
						//std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
					//}
				}
			});
		}
	}
	void AssetProcessor::process() {
		m_queueMutex.lock();
		std::vector<AssetTask> tasks = std::move(m_queue); // moving will clear m_queue
		m_queueMutex.unlock();

		//for (AssetTask& task : tasks) {
			//task.finalAsset->m_state = Asset::Loading;
		//}
		for (AssetTask& task : tasks) {
			// process it
			//log::out << "Process("<<m_processType<<"): " << task.asset->getPath() << "\n";
			// 
			//if (task.asset->getPath() == "assets/models/Platform/PlatformMesh.mesh"&&m_processType==Asset::LoadData) {
			//	//DebugBreak();
			//}
			// NOTE: loading and asset twice with the same type would cause errors.
			//   loading mesh twice with type LoadData for example.
			Asset::LoadFlags flags = task.asset->load(m_processType);
			task.asset->m_flags = flags;
			ENGONE_DEBUG(log::out << "Loaded " << task.asset->getPath()<<" next: "<<flags << "\n";)
			if (task.asset->m_error != Error::ErrorNone) {
				task.asset->m_flags = Asset::LoadNone;
				log::out << log::RED<<"Failed loading: " << task.asset->getPath() << "\n";
			}
			if (task.asset->m_state & Asset::Loaded) {
				if (task.asset->getPath().size() != 0) {
					ENGONE_DEBUG(log::out << "Loaded " << task.asset->getPath() << "\n";)
				}
			}
			//task.nextFlags = flags;
			// depending on asset type
			//task.finalAsset->m_state = Asset::Loaded;
		}
		for (AssetTask& t : tasks) {
			m_storage->processTask(t);
		}
		tasks.clear();
	}
	void AssetProcessor::stop() {
		m_running = false;
		m_wait.notify_one();
	}
	void AssetProcessor::queue(AssetTask task) {
		ENGONE_DEBUG(log::out << "Queue Task: " << task.asset->getPath() << " " << (Asset::LoadFlag)task.asset->m_flags<< "\n";)
		std::lock_guard lk(m_waitMutex);
		m_queueMutex.lock();
		m_queue.push_back(task);
		m_queueMutex.unlock();
		m_wait.notify_one();
	}
	int AssetProcessor::numQueuedTasks() {
		m_queueMutex.lock();
		int num = m_queue.size();
		m_queueMutex.unlock();
		return num;
	}
}