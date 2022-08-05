#pragma once

#include "Engone/Utilities/Tracker.h"

// uses _malloc, random when debugging
#include "Engone/Utilities/Utilities.h"

namespace engone {
	TrackerId TrackNode::trackerId = { "TrackNode", log::LIME};
	log::logger operator<<(log::logger out, const TrackerId& id) {
		log::Color prev = out.getColor();
		if (id.color != 0)
			out << id.color;
		out << id.name;
		out << prev;
		return out;
	}
	// returns true if added, false if ptr already exists
	bool TrackTree::add(value ptr) {
		info.actions++;
#ifdef ENGONE_DEBUG_TRACKER
		if(info.debug)
			log::out << "add " << ptr << "\n";
#endif
		// make sure i can add another node if i need to.
		if (maxCount < head + 1) {
			if (!data) {
				if (!resize(20)) {
					return 0;
				}
			} else {
				if (!resize((head + 1) * 2)) {
					return 0;
				}
			}
		}
		if (getRoot()) {
			return getRoot()->add(this, ptr);
		} else {
			rootIndex = newNode();
			if (getRoot()) {
				memset(getRoot(), 0, sizeof(TrackNode));
				getRoot()->valuePtr = ptr;
				return true;
			} else return false;
		}
	}
	// returns true if ptr was removed, false if it didn't exist
	bool TrackTree::remove(value ptr) {
		info.actions++;
#ifdef ENGONE_DEBUG_TRACKER
		if (info.debug)
			log::out << "remove " << ptr << "\n";
#endif
		if (!getRoot()) return false;
		return getRoot()->remove(this, rootIndex, ptr);
	}
	void TrackTree::printNodes() const {
		log::out << "Nodes\n";
		for (int i = 0; i < head; i++) {
			TrackNode* node = (data + i);
			log::out << i << " " << node->valuePtr;
			if (node->ai)
				log::out << "  a: " << node->ai;
			//else
			//	log::out << "  a:-1";

			if (node->bi)
				log::out << "  b: " << node->bi << "\n";
			else
				log::out << "\n";
		}
	}
	void TrackTree::print() const {
		log::out << "Tree\n";
		if (getRoot()) {
			char from[8];
			memset(from, 0, sizeof(from));
			sprintf(from, " r: %u", rootIndex);
			getRoot()->print(this, 0, from);
		}
	}
	void TrackTree::clear() {
		resize(0);
#ifdef ENGONE_DEBUG_TRACKER
		uint32_t debug = info.debug;
		TrackTree* tree = info.lastSession;
#endif
		info = {};
#ifdef ENGONE_DEBUG_TRACKER
		info.debug=debug;
		info.lastSession=tree;
#endif
	}

	void TrackNode::print(const TrackTree* tree, int depth, const char* str) const {
		TrackNode* a = tree->getNode(ai);
		TrackNode* b = tree->getNode(bi);
		for (int i = 0; i < depth * 2; i++) log::out << " ";
		log::out << valuePtr << str << "\n";
		char from[8];
		if (ai) {
			memset(from, 0, sizeof(from));
			sprintf(from, " a: %u", ai);
			a->print(tree, depth + 1, from);
		}
		if (bi) {
			memset(from, 0, sizeof(from));
			sprintf(from, " b: %u", bi);
			b->print(tree, depth + 1, from);
		}
	}
	// returns true if added, false if ptr already exists, or if something failed
	bool TrackNode::add(TrackTree* tree, TrackTree::value ptr) {
		TrackNode* a = tree->getNode(ai);
		TrackNode* b = tree->getNode(bi);
		if (valuePtr == ptr) return false; // could not add ptr
		if (ptr < valuePtr) {
			if (a) {
				return a->add(tree, ptr);
			} else {
				ai = tree->newNode();
				a = tree->getNode(ai);
				if (a) {
					a->valuePtr = ptr;
					return true;
				} else return false;
			}
		} else if (ptr > valuePtr) {
			if (b) {
				return b->add(tree, ptr);
			} else {
				bi = tree->newNode();
				b = tree->getNode(bi);
				if (b) {
					b->valuePtr = ptr;
					return true;
				} else return false;
			}
		}
	}
	// the returned node was detached when you broke a node. The other node replaced the removed node.
	// returned node is nullptr if no node was detached
	bool TrackNode::remove(TrackTree* tree, uint32_t& ref, TrackTree::value ptr) {
		TrackNode* a = tree->getNode(ai);
		TrackNode* b = tree->getNode(bi);
		if (valuePtr == ptr) {
			uint32_t loose1 = 0;
			uint32_t loose2 = 0;
			// ALL THIS SHOULD NOT FAIL IF THERE ARE NO LOOSE NODES

			TrackNode* cpy = tree->getNode(tree->head);// head-1 is the actual location, getNode decrements index by one. so it adds up.

			// could be nullptr because ref = loose1
			// find the node that refers to the last node and change it to refer to this
			TrackNode* root = tree->getRoot();
			if (root) {
				if (root->valuePtr == cpy->valuePtr) tree->rootIndex = ref;
				else root->replace(tree, cpy->valuePtr, ref);
			}
			if (ref % 2 == 0) { // for better distribution
				if (a) {
					loose1 = ai;
					loose2 = bi;
				} else if (b) {
					loose1 = bi;
				}
			} else {
				if (b) {
					loose1 = bi;
					loose2 = ai;
				} else if (a) {
					loose1 = ai;
				}
			}

			uint32_t oldRef = ref; // for debugging

			// attach a loose node to the parent
			ref = loose1;

			//*this = *(tree->data + tree->head - 1);
			// move last node to memory at this

			// remove last node
			tree->head--;

			TrackNode old = *this; // copy for debugging

			valuePtr = cpy->valuePtr;
			ai = cpy->ai;
			bi = cpy->bi;

			if (loose2) // no need to attach nullptr
				tree->getRoot()->reattach(tree, loose2); // reattach to parent node is enough but i don't have access to it so root will do.

#ifdef ENGONE_DEBUG_TRACKER
			// remove is usually the one to cause issues
			int dup = tree->findDouble();
			int miss = tree->findMissing();
			if (miss||dup) {
				log::out << "Action: " << tree->info.actions << "\n";
				tree->print();
				if (tree->info.lastSession) {
					tree->info.lastSession->print();
				}
				tree->save("tracking.cache");
				DebugBreak();
			}
#endif
			return true;
		} else if (ptr < valuePtr) {
			if (a)
				return a->remove(tree, ai, ptr);
		} else if (ptr > valuePtr) {
			if (b)
				return b->remove(tree, bi, ptr);
		}
		return false;
	}
	void TrackNode::replace(TrackTree* tree, TrackTree::value ptr, uint32_t index) {
		TrackNode* a = tree->getNode(ai);
		TrackNode* b = tree->getNode(bi);
		if (ptr < valuePtr) {
			if (a) {
				if (a->valuePtr == ptr) {
					ai = index;
				} else {
					a->replace(tree, ptr, index);
				}
			}
		} else if (ptr > valuePtr) {
			if (b) {
				if (b->valuePtr == ptr) {
					bi = index;
				} else {
					b->replace(tree, ptr, index);
				}
			}
		} else {
			// do nothing, replacing yourself doesn't work
			log::out << log::RED<<"replaced self\n"; // hasn't happened, hopefully won't
		}
	}
	void TrackNode::reattach(TrackTree* tree, uint32_t index) {
		TrackNode* a = tree->getNode(ai);
		TrackNode* b = tree->getNode(bi);
		TrackNode* node = tree->getNode(index);
		if (node->valuePtr < valuePtr) {
			if (a) {
				a->reattach(tree, index);
			} else {
				ai = index;
			}
		} else if (node->valuePtr > valuePtr) {
			if (b) {
				b->reattach(tree, index);
			} else {
				bi = index;
			}
		} else {
			tree->print();
			log::out << log::RED << "Reattach, found an equal ptr, should not happen!\n";
		}
	}
	TrackNode* TrackTree::getNode(uint32_t index) const {
		if (index == 0) return nullptr;
		return data + index - 1;
	}
	uint32_t TrackTree::newNode() {
		if (maxCount < head + 1) {
			return 0; // failed, resizing here is dangerous
		}
		memset(data + head, 0, sizeof(TrackNode));
		head++;
		return head;
	}
	TrackNode* TrackTree::getRoot() const {
		return getNode(rootIndex);
	}
	// size is the count of nodes.
	bool TrackTree::resize(uint32_t size) {
		if (size == 0) {
			if (data) {
#ifdef ENGONE_DEBUG_TRACKER
				free(validations);
				GetTracker().subMemory(sizeof(bool)*maxCount);
				validations = nullptr;
#endif
				alloc::_free(data,sizeof(TrackNode)*maxCount);
				GetTracker().subMemory<TrackNode>(sizeof(TrackNode) * maxCount);
				data = nullptr;
				maxCount = 0;
				head = 0;
				rootIndex = 0;
			}
		} else {
			if (!data) {
				TrackNode* newData = (TrackNode*)alloc::_malloc(sizeof(TrackNode) * size);
				if (!newData) return false;
				GetTracker().addMemory<TrackNode>(sizeof(TrackNode) * size);
#ifdef ENGONE_DEBUG_TRACKER
				validations = (bool*)malloc(sizeof(bool) * size);
				GetTracker().addMemory(sizeof(bool) * size);
#endif
				data = newData;
				maxCount = size;
			} else {
				TrackNode* newData = (TrackNode*)alloc::_realloc(data, maxCount*sizeof(TrackNode),sizeof(TrackNode) * size);
				if (!newData) return false;
				GetTracker().addMemory<TrackNode>(sizeof(TrackNode) * (size-maxCount));
#ifdef ENGONE_DEBUG_TRACKER
				validations = (bool*)realloc(validations, sizeof(bool) * (size-maxCount));
				GetTracker().addMemory(sizeof(bool) * (size - maxCount));
#endif
				data = newData;
				maxCount = size;
				if (head > maxCount) head = maxCount;
			}
		}
		return true;
	}
	Tracker global_tracker;
	Tracker& GetTracker() {
		return global_tracker;
	}
	void Tracker::printMemory() {
		log::out << "Memory summary:\n";
		log::out << " Total: " << m_totalMemory << "\n";
		log::out << " Floating: " << m_floatingMemory << "\n";
		for (auto& [id, info] : m_trackedObjects) {
			uint32_t sum = info.tree.head * info.size + info.floatingMemory;
			if (sum != 0) {
				log::out << " " << id << ": " << (sum) << ", " << (info.tree.head)<<", "<<info.floatingMemory << "\n";
			}
		}
		if (m_trackedObjects.size() == 0) {
			log::out << "No classes\n";
		}
	}
	void Tracker::printTree() {
		for (auto& [key, info] : m_trackedObjects) {
			info.tree.print();
		}
	}
	void Tracker::clear(bool force) {
		if (m_locked && !force) {
			m_shouldClear = true;
			return;
		}
		m_locked = true;

		m_trackQueue.clear();
		m_untrackQueue.clear();
		// Since StackTree uses the tracker we need to untacks it's memory and then clear trackedObjects
		for (auto& [id, info] : m_trackedObjects) {
			info.tree.clear();
		}
		m_trackedObjects.clear();
		//m_totalMemory = 0;
		//m_floatingMemory = 0;

		m_locked = false;
		m_shouldClear = false;
	}
	Tracker::ClassInfo& Tracker::getClassInfo(TrackerId id) {
		TrackTree* tree = nullptr;
		auto find = m_trackedObjects.find(id);
		if (find == m_trackedObjects.end()) {
			m_trackedObjects[id] = {};
			return m_trackedObjects[id];
		} else {
			return find->second;
		}
	}
	void Tracker::handleQueue() {
		if (m_locked)
			return;
		m_locked = true;
		while (m_trackQueue.size() > 0) {

			m_trackQueueLocked = true;
			TrackClass obj = m_trackQueue.back();
			m_trackQueue.pop_back();
			m_trackQueueLocked = false;


			ClassInfo& info = getClassInfo(obj.id);
			if(obj.size!=0)
				info.size = obj.size;

			bool yes = info.tree.add(obj.ptr);
			if (m_info.logging)
				log::out << "Track " << obj.id << " " << info.size;
			if (yes) {
				m_info.tracks++;
				m_totalMemory += info.size;
				if (m_info.logging)
					log::out << " " << m_totalMemory << " added\n";
			} else {
				if (m_info.logging)
					log::out << " existing\n";
			}
		}
		while (m_untrackQueue.size() > 0) {

			m_untrackQueueLocked = true;
			TrackClass obj = m_untrackQueue.back();
			m_untrackQueue.pop_back();
			m_untrackQueueLocked = false;

			ClassInfo& info = getClassInfo(obj.id);
			if(obj.size!=0)
				info.size = obj.size;

			bool yes = info.tree.remove(obj.ptr);
			if (m_info.logging)
				log::out << "Untrack "<<obj.id<<" " << info.size;
			if (yes) {
				m_info.untracks++;
				m_totalMemory -= info.size;
				if (m_info.logging)
					log::out << " " << m_totalMemory<<" removed\n";
			} else {
				if (m_info.logging)
					log::out << " missing\n";
			}
		}
		if (m_shouldClear)
			clear(true);
		m_locked = false;
	}
#ifdef ENGONE_DEBUG_TRACKER
	uint32_t TrackTree::findDouble() const {
		if (!validations) return 0;
		memset(validations, 0, head * sizeof(bool));
		if (rootIndex)
			validations[rootIndex - 1] = true;
		for (int i = 0; i < head; i++) {
			TrackNode* node = getNode(i + 1);
			if (node->ai) {
				if (validations[node->ai - 1])
					return node->ai;
				else validations[node->ai - 1] = true;
			}
			if (node->bi) {
				if (validations[node->bi - 1])
					return node->bi;
				else validations[node->bi - 1] = true;
			}
		}
		return 0;
	}
	uint32_t TrackTree::findMissing() const {
		if (!validations) return 0;
		memset(validations, 0, head * sizeof(bool));
		if(rootIndex)
			validations[rootIndex - 1] = true;
		for (int i = 0; i < head; i++) {
			TrackNode* node = getNode(i + 1);
			if (node->ai) {
				validations[node->ai - 1] = true;
			}
			if (node->bi) {
				validations[node->bi - 1] = true;
			}
		}
		for (int i = 0; i < head; i++) {
			if (!validations[i])
				return i;
		}
		return 0;
	}
	bool TrackTree::save(const std::string& path) const {
		FileWriter writer(path, false);

		if (!writer.isOpen())
			return false;
		try {
			uint32_t seed = GetRandomSeed();
			writer.write(&seed);
			writer.write(&info.actions);
			writer.write(&rootIndex);
			writer.write(&maxCount);
			writer.write(&head);
			writer.write((char*)data, sizeof(TrackNode) * head); // data between maxCount-head is useless
		} catch (FileError err) {
			log::out << err << "\n";
			return false;
		}
		return true;
	}
	bool TrackTree::load(const std::string& path, uint32_t& seed, uint32_t& actions) {
		FileReader reader(path, false);
		if (!reader.isOpen())
			return false;

		try {
			reader.read(&seed);
			reader.read(&actions);
			reader.read(&rootIndex);
			reader.read(&maxCount);
			reader.read(&head);
			resize(maxCount);
			reader.read((char*)data, sizeof(TrackNode) * head);
		} catch (FileError err) {
			log::out << err << "\n";
			return false;
		}

		return true;
	}
	
	void TrackTreeTest() {
		TrackTree tree;
		TrackTree lastTree;

		const int objCount = 100;
		TrackTree::value objs[objCount];
		memset(objs, 0, sizeof(TrackTree::value) * objCount);

		uint32_t seed = 0;
		uint32_t actions = 0;
		bool yes = lastTree.load("tracking.cache", seed, actions);
		if (yes) {
			SetRandomSeed(seed);
			tree.getInfo().lastSession = &lastTree;
		}

		int totalActions = 0;
		int removals = 0;
		int addings = 0;
		while (true) {
			int index = GetRandom() * objCount;

			double prob = (1 + sin(totalActions / 4000000.0)) / 2;
			prob = 0.1 + prob * 0.8;
			if (GetRandom() > prob) {
				if (!objs[index]) {
					objs[index] = (TrackTree::value)index;
					//objs[index] = new int(index);
				}
				tree.add(objs[index]);
				//tracker.track(objs[index]);
				addings++;
			} else {
				//tracker.untrack(objs[index]);
				tree.remove(objs[index]);
				//delete objs[index];
				objs[index] = 0;
				removals++;
			}
			//if (tree.info.actions == actions) {
			//	//lastTree.print();
			//	//log::out << "okay\n";
			//}
			if (yes && tree.getInfo().actions > actions - 5) {
				tree.print();
				tree.getInfo().debug = true;
			}
			totalActions++;
			if (totalActions % 100000 == 0) {
				log::out << "Total actions: " << totalActions << "  tracks: " << addings << "  untracks: " << removals << "\n";
				//tree.print();
				addings = 0;
				removals = 0;
			}
			if (tree.getInfo().actions == 10000000) {
				log::out << "Cleared\n";
				SetRandomSeed((uint32_t)GetSystemTime());
				tree.clear();
			}
		}
	}
	class Fruit {
	public:
		static const char* trackerId;
		Fruit(int x) { this->x = x; }
		int x = 5;
		int y = 4;
	};
	const char* Fruit::trackerId = "Fruit";
	void TrackerTest() {
		//GetTracker().setClassName<Fruit>("Fruit");
		const int objCount = 100;
		Fruit* objs[objCount];
		memset(objs, 0, sizeof(Fruit*) * objCount);
		//for (int i = 0; i < objCount; i++) {
		//	objs[i] = new Fruit(i * 2);
		//	GetTracker().track(objs[i]);
		//}
		//log::out << "Size of fruit " << sizeof(Fruit) << "\n";
		//log::out << "Size of all fruits " << (sizeof(Fruit)*objCount) << "\n";

		int totalActions = 0;
		int removals = 0;
		int addings = 0;
		while (totalActions < 100000) {
			int index = GetRandom() * objCount;

			double prob = (1 + sin(totalActions / 4000000.0)) / 2;
			prob = 0.1 + prob * 0.8;
			if (GetRandom() > prob) {
				if (!objs[index]) {
					objs[index] = new Fruit(index);
				}
				GetTracker().track(objs[index]);
				addings++;
			} else {
				delete objs[index];
				GetTracker().untrack(objs[index]);
				objs[index] = 0;
				removals++;
			}
			totalActions++;
			if (totalActions % 10000 == 0) {
				log::out << "Total actions: " << totalActions << "  tracks: " << addings << "  untracks: " << removals << " Memory: " << GetTracker().getMemory() << "\n";
				addings = 0;
				removals = 0;
			}
		}

		GetTracker().printMemory();
		for (int i = 0; i < objCount; i++) {
			if (objs[i]) {
				GetTracker().untrack(objs[i]);
				delete objs[i];
				objs[i] = nullptr;
			}
		}
		GetTracker().printMemory();
		GetTracker().clear();
	}
#endif
}