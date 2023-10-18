#include "ProUnk/Combat/Tags.h"

namespace prounk {
	
	static std::unordered_map<std::string, TagBit> nameMap;
	static std::unordered_map<TagBit, std::string> bitMap;

	bool AddTag(const std::string& name) {
		//-- tag map is full
		if (nameMap.size() == GAME_TAG_LIMIT) {
			return false;
		}
		//-- find empty bit
		TagBit bit;
		for (int i = 0; i < GAME_TAG_LIMIT;i++) {
			auto findName = bitMap.find(i);
			if (findName == bitMap.end()) {
				bit = i;
				break;
			}
		}
		//-- remove previous tag which would collide with new tag
		auto findName = bitMap.find(bit);
		if (findName != bitMap.end()) {
			if(name!=findName->second)
				nameMap.erase(findName->second);
		}
		//-- set new tag
		nameMap[name] = bit;
		bitMap[bit] = name;
		return true;
	}
	void RemoveTag(const std::string& name) {
		auto find = nameMap.find(name);
		if (find != nameMap.end()) {
			nameMap.erase(name);
			bitMap.erase(find->second);
		}
	}
	void RemoveTag(TagBit bit) {
		auto find = bitMap.find(bit);
		if (find != bitMap.end()) {
			bitMap.erase(bit);
			nameMap.erase(find->second);
		}
	}
	bool Tags::has(const std::string& name) {
		auto find = nameMap.find(name);
		if (find == nameMap.end()) {
			return false;
		}
		return bitTags&find->second;
	}

	//void SetTag(TagBit bit, const std::string& name) {
//	//-- remove previous tag which would collide with new tag
//	auto findName = bitMap.find(bit);
//	if (findName != bitMap.end()) {
//		if(name!=findName->second)
//			nameMap.erase(findName->second);
//	}
//	//-- set new tag
//	nameMap[name] = bit;
//	bitMap[bit] = name;
//}
}