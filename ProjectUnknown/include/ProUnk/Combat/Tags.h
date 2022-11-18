#pragma once

/*
	Tag system for items and objects in the game.

	Static functions such as AddTag, RemoveTag can be used to manage what tags there are in the game.

	The current system is very simple. There is a limit of 32 types of tags. This can be increased to 64 with uint64_t.
	You can change how things works behind the scenes and possibly make a more complex and efficient system but this works for now.

	First you use AddTag to add the tags that exist in the game. Fire, Water, Item, Magical, Metallic, Organic, Bloody...
	Then you can use a Tags member variable for the items and entities you want to have tags.
	Then you can use 'set' and 'has' member variables on the tag to set and check what tags an item or creature has.

	One problem may be if you want more complicated things. Say you want an item which is a backpack. How do you deal with that?
	Where is that inventory stored?

	Consider moving this description to Obsidian.

*/

namespace prounk {

#define OBJECT_IS_DEAD 0x00200000

#define GAME_TAG_LIMIT 32
	typedef uint32_t TagBit;

	class Tags {
	public:

		TagBit bitTags;

		bool has(const std::string& name);
		inline bool has(TagBit bit) { return bitTags & bit; }
		void set(const std::string& name, bool active);
	};

	// returns false if there is no bit slot for the tag. consider removing a tag
	bool AddTag(const std::string& name);
	//void SetTag(TagBit bit, const std::string& name);
	void RemoveTag(const std::string& name);
	void RemoveTag(TagBit bit);
}