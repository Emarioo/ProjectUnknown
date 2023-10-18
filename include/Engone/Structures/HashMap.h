#pragma once

#include "Engone/Structures/Stack.h"
#include "Engone/Logger.h"

namespace engone {
	// Integer based hash map meant for few pairs.
	// Few means 100 but more can be used but maybe not optimal.
	// Or maybe it is but that isn't the goal
	// Todo: More testing to ensure that this class is flawless. There may still be issues.
	class HashMap {
	public:
		//typedef int Key;
		//typedef int Value;
		static const int INVALID_INDEX = -1;

		struct IterationInfo {
			u32 key;
			void* value;
			//Key key;
			//Value value;
			int index;
			int depth;
			
			// uses printf
			void print();
		private:
			int position=0;

			friend class HashMap;
		};
		// keymapSize is the size used when allocating initial array where key/values are
		// stored. Duplicates will be stored in a seperate allocation.
		// The size should be high enough to avoid duplicates and small enough to
		// be memory efficient. A fairly good size would be the amount of expected values
		// multiplied by 1.5.
		// keymapSize is just a variable that is set within the class. Nothing else is done in
		// the constructor.
		HashMap(u32 typeSize, u32 allocType, u32 keymapSize=50);
		~HashMap();
		// frees memory and makes the hash map as good as new.
		// even preventDeconstructor
		// keymapSize stays the same
		void cleanup();

		void preventDeconstructor();

		bool insert(u32 key, void* value);
		
		// found is a pointer to a bool which if true indicates the value being found.
		// false means not found.
		void* get(u32 key);
		
		bool erase(u32 key);

		// Primarly works as get. If value isn't found then a value will be created which can
		// then be change with the returned reference.
		// Todo: operator[] to work with both insert and get
		//Value& operator[](int key);

		// Any changes made to hash map will invalidate the position which
		// will cause unexpected results but no crash.
		bool iterate(IterationInfo& info);

		// Equals operator for sameAs?

		// returns the amount of inserted things
		u32 getSize() { return m_size; }

		bool sameAs(HashMap& map);

		u32 getMemoryUsage();

		// outMap will be cleaned and then all data of this map will
		// be copied to outMap. (new allocations will be done making the maps independent)
		// returns false if allocations failed
		bool copy(HashMap& outMap);

		void print();

		void printMemory();


		// Compress may be a better name
		static u32 Hash(const std::string& str);
		static u32 Hash(void* ptr, u32 bytes);

	private:
		// size of this struct is 11 (12 with sizeof(chain)). It used to be 13 (16 with sizeof(Chain)).
		// indexNext is short and next to used to achive the 12 byte size instead of 16.
		struct Chain {
			u32 key;
			short indexNext = INVALID_INDEX; // for overflow and linked list
			bool used=false;
			//u32 value;
			char value[];
		};
		Chain* getDChain(u32 index);
		Chain* getKChain(u32 index);
		
		Memory m_keymap;
		Memory m_chainData;
		Stack m_emptySpots;
		u32 m_keymapSize; // preferred size by the user
		u32 m_size=0; // amount of insertions
		bool m_preventDeconstructor = false;

		u32 m_typeSize;
		u32 m_allocType;

		u32 hash(u32 key);

		// the created chain is reset to default values from Chain()
		// the function returns indexNext
		// -1 is returned if something failed. usually allocation failure.
		u32 createChain();

		friend void HashMapTestCase();
	};

	void HashMapTestCase();
}