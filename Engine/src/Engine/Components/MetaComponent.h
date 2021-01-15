#pragma once

#include <vector>
#include <string>
/* Examples

velocity
acceleration
polation between to values

type parameters...

polation pos/vel/rot x/y/z fMin fMax fSpeed
velocity

*/
namespace engine {
	enum MetaIdentity {
		X, Y, Z,
		Pos, Rot,
		// Types
		Velocity, // Move Object by f[0] f[1] f[2] in respective direction
		Polation, // Move Object between f[0] and f[1] with f[2] speed
		//Weight, // Describes f[0] weight is required to push this object
		//Clamp, // Clamp value between f[0] and f[1]
		//Reset, // When value is b[0] above or not b[0] below f[0] then set value to f[1]
		//Solid, // Collision b[0] or not b[0]

		None // Used for MetaStripes with less then 3 Types
	};


	class MetaStrip {
	public:
		MetaStrip(MetaIdentity);
		MetaStrip(MetaIdentity, MetaIdentity);
		MetaStrip(MetaIdentity, MetaIdentity, MetaIdentity);
		MetaIdentity types[3];
		//int ints[3]{0,0,0};
		bool bools[1]{ false };
		float floats[3]{ 0,0,0 };
	};


	class MetaComponent {
	public:
		MetaComponent();

		std::vector<MetaStrip> meta;
		int AddMeta(MetaStrip); // TODO: Fix so you can't add the same metadata
		void DelMeta(MetaIdentity, MetaIdentity, MetaIdentity);
		/*
		Can return nullptr
		*/
		MetaStrip* GetMeta(MetaIdentity, MetaIdentity, MetaIdentity);
	};
}