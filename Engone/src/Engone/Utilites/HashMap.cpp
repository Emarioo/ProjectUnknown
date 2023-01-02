#pragma once

#include "Engone/Utilities/HashMap.h"

#include "Engone/Utilities/RandomUtility.h"

namespace engone {

	HashMap::HashMap(int keymapSize) : keymapSize(keymapSize) {

	}
	HashMap::~HashMap() {
		if(!m_preventDeconstructor)
			cleanup();
	}
	void HashMap::preventDeconstructor() {
		m_preventDeconstructor = true;
	}
	void HashMap::cleanup() {
		keymap.resize(0);
		chainData.resize(0);
		emptySpots.cleanup();
		size = 0;
		m_preventDeconstructor = false;
	}
	int HashMap::getMemoryUsage() {
		return keymap.max*sizeof(Chain) + chainData.max*sizeof(Chain);
	}
	int HashMap::hash(Key key) {
		// will crash if keymap.max is 0.
		int index = key % keymap.max; // yes, you should laugh
		return index;
	}
	int HashMap::createChain() {
		int indexNext=0;
		if (emptySpots.size() != 0) {
			indexNext = emptySpots.pop();
			Chain* chain = chainData.data + indexNext;
			*chain = Chain();
		} else {
			if (chainData.used == chainData.max) {
				bool yes = chainData.resize((chainData.max) * 2 + 4);
				if (!yes)
					return -1; // failed allocating

				// initialize data
				for (int i = chainData.used; i < chainData.max; i++) {
					Chain* temp = chainData.data + i;
					*temp = Chain();
				}
			}
			indexNext = chainData.used;
			Chain* chain = chainData.data + indexNext;
			*chain = Chain();
			chainData.used++;
		}
		return indexNext;
	}
	bool HashMap::insert(Key key, Value value) {
		// Check for enough memory
		if (!keymap.data) {
			bool yes = keymap.resize(keymapSize);
			if (!yes)
				return false; // return if failed resize
			
			// initialize data
			for (int i = 0; i<keymap.max; i++) {
				Chain* chain = keymap.data + i;
				*chain = Chain();
			}
		}
		
		int index = hash(key);

		Chain* chain = keymap.data + index;
		while (true) {
			if (!chain->used) {
				chain->used = true;
				chain->key = key;
				chain->value = value;
				chain->indexNext = INVALID_INDEX;
				size++;
				return true;
			} else if (chain->key == key) {
				chain->value = value;
				return true;
			} else if(key<chain->key) { // this ensures that chains are sorted by the key variable
				// create chain for later use
				int indexNext = createChain();
				if (index == -1)
					return false;

				// move the chain to be replaced
				Chain* newChain = chainData.data + indexNext;
				*newChain = *chain;
				
				// replace old chain with new chain
				chain->used = true;
				chain->key = key;
				chain->value = value;
				chain->indexNext = indexNext;
				size++;

				return true;
				// chain could be keymap so chain needs to be replaced by new pair.
				// the old chain needs to be attached afterwards.
			} else if (chain->indexNext == INVALID_INDEX) {
				int indexNext = createChain();
				if (indexNext == -1)
					return false;

				chain->indexNext = indexNext;
				chain = chainData.data + indexNext;
				continue;
				//if (emptySpots.used != 0) {
				//	chain->indexNext = *(emptySpots.data+emptySpots.used-1);
				//	chain = chainData.data + chain->indexNext;
				//	*chain = Chain();
				//	emptySpots.used--;
				//} else {
				//	if (chainData.used == chainData.max) {
				//		bool yes = chainData.resize((chainData.max) * 2 + 4);
				//		if (!yes)
				//			return false; // failed allocating

				//		// initialize data
				//		for (int i = chainData.used; i < chainData.max; i++) {
				//			Chain* temp = chainData.data + i;
				//			*temp = Chain();
				//		}
				//	}
				//	chain->indexNext = chainData.used;
				//	chain = chainData.data + chain->indexNext;
				//	*chain = Chain();
				//	chainData.used++;
				//}
				//continue;
			} else {
				chain = chainData.data + chain->indexNext;
				// linked list stuff with overflow data
				continue;
			}
		}
		return false;
	}
	HashMap::Value HashMap::get(Key key, bool* found) {
		if (keymap.max == 0) {
			if(found)
				*found = false;
			return 0;
		}

		int index = hash(key);

		Chain* chain = keymap.data + index;
		while (true) {
			if (chain->used) {
				if (chain->key == key) {
					if (found)
						*found = true;
					return chain->value;
				} else if (chain->indexNext == INVALID_INDEX) {
					break; // failed
				} else {
					chain = chainData.data + chain->indexNext;
					continue;
				}
			} else {
				break; // failed
			}
		}
		// failed
		if (found)
			*found = false;
		return 0;
	}
	bool HashMap::get(Key key, Value& outValue) {
		if (keymap.max == 0) {
			return false;
		}

		int index = hash(key);

		Chain* chain = keymap.data + index;
		while (true) {
			if (chain->used) {
				if (chain->key == key) {
					outValue = chain->value;
					return true;
				} else if (chain->indexNext == INVALID_INDEX) {
					break; // failed
				} else {
					chain = chainData.data + chain->indexNext;
					continue;
				}
			} else {
				break; // failed
			}
		}
		// failed
		return false;
	}
	bool HashMap::erase(Key key) {
		if (keymap.max == 0) {
			return false;
		}

		int index = hash(key);

		Chain* firstChain = keymap.data + index;
		Chain* chain = firstChain;
		while (chain) {
			if (!chain->used) 
				return false;
			
			if (chain->key == key) {
				if (chain->indexNext == INVALID_INDEX) {
					*chain = Chain();
				} else {
					int indexNext = chain->indexNext;

					bool yes = emptySpots.push(indexNext);
					if (!yes) {
						//return false;  // uncomment to force efficient use of memory
					}

					Chain* nextChain = chainData.data+chain->indexNext;
					*chain = *nextChain;

					*nextChain = Chain();
				}
				size--;
				return true;
			}
			
			if (chain->indexNext==INVALID_INDEX)
				return false;

			chain = chainData.data + chain->indexNext;
		}
		
		return false;
	}
	//HashMap::Value& HashMap::operator[](int key) {
	//	// check key map
	//	if (!keymap.data) {
	//		bool yes = keymap.resize(keymapSize);
	//		if (!yes) {
	//			assert(false,"Allocation failed keymap");
	//		}
	//		// initialize data
	//		for (int i = 0; i < keymap.max; i++) {
	//			Chain* chain = keymap.data + i;
	//			*chain = Chain();
	//		}
	//	}
	//	int index = hash(key);
	//	THIS FUNCTION DOES NOT ORDER CHAINS BY KEY BUT SHOULD FIX IT BEFORE USING IT
	//	Chain* chain = keymap.data + index;
	//	while (chain) {
	//		if (!chain->used) {
	//			chain->used = true;
	//			chain->key = key;
	//			chain->value = 0;
	//			chain->indexNext = INVALID_INDEX;
	//			size++;
	//			return chain->value;
	//		} else if (chain->key == key) {
	//			return chain->value;
	//		} else if (chain->indexNext == INVALID_INDEX) {
	//			if (chainData.used == chainData.max) {
	//				bool yes = chainData.resize((chainData.max) * 2 + 4);
	//				if (!yes) {
	//					assert(false, "Failed Allocating chainData");
	//				}
	//				// initialize data
	//				for (int i = chainData.used; i < chainData.max; i++) {
	//					Chain* temp = chainData.data + i;
	//					*temp = Chain();
	//				}
	//			}
	//			chain->indexNext = chainData.used;
	//			chain = chainData.data + chain->indexNext;
	//			chain->used = false;
	//			chainData.used++;
	//			continue;
	//		} else {
	//			chain = chainData.data + chain->indexNext;
	//			// linked list stuff with overflow data
	//			continue;
	//		}
	//	}
	//	assert(false, "HashMap operator[] bad spot");
	//}
	int HashMap::getSize() {
		return size;
	}
	bool HashMap::sameAs(HashMap& map) {
		if (map.getSize() != getSize())
			return false;

		// what happens if they have differntly sized maps?
		if (keymap.max != map.keymap.max) {
			log::out << log::RED << "HashMap::sameAs : Difference in keymap.max. Missing implementation!\n";
			return false;
		}

		// each chain should have the same pairs
		for (int i = 0; i < keymap.max; i++) {
			Chain* chain = keymap.data + i;
			Chain* chain2 = map.keymap.data + i;

			while (true) {
				if (chain->used != chain2->used)
					return false;

				if (chain->used) {
					if (chain->key != chain2->key)
						return false;
					if (chain->value != chain2->value)
						return false;
				} else {
					break; // neither chain is used we break
				}
				if (chain->indexNext == INVALID_INDEX &&
						chain2->indexNext == INVALID_INDEX)
					break;

				if (chain->indexNext != INVALID_INDEX &&
					chain2->indexNext != INVALID_INDEX) {

					chain = chainData.data + chain->indexNext;
					chain2 = map.chainData.data + chain2->indexNext;
					continue;
				}
				// indexNext is different
				break;
			}
		}
	
		return true; // no differences found so we return
	}
	bool HashMap::copy(HashMap& outMap) {
		outMap.cleanup();

		bool yes = outMap.keymap.resize(keymap.max);
		if (!yes) {
			cleanup();
			return false;
		}
		
		yes = outMap.chainData.resize(chainData.max);
		if (!yes) {
			cleanup();
			return false;
		}

		yes = outMap.emptySpots.copy(emptySpots);
		if (!yes) {
			cleanup();
			return false;
		}
		
		outMap.keymap.used = keymap.used; // not used by keymap
		outMap.chainData.used = chainData.used; // not used by keymap
		
		memcpy(outMap.keymap.data, keymap.data, keymap.max*sizeof(Chain));
		memcpy(outMap.chainData.data, chainData.data, chainData.max*sizeof(Chain));

		outMap.keymapSize = keymapSize;
		outMap.size = size;

		// should not be copied because of unclear behaviour
		// outMap.preventDeconstructor = preventDeconstructor;

		return true;
	}
	void HashMap::IterationInfo::print() {
		printf("Key: %d Value: %d ( Index: %d Depth: %d )\n", key, value, index, depth);

	}
	void HashMap::print() {
		HashMap::IterationInfo info;
		while (iterate(info))
			info.print();
	}
	bool HashMap::iterate(IterationInfo& info) {
		if (keymap.max == 0)
			return false;

		int index = info.position % keymap.max;
		int depth = info.position / keymap.max;
		
		while (index<keymap.max) {
			Chain* chain = keymap.data + index;
			int nowDepth = 0;
			while (chain) {
				if (!chain->used) {
					// end of chain try next index, happens with first chain
					index++;
					depth = 0;
					break;
				}
				if (nowDepth == depth) {
					// found next value
					info.key = chain->key;
					info.value = chain->value;
					info.index = index;
					info.depth = depth;
					depth++; // increase for next time
					info.position = index + depth * keymap.max;
					return true;
				}
				if (chain->indexNext == INVALID_INDEX) {
					// end of chain try next index 
					index++;
					depth = 0;
					break;
				}
				// go deeper into chain
				chain = chainData.data + chain->indexNext;
				nowDepth++;
				continue;
			}
		}
		return false;
	}

	void HashMapTestCase() {
		HashMap map(20);
		HashMap map2(20);

		//map.insert(0, 15);
		//map.insert(100, 24);

		//int val = map.get(0);
		//int val2 = map.get(100);

		//int val = map.get(26);
		//map.insert(26, 20);
		//val = map.get(26);
		//map.erase(26);
		//val = map.get(26);

		map.insert(40, 256);
		map.insert(0, 925);
		map.insert(20, 920);

		map2.insert(0, 925);
		map2.insert(40, 256);
		map2.insert(20, 920);

		//map[25] = 24;
		//map[92] = 92;
		//map[125] = 95;
		//map[99] = 995;
		//map[199] = 1992;

		//HashMap::IterationInfo info2;
		//while (map.iterate(info2))
		//	info2.print();
		//
		//printf("---\n");
		//map.erase(99);

		//int pairs = 20;
		//std::vector<int> keys;
		//for (int i = 0; i < pairs; i++) {
		//	int key = floor(GetRandom() * 20)*10;
		//	int value = 100 + i;// floor(GetRandom() * 10000);
		//	bool yes = map.insert(key,value);
		//	if (yes) {
		//		bool found = false;
		//		for (int k : keys) {
		//			if (k == key) {
		//				found = true;
		//				printf("duplicate %d:%d\n",key,value);
		//				break;
		//			}
		//		}
		//		if(!found)
		//			keys.push_back(key);
		//	}
		//}

		//map.print();
		//for (int i = 0; i < keys.size(); i++) {
		//	//int key = floor(GetRandom() * 10000);
		//	
		//	int value = map.get(keys[i]);
		//	printf("map[%d]= %d\n",keys[i], value);
		//}
		
		//printf("--Keys: %d---\n",keys.size());
		
		//-- ERASE KEYS
		//for (int i = 0; i < keys.size()*0.33; i++) {
		//	int index = floor(GetRandom()* keys.size());
		//	map.erase(keys[index]);
		//	//int value = map.get(keys[index]);
		//	//printf("map[%d]= %d\n", keys[index], value);
		//}
		//for (int i = 0; i < keys.size(); i++) {
		//	//int key = floor(GetRandom() * 10000);
		//	bool boo;
		//	int value = map.get(keys[i],&boo);
		//	if(boo)
		//		printf("map[%d]= %d\n", keys[i], value);
		//}

		map.print();
		map2.print();

		printf("size: %d / %d\n", map.getSize(), map2.getSize());
		printf("same? %d\n", map.sameAs(map2));

		// Todo: Analysis for how many chains there are.
		//		Compare different hash functions and such.
		//		Test sameAs
		// Note: be aware of duplicates when analyzing data
		//		Use a std::vector and add inserted keys to it when testing.
		//		Do not use iterator since it may be buggy. But of course run a bunch of tests on the
		//		Iterator. But if there is something wrong with it you would still be
		//		able to run the other tests without relying on it.


		printf("MemoryUsage: %d\n", map.getMemoryUsage());

		//printf("%d\n",sizeof(HashMap::Chain));

		std::cin.get();
		printf("Finished HashMap Test Case\n");
	}
}