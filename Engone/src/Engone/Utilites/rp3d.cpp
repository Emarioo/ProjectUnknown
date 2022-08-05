#include "Engone/Utilities/rp3d.h"

//#include "Engone/Logger.h"

#ifndef ENGONE_NO_PHYSICS
/*
	Some useful functions for ReactPhysics3D
*/
namespace engone {
	// turn glm to rp3d, can unfortunately not be constexpr
	rp3d::Vector3 ToRp3dVec3(const glm::vec3& in) {
		return *(rp3d::Vector3*)&in;
	}
	glm::vec3 ToGlmVec3(const rp3d::Vector3& in) {
		return *(glm::vec3*)&in;
	}
	// may not work properly
	rp3d::Vector3 ToEuler(const rp3d::Quaternion& q) {
		float ang;
		rp3d::Vector3 rot;
		q.getRotationAngleAxis(ang, rot);
		rot *= ang;
		return rot;
	}
	rp3d::Quaternion ToRp3dQuat(const glm::quat& in) {
		return *(rp3d::Quaternion*)&in;
	}
	glm::quat ToGlmQuat(const rp3d::Quaternion& in) {

		//glm::quat q = *(glm::quat*)&in;

		//std::cout << "Q " << in.w << " " << q[3] << "\n";

		return *(glm::quat*)&in;
	}
	glm::mat4 ToMatrix(const rp3d::Transform& t) {
		glm::mat4 out;
		t.getOpenGLMatrix((float*) & out);
		//glm::mat4 mat = glm::translate(ToGlmVec3(t.getPosition())) * glm::mat4_cast(ToGlmQuat(t.getOrientation()));
		return out;
	}
	rp3d::Transform ToTransform(const glm::mat4& m) {
		glm::vec3 skew;
		glm::vec3 scale;
		glm::vec4 pers;
		glm::vec3 pos;
		glm::quat rot;
		//log::out << m << "\n";
		glm::decompose(m, scale, rot, pos, skew, pers);
		rot = glm::conjugate(rot);
		//log::out << rot << "\n";
		glm::mat4 mat = glm::translate(pos) * glm::mat4_cast(rot);

		glm::quat yesh = rot;
		glm::mat4 matq = glm::mat4_cast(rot);
		//log::out << matq<<"\n";
		glm::decompose(matq, scale, rot, pos, skew, pers);
		rot = glm::conjugate(rot);
		glm::mat4 mat2 = glm::mat4_cast(rot);
		//log::out << mat2 << "\n";

		//rot = glm::conjugate(rot);
		//log::out << rot << "\n";

		rp3d::Transform tr;
		tr.setFromOpenGL((float*)&mat);
		//tr.setPosition(ToRp3dVec3(pos));
		//tr.setOrientation(ToRp3dQuat(rot));
		return tr;
	}
	void DecomposeGlm(const glm::mat4& m, glm::vec3* pos, glm::quat* rot, glm::vec3* scale) {
		float garb[4];
		if (!pos) pos = (glm::vec3*)garb;
		if (!rot) rot = (glm::quat*)garb;
		if (!scale) scale = (glm::vec3*)garb;
		glm::decompose(m, *scale, *rot, *pos, *(glm::vec3*)garb, *(glm::vec4*)garb);
		*rot = glm::conjugate(*rot);
	}
}
#endif