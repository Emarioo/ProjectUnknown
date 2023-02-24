
#include "Engone/Structures/HashMap.h"

#include "Engone/Utilities/RandomUtility.h"

namespace engone {

	HashMap::HashMap(uint32 typeSize, uint32 allocType, uint32 keymapSize) : 
		m_typeSize(typeSize), m_allocType(allocType), m_keymapSize(keymapSize), 
		m_keymap(sizeof(Chain)+typeSize,allocType), m_chainData(sizeof(Chain)+typeSize,allocType), m_emptySpots(sizeof(uint32),allocType) {

	}
	HashMap::~HashMap() {
		if(!m_preventDeconstructor)
			cleanup();
	}
	void HashMap::preventDeconstructor() {
		m_preventDeconstructor = true;
	}
	void HashMap::cleanup() {
		m_keymap.resize(0);
		m_chainData.resize(0);
		m_emptySpots.cleanup();
		m_size = 0;
		m_preventDeconstructor = false;
	}
	uint32 HashMap::getMemoryUsage() {
		return m_keymap.max*m_keymap.getTypeSize() + m_chainData.max * m_chainData.getTypeSize()+m_emptySpots.size()*sizeof(uint32);
	}
	uint32 HashMap::hash(uint32 key) {
		// will crash if keymap.max is 0.
		uint32 index = key % m_keymap.max; // yes, you should laugh
		return index;
	}
	uint32 HashMap::createChain() {
		uint32 indexNext=0;
		if (m_emptySpots.size() != 0) {
			m_emptySpots.pop(&indexNext);
			Chain* chain = getDChain(indexNext);
			*chain = Chain();
		} else {
			if (m_chainData.used == m_chainData.max) {
				bool yes = m_chainData.resize((m_chainData.max) * 2 + 4);
				if (!yes)
					return -1; // failed allocating

				// initialize data
				for (int i = m_chainData.used; i < m_chainData.max; i++) {
					Chain* temp = getDChain(i);
					*temp = Chain();
				}
			}
			indexNext = m_chainData.used;
			Chain* chain = getDChain(indexNext);
			*chain = Chain();
			m_chainData.used++;
		}
		return indexNext;
	}
	bool HashMap::insert(uint32 key, void* value) {
		// Check for enough memory
		if (!m_keymap.data) {
			bool yes = m_keymap.resize(m_keymapSize);
			if (!yes)
				return false; // return if failed resize
			
			// initialize data
			for (int i = 0; i< m_keymap.max; i++) {
				Chain* chain = getKChain(i);
				*chain = Chain();
			}
		}
		
		int index = hash(key);

		Chain* chain = getKChain(index);
		while (true) {
			printf("Loop %u %u %u\n", chain->key, *(uint32*)chain->value, chain->indexNext);
			if (!chain->used) {
				chain->used = true;
				chain->key = key;
				//chain->value = value;
				if(value)
					memcpy(chain->value,value,m_typeSize);
				else
					memset(chain->value, 0, m_typeSize);
				chain->indexNext = INVALID_INDEX;
				printf("v1 %u %u\n", chain->key ,*(uint32*)chain->value);
				m_size++;
				return true;
			} else if (chain->key == key) {
				if(value)
					memcpy(chain->value, value, m_typeSize);
				else
					memset(chain->value, 0, m_typeSize);
				printf("v2 %u %u\n", chain->key, *(uint32*)chain->value);
				//chain->value = value;
				return true;
			} else if(key<chain->key) { // this ensures that chains are sorted by the key variable
				// create chain for later use
				int indexNext = createChain();
				if (indexNext == -1)
					return false;

				// move the chain to be replaced
				Chain* newChain = getDChain(indexNext);
				//*newChain = *chain;
				memcpy(newChain, chain, m_chainData.getTypeSize());
				printf("v3 %u %u\n", newChain->key, *(uint32*)newChain->value);

				// replace old chain with new chain
				chain->used = true;
				chain->key = key;
				//chain->value = value;
				if(value)
					memcpy(chain->value, value, m_typeSize);
				else
					memset(chain->value, 0, m_typeSize);
					
				chain->indexNext = indexNext;
				m_size++;
				printf("v4 %u %u\n", chain->key, *(uint32*)chain->value);

				return true;
				// chain could be keymap so chain needs to be replaced by new pair.
				// the old chain needs to be attached afterwards.
			} else if (chain->indexNext == INVALID_INDEX) {
				int indexNext = createChain();
				if (indexNext == -1)
					return false;

				chain->indexNext = indexNext;
				chain = getDChain(indexNext);
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
				chain = getDChain(chain->indexNext);
				// linked list stuff with overflow data
				continue;
			}
		}
		return false;
	}
	HashMap::Chain* HashMap::getDChain(uint32 index) {
		return (Chain*)((char*)m_chainData.data + index * m_chainData.getTypeSize());
	}
	HashMap::Chain* HashMap::getKChain(uint32 index) {
		return (Chain*)((char*)m_keymap.data + index * m_keymap.getTypeSize());
	}
	void HashMap::printMemory() {
		//for (int i = 0; i < m_keymap.max;i++) {
			//Chain* chain = (Chain*)((char)m_keymap.data + i*m_keymap.getTypeSize());
		//}
	}
	void* HashMap::get(uint32 key) {
		if (m_keymap.max == 0) {
			return 0;
		}

		int index = hash(key);

		Chain* chain = getKChain(index);
		while (true) {
			if (chain->used) {
				if (chain->key == key) {
					return chain->value;
				} else if (chain->indexNext == INVALID_INDEX) {
					break; // failed
				} else {
					chain = getDChain(chain->indexNext);
					continue;
				}
			} else {
				break; // failed
			}
		}
		return 0;
	}
	bool HashMap::erase(uint32 key) {
		if (m_keymap.max == 0) {
			return false;
		}

		int index = hash(key);

		Chain* firstChain = getKChain(index);
		Chain* chain = firstChain;
		while (chain) {
			if (!chain->used) 
				return false;
			
			if (chain->key == key) {
				if (chain->indexNext == INVALID_INDEX) {
					*chain = Chain();
				} else {
					uint32 indexNext = chain->indexNext;

					bool yes = m_emptySpots.push(&indexNext);
					if (!yes) {
						//return false;  // uncomment to force efficient use of memory
					}

					Chain* nextChain = getDChain(chain->indexNext);
					*chain = *nextChain;

					*nextChain = Chain();
				}
				m_size--;
				return true;
			}
			
			if (chain->indexNext==INVALID_INDEX)
				return false;

			chain = getDChain(chain->indexNext);
		}
		
		return false;
	}
	//HashMap::Value& HashMap::operator[](int key) {
	// USE getDChain! 
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
	bool HashMap::sameAs(HashMap& map) {
		if (map.getSize() != getSize())
			return false;

		// what happens if they have differntly sized maps?
		if (m_keymap.max != map.m_keymap.max) {
			log::out << log::RED << "HashMap::sameAs : Difference in keymap.max. Implement this?\n";
			return false;
		}
		if (m_typeSize != m_typeSize) {
			log::out << log::RED << "HashMap::sameAs : Difference in typeSize.\n";
			return false;
		}

		// each chain should have the same pairs
		for (int i = 0; i < m_keymap.max; i++) {
			Chain* chain = getKChain(i);
			Chain* chain2 = (Chain*)map.getKChain(i);

			while (true) {
				if (chain->used != chain2->used)
					return false;

				if (chain->used) {
					if (chain->key != chain2->key)
						return false;
					for (int i = 0;i< m_typeSize;i++) {
						if (((char*)chain->value)[i] != ((char*)chain2->value)[i])
							return false;
					}
				} else {
					break; // neither chain is used we break
				}
				if (chain->indexNext == INVALID_INDEX &&
						chain2->indexNext == INVALID_INDEX)
					break;

				if (chain->indexNext != INVALID_INDEX &&
					chain2->indexNext != INVALID_INDEX) {

					chain = getDChain(chain->indexNext);
					chain2 = (Chain*)map.getDChain(chain2->indexNext);
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

		bool yes = outMap.m_keymap.resize(m_keymap.max);
		if (!yes) {
			cleanup();
			return false;
		}
		
		yes = outMap.m_chainData.resize(m_chainData.max);
		if (!yes) {
			cleanup();
			return false;
		}

		yes = outMap.m_emptySpots.copy(m_emptySpots);
		if (!yes) {
			cleanup();
			return false;
		}
		
		outMap.m_keymap.used = m_keymap.used; // not used by keymap
		outMap.m_chainData.used = m_chainData.used; // not used by keymap
		
		memcpy(outMap.m_keymap.data, m_keymap.data, m_keymap.max*m_keymap.getTypeSize());
		memcpy(outMap.m_chainData.data, m_chainData.data, m_chainData.max * m_keymap.getTypeSize());

		outMap.m_keymapSize = m_keymapSize;
		outMap.m_size = m_size;

		// should not be copied because of unclear behaviour
		// outMap.preventDeconstructor = preventDeconstructor;

		return true;
	}
	void HashMap::IterationInfo::print() {
		printf("Key: %d Value: %d ( Index: %d Depth: %d )\n", key, *(uint32*)value, index, depth);

	}
	void HashMap::print() {
		HashMap::IterationInfo info;
		while (iterate(info))
			info.print();
	}
	bool HashMap::iterate(IterationInfo& info) {
		if (m_keymap.max == 0)
			return false;

		int index = info.position % m_keymap.max;
		int depth = info.position / m_keymap.max;
		
		while (index< m_keymap.max) {
			Chain* chain = getKChain(index);
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
					info.position = index + depth * m_keymap.max;
					return true;
				}
				if (chain->indexNext == INVALID_INDEX) {
					// end of chain try next index 
					index++;
					depth = 0;
					break;
				}
				// go deeper into chain
				chain = getDChain(chain->indexNext);
				nowDepth++;
				continue;
			}
		}
		return false;
	}

	void HashMapTestCase() {
		HashMap map(sizeof(uint32),ALLOC_TYPE_HEAP, 20);
		HashMap map2(sizeof(uint32), ALLOC_TYPE_HEAP, 20);

		//map.insert(0, 15);
		//map.insert(100, 24);

		//int val = map.get(0);
		//int val2 = map.get(100);

		//int val = map.get(26);
		//map.insert(26, 20);
		//val = map.get(26);
		//map.erase(26);
		//val = map.get(26);

#define MAP_INSERT(M,K,V) tmp=V;M.insert(K,&tmp);

		uint32 tmp;
		MAP_INSERT(map,40,256);
		MAP_INSERT(map,0,925);
		MAP_INSERT(map,20,920);
		MAP_INSERT(map2,0,925);
		MAP_INSERT(map2,40,256);
		MAP_INSERT(map2,20,920);

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


		printf("MemoryUsage: %d, %d\n", map.getMemoryUsage(), map2.getMemoryUsage());

		//printf("%d\n",sizeof(HashMap::Chain));

		std::cin.get();
		printf("Finished HashMap Test Case\n");
	}
}