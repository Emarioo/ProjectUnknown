#pragma once
#ifndef ENGONE_NO_PHYSICS
#include "reactphysics3d/reactphysics3d.h"
/*
	Some useful functions for ReactPhysics3D
*/
namespace engone {
	// turn glm to rp3d, can unfortunately not be constexpr
	rp3d::Vector3 glm_rp3d(const glm::vec3& in);
	// turn rp3d to glm
	constexpr glm::vec3 rp3d_glm(const rp3d::Vector3& in) {
		return *(glm::vec3*)&in;
	}
	// may not work properly
	rp3d::Vector3 ToEuler(const rp3d::Quaternion& q);
}
#endif