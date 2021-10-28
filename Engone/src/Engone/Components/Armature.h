#pragma once

#include "../DebugTool/DebugHandler.h"
//#include "Model.h"
#include "Animator.h"
#include <vector>
#include <glm/glm.hpp>

namespace engone {
	class Bone {
	public:
		unsigned short parent = 0;

		glm::mat4 localMat = glm::mat4(1);
		glm::mat4 invModel = glm::mat4(1);

		Bone() {}

	};
	class Armature {
	public:
		std::vector<Bone> bones;

		Armature() {}
		//Model* model;

		/*
		Parameter is an array of glm::mat4 with bones.size()
		*/
		void GetArmatureTransforms(std::vector<glm::mat4>& mats,Animator* animator=nullptr)
		{
#if gone
			//std::cout << "tram " << std::endl;
			if (model != nullptr) {
					//std::cout << model->armature->bones.size() << "\n";
					std::vector<glm::mat4> modelT(bones.size());
					for (int i = 0; i < bones.size(); i++) {
						glm::mat4 loc = bones[i].localMat;
						glm::mat4 inv = bones[i].invModel;
						glm::vec3 pos = { 0,0,0 };
						glm::vec3 euler = { 0,0,0 };
						glm::vec3 scale = { 1,1,1 };
						glm::mat4 quater = glm::mat4(1);
						
						if (animator != nullptr) {
							for (auto& p : animator->enabledAnimations) {
								for (int j = 0; j < model->animations.size(); j++) {
									//std::cout << p.first << " " <<model->animations[j]->name<< std::endl;
									if (p.first == model->animations[j]->name) {
										//std::cout << model->animations[j]->objects.count(i) << std::endl;
										if (model->animations[j]->objects.count(i) > 0) {
											//std::cout << "val " << std::endl;
											model->animations[j]->objects[i].GetValues(p.second.frame, p.second.blend,
												pos, euler, scale, quater);
										}
									}
								}
							}
						}
						//std::cout << "out " << std::endl;
						//if(pos!=glm::vec3(0))
							//bug::out < "Different" < bug::end;
						glm::mat4 ani = glm::translate(glm::mat4(1), pos)
							* quater
							//* glm::rotate(euler.x, glm::vec3(1, 0, 0))
							//* glm::rotate(euler.z, glm::vec3(0, 0, 1))
							//* glm::rotate(euler.y, glm::vec3(0, 1, 0))
							* glm::scale(scale)
							;

						if (i == 0)
							modelT[i] = (loc)*ani;
						else
							modelT[i] = modelT[bones[i].parent] * (loc * ani);

						mats[i] = (modelT[i]) * inv;
				}
			}
#endif;
		}
		bool hasError = false;
	};
}