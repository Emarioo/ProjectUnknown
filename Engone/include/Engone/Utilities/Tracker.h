#pragma once

#include "Engone/Logger.h"

//#define ENGONE_DEBUG_TRACKER

namespace engone {

	// Memory Tracker
	// Two types of tracking.
	// Floating memory: this is just a variable you can add and subtract from (functions addMemory,subMemory). Mostly used with malloc/free.
	// Single pointers: functions track and untrack to add a pointer to a unique list. Duplicates of the same pointers will be ignored.
	//	You can see ho much memory each type of pointer takes up. Example, 95 bytes for Banana, 44 for Apple.
	//  See track/untrack functions for more details.
	//
	// A rule of thumb is to not track classes in constructors or destructors. Floating memory is fine.
	//
	// ISSUE: log an error if allocator failed?
	// FEATURE: filter for classes when printing?
	// FEATURE: standrad trackerIds for int, float, std::thread, and also track/untrack which works on them.
	// FEATURE: color codes for classes TrackerColor?
	class TrackNode;
	class TrackTree;
	struct TrackInfo {
		uint32_t actions = 0;
#ifdef ENGONE_DEBUG_TRACKER
		TrackTree* lastSession = nullptr;
		bool debug = false; // whether to log add, remove actions
#endif
	};
	// ISSUE: there is a occasional issue where a duplicate index is found.
	class TrackTree {
	public:
		TrackTree() = default;
		~TrackTree() {
			resize(0);
		}

		typedef void* value;

		// returns true if added, false if ptr already exists
		bool add(value ptr);
		// returns true if ptr was removed, false if it didn't exist
		bool remove(value ptr);
		void print() const;
		// will clear all data. As good as new.
		void clear();

		void printNodes() const;

		TrackInfo& getInfo() { return info; }

	private:

		TrackInfo info;

		uint32_t rootIndex = 0;
		uint32_t maxCount = 0;
		uint32_t head = 0;
		// only change the allocation with resize
		TrackNode* data = nullptr;

		TrackNode* getNode(uint32_t index) const;
		uint32_t newNode();
		TrackNode* getRoot() const;

		// size is the count of nodes.
		bool resize(uint32_t size);

		friend class TrackNode;
		friend class Tracker;

#ifdef ENGONE_DEBUG_TRACKER
	public:
		bool* validations = nullptr; // should be size of maxCount
		// searches for a duplicate of an index. returns it if found, 0 if everything is fine
		uint32_t findDouble() const;
		// checks if memory is contiguous, returns true if not
		uint32_t findMissing() const;
		// save tree to file
		bool save(const std::string& path) const;
		// load tree from file
		bool load(const std::string& path, uint32_t& outSeed, uint32_t& actions);

		//friend void TrackTreeTest();
#endif
	};
	// name, color, filter  {"Fruit",log::RED,3}
	// printing id with color code 0, will use whatever previous color log color.
	struct TrackerId {
		TrackerId(const char* name) : name(name) {}
		TrackerId(const char* name, log::Color color) : name(name), color(color) {}
		TrackerId(const char* name, log::Color color, uint16_t filter) : name(name), color(color),filter(filter) {}
		const char* name = "";
		log::Color color = (log::Color)0;
		uint16_t filter = -1;

		bool operator==(const TrackerId& id) const {
			return name == id.name;
		}
	};

}
template<>
struct std::hash<engone::TrackerId> {
	std::size_t operator()(const engone::TrackerId& id) const {
		return std::hash<std::size_t>{}((std::size_t)id.name);
	}
};
namespace engone{

	log::logger operator<<(log::logger out, const TrackerId& id);
	class TrackNode {
	public:
		TrackNode() = default;
		void print(const TrackTree* tree, int depth, const char* str) const;
		// returns true if added, false if ptr already exists, or if something failed
		bool add(TrackTree* tree, TrackTree::value ptr);
		// the returned node was detached when you broke a node. The other node replaced the removed node.
		// returned node is nullptr if no node was detached
		bool remove(TrackTree* tree, uint32_t& ref, TrackTree::value ptr);
		void replace(TrackTree* tree, TrackTree::value ptr, uint32_t index);
		void reattach(TrackTree* tree, uint32_t index);

		static TrackerId trackerId;
	private:
		TrackTree::value valuePtr = 0;
		uint32_t ai = 0;
		uint32_t bi = 0;

		friend class TrackTree;
	};
	// All template functions in this class requires T to have T::trackerId. See Tracker::track(T*) for more details.
	class Tracker {
	public:
		Tracker() = default;
		~Tracker() { 
			// need to clear the trees before destroying unordered_map since the trees need to untrack tracknodes.
			clear(true);
		}
		//static const TrackerFilter EngoneClasses=-1;
		struct Info {
			bool logging = false;
			int trackQueue = 0;
			int tracks = 0;
			int untrackQueue = 0;
			int untracks = 0;
			int addmems = 0;
			int submems = 0;
		};
		// id, size, ptr, filter, color
		struct TrackClass {
			TrackerId id;
			uint16_t size = 0;
			void* ptr = 0;
		};
		struct ClassInfo {
			TrackTree tree;
			uint16_t size=0;
			int32_t floatingMemory=0; // in bytes, negative if something is wrong
		};
		// Will track specified pointer, will do nothing if it already exists.
		// T needs to have T::trackerId. This is a static TrackerId member in class T.
		// Search for "static const char* in class" in your browser for more details. (TrackerId is a const char*)
		// Use track(TrackClass) if you can't add trackerId to class T.
		template<class T>
		void track(T* t) {
			track({ T::trackerId,sizeof(T),t });
		}
		// Will untrack specified pointer, will do nothing if it doesn't exist.
		// T needs to have T::trackerId. See Tracker::track(T*) for more information.
		template<class T>
		void untrack(T* t) {
			untrack({ T::trackerId,sizeof(T),t });
		}
		
		// General track function
		// Does not allow tracking of TrackNode
		void track(TrackClass obj) {
			if (!obj.ptr) return;
			if (obj.id == TrackNode::trackerId) {
				log::out << log::RED<<"You cannot track " << TrackNode::trackerId << "!\n";
				return;
			}
			if (!m_trackQueueLocked) {
				m_info.trackQueue++;
				m_trackQueue.push_back(obj);
				handleQueue();
			} else {
				log::out << log::RED << "Track queue was locked, could not track "<< obj.id << ":" << obj.ptr << "(id:ptr)\n";
			}
		}
		void untrack(TrackerId id, void* ptr) {
			untrack({id, 0,ptr});
		}
		// General untrack function
		void untrack(TrackClass obj) {
			if (!obj.ptr) return;
			if (obj.id == TrackNode::trackerId) {
				log::out << log::RED << "You cannot untrack " << TrackNode::trackerId << "!\n";
				return;
			}
			if (!m_untrackQueueLocked) {
				m_info.untrackQueue++;
				m_untrackQueue.push_back(obj);
				handleQueue();
			} else {
				log::out << log::RED << "Track queue was locked, could not track " << obj.id << ":" << obj.ptr << " (id:ptr)\n";
			}
		}
	
		// Total tracked memory
		// includeTracker as true will add the tracker's internal memory to the total. (you probably want this as false)
		int32_t getMemory(bool includeTracker=false) const {
			if(includeTracker)
				return m_totalMemory;

			int32_t out = m_totalMemory;
			auto find = m_trackedObjects.find(TrackNode::trackerId);
			if (find != m_trackedObjects.end()) {
				const ClassInfo& info = find->second;
				out-=info.floatingMemory; // TrackNodes are tracked with floating memory since they aren't individual.
			}

			return out;
		}
		// Memory used by the tracker itself. (TrackNodes)
		int32_t getInternalMemory() const {
			auto find = m_trackedObjects.find(TrackNode::trackerId);
			if (find != m_trackedObjects.end()) {
				const ClassInfo& info = find->second;
				return info.floatingMemory; // TrackNodes are tracked with floating memory since they aren't individual.
			} else {
				return 0;
			}
		}

		template<class T>
		uint32_t getClassMemory() const {
			return getClassMemory(T::trackerId);
		}
		// Memory for both single pointers and floating memory.
		uint32_t getClassMemory(TrackerId id) const {
			// cannot use getClassInfo becuse it is not const
			auto find = m_trackedObjects.find(id);
			if (find == m_trackedObjects.end()) {
				return 0;
			} else {
				const ClassInfo& info = find->second;
				return info.tree.head * info.size+info.floatingMemory;
			}
		}
		// Memory tracked by non-class floating memory.
		int32_t getFloatingMemory() const {
			return m_floatingMemory;
		}
		// For specific class
		template<class T>
		int32_t getFloatingMemory() const { 
			return getFloatingMemory(T::trackerId);
		}
		// For specific class
		int32_t getFloatingMemory(TrackerId id) const {
			auto find = m_trackedObjects.find(id);
			if (find == m_trackedObjects.end()) {
				return 0;
			} else {
				const ClassInfo& info = find->second;
				return info.floatingMemory;
			}
		}

		// ISSUE: not thread safe
		template<class T>
		void addMemory(uint32_t bytes) {
			addMemory(T::trackerId, bytes);
		}
		void addMemory(TrackerId id, uint32_t bytes) {
			m_info.addmems++;
			ClassInfo& info = getClassInfo(id);
			info.floatingMemory += bytes;
			m_totalMemory += bytes;
			if (m_info.logging)
				log::out << "Float " <<id<<" +" << bytes << " " << m_totalMemory << "\n";
		}
		// General memory
		void addMemory(uint32_t bytes) {
			m_info.addmems++;
			m_floatingMemory += bytes;
			m_totalMemory += bytes;
			if (m_info.logging)
				log::out << "Float +" << bytes << " "<<m_totalMemory<<"\n";
		}
		// ISSUE: not thread safe
		template<class T>
		void subMemory(uint32_t bytes) {
			subMemory(T::trackerId, bytes);
		}
		void subMemory(TrackerId id, uint32_t bytes) {
			m_info.submems++;
			ClassInfo& info = getClassInfo(id);
			info.floatingMemory -= bytes;
			m_totalMemory -= bytes;
			if (m_info.logging)
				log::out << "Float "<<id<<" -" << bytes << " " << m_totalMemory << "\n";
		}
		// General memory
		void subMemory(uint32_t bytes) {
			m_info.submems++;
			m_floatingMemory -=bytes;
			m_totalMemory -= bytes;
			if (m_info.logging)
				log::out << "Float -" << bytes << " " << m_totalMemory << "\n";
		}

		// prints tracked memory
		void printMemory();

		// Will clear tracked pointers but not totalMemory or general float memory.
		// Force as true will clear even if tracker is locked.
		// Will lock and unlock tracker.
		void clear(bool force=false);

		void printTree();
		// debug purpose
		//TrackTree* tree() {
		//	auto find = m_trackedObjects.begin();
		//	if (find == m_trackedObjects.end()) {
		//		m_trackedObjects[sizeof(int)] = {};
		//		return &m_trackedObjects[sizeof(int)];
		//	}
		//	return &find->second;
		//}

		Info& getInfo() { return m_info; }

		// Dev NOTE: function cannot be const because function will be creat ClassInfo if it doesn't exist.
		Tracker::ClassInfo& getClassInfo(TrackerId id);

		// Note that you can't fully rely on these values to see if there are memory leaks. AddMem could be used as SubMem. And you may do multiple AddMem but then one SubMem for all of them.
		void printInfo() {
			log::out << "Queues: "<<m_info.trackQueue<<" / "<<m_info.untrackQueue << " Track/Untracks: " << m_info.tracks << " / " << m_info.untracks << "  AddMem/SubMem: " << m_info.addmems << " / " << m_info.submems << "\n";
		}
	private:
		Info m_info;

		int32_t m_totalMemory=0;
		int32_t m_floatingMemory=0; // memory not belonging to any class

		std::vector<TrackClass> m_trackQueue;
		std::vector<TrackClass> m_untrackQueue;

		bool m_untrackQueueLocked, m_trackQueueLocked = false; // not sure if queue lock is necessary.
		bool m_locked = false;
		bool m_shouldClear = false;

		std::unordered_map<TrackerId, ClassInfo> m_trackedObjects;

		// will handle queues if not locked otherwise just return
		// template is only needed to debug values
		void handleQueue();
	};
#ifdef ENGONE_DEBUG_TRACKER
	// Function will run a while loop which will generate and insert random values into TrackTree
	// The tree will then to an evaluation of itself and will cause a breakpoint where you can debug issues.
	// If an error occured, a cache will be made of when it happend and what data it had.
	// Restarting the program would do the same thing but print out the tree a few times before the breakpoint.
	void TrackTreeTest();
	// Will test the tracker's functionality, not fully. just a bit
	void TrackerTest();
#endif

	// global tracker
	Tracker& GetTracker();
}
