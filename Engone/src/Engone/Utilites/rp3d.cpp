#include "Engone/Utilities/rp3d.h"

#ifndef ENGONE_NO_PHYSICS
/*
	Some useful functions for ReactPhysics3D
*/
namespace engone {
	// turn glm to rp3d, can unfortunately not be constexpr
	rp3d::Vector3 glm_rp3d(const glm::vec3& in) {
		return *(rp3d::Vector3*)&in;
	}
	// may not work properly
	rp3d::Vector3 ToEuler(const rp3d::Quaternion& q) {
		float ang;
		rp3d::Vector3 rot;
		q.getRotationAngleAxis(ang, rot);
		rot *= ang;
		return rot;
	}
}
#endif