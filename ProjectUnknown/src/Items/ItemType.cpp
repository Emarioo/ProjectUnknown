#include "ItemType.h"

ItemType::ItemType()
: textureGroup(0),textureIndex(0),name("Unknown"){

}
ItemType::ItemType(char textureGroup, short textureIndex, const std::string& name, short maxCount)
: textureGroup(textureGroup),textureIndex(textureIndex),name(name),maxCount(maxCount) {

}