#pragma once

//#include "Engone/Assets/Asset.h"

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
	class AssetStorage {
	public:
		AssetStorage();
		~AssetStorage();

		enum Flag : uint8_t {
			//Async=1, // Async by default
			Sync = 1,
			ForceLoad = 2, // When used in load function. A new asset will always be made. This new asset will overwrite an asset with the same name.
			SoftLoad = 4, // get
		};
		typedef uint8_t Flags;

		static const Flags defaultFlags = Sync;

		// creates a new asset loaded from path
		template<class T>
		T* load(const std::string& path, Flags flags = defaultFlags) {
			// ask to load an asset as a task through processors.
			// reload asset if already loaded
			m_assetMutex.lock();
			std::string _path = modifyPath<T>(path);

			T* t = nullptr;
			bool quit = false;
			if ((flags & ForceLoad) == 0) {
				auto& list = m_assets[T::TYPE];
				auto find = list.find(path);
				if (find != list.end()) {
					t = (T*)find->second;
					if (t->m_state == Asset::Processing) {
						// already loading
						quit = true;
					} else {
						//log::out << "Reload " << path << "\n"; // reload if asset failed or is loaded
					}
				}
			}
			if (!t) {
				//log::out << "Load " << path << "\n";
				t = new T();
				t->m_flags = Asset::LoadIO;
				t->setStorage(this);
				GetTracker().track(t);
				m_assets[T::TYPE][path] = t;
				_path += GetFormat(T::TYPE);
				t->setPath(_path);
				t->setLoadName(path);
			}
			m_assetMutex.unlock();
			if ((flags & SoftLoad) && t->m_state != Asset::Waiting){
				quit = true;
			}
			if (quit) {
				return t;
			}
			flags |= Sync;
			t->m_state = Asset::Processing;
			if (flags & Sync) {
				// load returns flags but we can safely ignore them.
				t->load(Asset::LoadAll);
			} else {
				//AssetTask task(t, Asset::LoadIO);
				AssetTask task(t);
				processTask(task);
			}

			return t;
		}
		// deletes an asset which was loaded from path
		// note that the pointer of the asset is invalid memory after it is unloaded
		template<typename T>
		void unload(const std::string& path, Flags flags = defaultFlags) {
			m_assetMutex.lock();
			std::string _path = modifyPath<T>(path);

			auto& list = m_assets[T::TYPE];
			auto find = list.find(path);
			if (find != list.end()) {
				delete find->second(); // bad, what if asset is being processed
				GetTracker().untrack(find->second);
				list.erase(find);
			}
			m_assetMutex.unlock();
		}
		// returns nullptr if not found. str could be path or name
		template<typename T>
		T* get(const std::string& str, Flags flags = defaultFlags) {
			m_assetMutex.lock();
			auto& list = m_assets[T::TYPE];
			auto find = list.find(str);
			T* out = nullptr;
			if (find != list.end()) {
				out = (T*)find->second;
			} 
			//else if(flags&SoftLoad) {
			//	out = load<T>(str, ForceLoad);
			//}
			m_assetMutex.unlock();
			return out;
		}
		//typedef MeshAsset T;
		// returns nullptr if name already exists.
		// by using this function you relieve your responsibility of deleting asset. It is now done by storage.
		template<typename T>
		T* set(const std::string& name, T* asset, Flags flags = defaultFlags) {
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
					if (flags&Sync) {
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

		template<typename T>
		std::string modifyPath(const std::string& str) {
			std::string out;
			// optimize this
			if (!m_storagePath.empty()&&!str._Starts_with(m_storagePath))
				out += m_storagePath;

			if (T::TYPE == ModelAsset::TYPE)
				out += "models/";

			out += str;

			return out;
		}
		
		// add LoadData processor
		void addProcessor();
		void addIOProcessor();

		void cleanup();

		void printTasks();

		// can be null if no asset has been queued which requires this processor
		AssetProcessor* getGLProc() const { return m_glProcessor; }

	private:
		bool m_running = false;

		// will queue task into appropriate processor
		// path needs to be set for the asset before hand.
		void processTask(AssetTask task);
		int m_ioDistr=0; // distrubtion when queueing tasks
		int m_distr=0;

		//AssetProcessor* m_ioProcessor=nullptr;
		std::vector<AssetProcessor*> m_ioProcessors;
		std::vector<AssetProcessor*> m_processors;
		AssetProcessor* m_glProcessor=nullptr; // OpenGL is single threaded. Can only have one processor

		std::mutex m_ioProcMutex;
		std::mutex m_procMutex;
		std::mutex m_glProcMutex;

		// all assets in storage
		std::mutex m_assetMutex;
		std::unordered_map<AssetType, std::unordered_map<std::string,Asset*>> m_assets;
		//std::unordered_map<AssetType, std::vector<Asset*>> m_assets;
		
		// loading assets and then changing this will not allow
		// you to access those assets again since the path is different
		std::string m_storagePath="assets/";

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
		std::mutex m_queueMutex;

		Asset::LoadFlag m_processType;
		std::vector<AssetTask> m_queue; // tasks to do

		std::mutex m_waitMutex;
		std::condition_variable m_wait;
	};
}