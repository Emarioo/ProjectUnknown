#pragma once

#include "Engone/Assets/AnimationAsset.h"
#include "Engone/Assets/ArmatureAsset.h"
#include "Engone/Assets/ColliderAsset.h"
#include "Engone/Assets/FontAsset.h"
#include "Engone/Assets/MaterialAsset.h"
#include "Engone/Assets/MeshAsset.h"
#include "Engone/Assets/ModelAsset.h"
#include "Engone/Assets/TextureAsset.h"
#include "Engone/Assets/ShaderAsset.h"

/* -- AssetModule --

AssetStorage - Assets and processers are storeds here. (accessed from multiple threads)
Loader - one thread for reading and writing files (I/O)
Processer - Processes tasks from the loader into usable assets. (multiple threads)

OPENGL is not multithreaded which means only one thread is allowed
for OpenGL related things(this would be on the render thread).

AssetStorage has disc and non-disc storage.
Files can be loaded to disc storage.
Asses which you make can be added to non-disc storage. (shaders for example)

how does sync work with renderer thread

ask storage for an asset. It gives you a pointer to asset
You can specify flags. Sync or Async.
Sync will ensure that the returned asset isn't empty(it has been loaded)
Async will give you an asset which probably is empty but is loaded whenever the
processor is done

ask storage to reload an asset from disc if it has been updated on disc.
ask storage to reload all assets from disc.

purpose of unload is to clean assets and memory so that is what it should do. delete asset

Any loaded asset is assumed to be used by storage

MaterialAsset::load - requires opengl context when without storage because of diffuse_map which is a texture asset

cleanup on some assets require opengl context. Have a delete queue process? it would need to out live AssetStorage
could be destroyed before delete queue has finished. Waiting for delete queue and then destroying storage could be an option
unless storage is being destroyed on render thread in which case it might lock itself depending on how things are coded.

*/

namespace engone {
	extern int ASSET_LEVEL;

	// Update
	// Render
	// glfw event
	// Update...

	struct AssetTask {
		//AssetTask(Asset* asset, Asset::LoadFlags flags) : asset(asset) , nextFlags(flags) {}
		AssetTask(Asset* asset) : asset(asset) {}
		Asset* asset;
		//Asset::LoadFlags nextFlags;
		//enum State : uint8_t {
		//	None = 0,
		//	BaseProcessing = 1,
		//	GLProcessing = 2,
		//	Finished = 4,
		//};
		//State state=None; // finished, phase1, phase2
	};
	// An instance of this class exists in each instance of Window.
	// Allow auto refresh
	class AssetStorage {
	public:
		AssetStorage();
		~AssetStorage();

		//-- Flags for methods
		static const uint8_t SYNC = 1; // Perform function synchronously. Applies to AssetStorage::load
		//static const uint8_t FORCE_RELOAD = 2;// Load were supposed to always load asset even if it had been loaded. BUT, it will now reload automatically.
		static const uint8_t DEFAULT_FLAGS = 0;

		// load asset from path
		template<class T>
		T* load(const std::string& path, uint8_t flags = DEFAULT_FLAGS) {
			// ask to load an asset as a task through processors.
			// reload asset if already loaded
			m_assetMutex.lock();
			std::string _path = modifyPath<T>(path);

			T* asset = nullptr;

			//-- Find asset, (simular to AssetStorage::get)
			auto& list = m_assets[T::TYPE];
			auto find = list.find(path);
			if (find != list.end()) {
				asset = (T*)find->second;
			}

			if (!asset) {
				//log::out << "Load " << path << "\n";
				asset = new T();
				asset->setStorage(this);
				GetTracker().track(asset);
				m_assets[T::TYPE][path] = asset;
				_path += GetFormat(T::TYPE);
				asset->setPath(_path);
				asset->setLoadName(path);
			}
			m_assetMutex.unlock();
			
			if (asset->m_state == Asset::Processing) // return if asset already is being processed
				return asset; 
			
			asset->m_state = Asset::Processing;
			if (flags & SYNC) {
				// load returns flags but we can safely ignore them.
				asset->load(Asset::LoadAll);
			} else {
				asset->m_flags = Asset::LoadIO;
				AssetTask task(asset);
				processTask(task);
			}
			return asset;
		}
		
		// deletes an asset which was loaded from path
		// note that the pointer of the asset is invalid memory after it is unloaded
		template<typename T>
		void unload(const std::string& path, uint8_t flags = DEFAULT_FLAGS) {
			m_assetMutex.lock();
			std::string _path = modifyPath<T>(path);

			auto& list = m_assets[T::TYPE];
			auto find = list.find(path);
			if (find != list.end()) {
				if (find->second->m_state == Asset::Processing) {
					log::out << "AssetStorage::unload - Asset cannot be unloaded while being processed!\n";
				} else {
					delete find->second; // can asset be deleted?
					GetTracker().untrack(find->second);
					list.erase(find);
				}
			}
			m_assetMutex.unlock();
		}
		
		// returns nullptr if not found. str could be path or name
		template<typename T>
		T* get(const std::string& str, uint8_t flags = DEFAULT_FLAGS) {
			m_assetMutex.lock();
			auto& list = m_assets[T::TYPE];
			auto find = list.find(str);
			T* out = nullptr;
			if (find != list.end()) {
				out = (T*)find->second;
			}
			m_assetMutex.unlock();
			return out;
		}

		// returns nullptr if name already exists.
		// by using this function you relieve your responsibility of deleting asset. It is now done by storage.
		template<typename T>
		T* set(const std::string& name, T* asset, uint8_t flags = DEFAULT_FLAGS) {
			asset->m_path.clear(); // no path, asset cannot reload, you can of course change it's data yourself

			m_assetMutex.lock();

			auto& list = m_assets[T::TYPE];
			auto find = list.find(name);
			T* t = nullptr;
			bool doStuff = false;
			if (find != list.end()) {
				t = (T*)find->second;
				if (t == asset) {
					// fine
				} else {
					// not fine
					// should maybe warn here
					log::out << log::YELLOW << "AssetStorage::set - overwriting asset, deleting old one.\n";
					delete t; // ISSUE: DELETING DOES NOT WORK IF IT IS OPENGL STUFF
					t = asset;
					doStuff = true;
				}
			} else {
				t = asset;
				doStuff = true;
			}
			if (doStuff) {
				t->setStorage(this);
				GetTracker().track(asset);
				m_assets[T::TYPE][name] = t;
				
				if (t->m_flags == Asset::LoadNone) {
					t->m_state = Asset::Loaded;
				} else {
					t->m_state = Asset::Processing;
					if (flags&SYNC) {
						t->load(Asset::LoadAll);
					} else {
						AssetTask task(t);
						processTask(t);
					}
				}
			}
			m_assetMutex.unlock();

			return t;
		}

		void addIOProcessor();
		void addDataProcessor();
		void addGraphicProcessor();

		void cleanup();

		void printTasks();

		std::vector<AssetProcessor*>& getIOProcessors() { return m_ioProcessors; }
		std::vector<AssetProcessor*>& getDataProcessors() { return m_dataProcessors; }
		std::vector<AssetProcessor*>& getGraphicProcessors() { return m_graphicProcessors; }

	private:
		bool m_running = false;

		// will queue task into appropriate processor
		// path needs to be set for the asset before hand.
		void processTask(AssetTask task);
		
		//-- distrubtion when queueing tasks
		int m_ioNext=0;
		int m_dataNext=0;
		int m_graphicNext=0;

		//-- Processors
		std::vector<AssetProcessor*> m_ioProcessors;
		std::vector<AssetProcessor*> m_dataProcessors;
		std::vector<AssetProcessor*> m_graphicProcessors; // OpenGL is single threaded. Can only have one processor.
		std::mutex m_ioProcMutex;
		std::mutex m_dataProcMutex;
		std::mutex m_graphicProcMutex;

		//-- Assets in storage
		std::unordered_map<AssetType, std::unordered_map<std::string,Asset*>> m_assets;
		std::mutex m_assetMutex;
		
		// loading assets and then changing this will not allow
		// you to access those assets again since the path is different
		std::string m_storagePath="assets/";

		template<typename T>
		std::string modifyPath(const std::string& str) {
			std::string out;
			// optimize this
			if (!m_storagePath.empty() && !str._Starts_with(m_storagePath))
				out += m_storagePath;

			if (T::TYPE == ModelAsset::TYPE)
				out += "models/";

			out += str;

			return out;
		}

		friend class AssetLoader;
		friend class AssetProcessor;
	};
	// call start to run the processor on a thread.
	// you can call process to update tasks incase you don't want it on a thread. (processing OpenGL stuff for example)
	class AssetProcessor {
	public:
		AssetProcessor(AssetStorage* storage, Asset::LoadFlag type);
		~AssetProcessor();
		void cleanup();

		// creates a thread which runs process in a while loop with a bit of sleep.
		void start();
		void stop();

		// works through the queue and clears it. Finished tasks are given to AssetStorage for more processing.
		void process();

		void queue(AssetTask task);

		int numQueuedTasks();
		
	private:
		AssetStorage* m_storage = nullptr;
		bool m_running=false;

		std::thread m_thread;

		Asset::LoadFlag m_processType;
		std::vector<AssetTask> m_queue; // tasks to do
		std::mutex m_queueMutex;

		std::mutex m_waitMutex;
		std::condition_variable m_wait;
	};
}