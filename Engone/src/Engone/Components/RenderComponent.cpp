#include "gonpch.h"

#include "RenderComponent.h"

namespace engone {

	void RenderComponent::SetModel(const std::string& name) {
		Model* mod = GetModelAsset(name);
		if (mod != nullptr) {
			model = mod;
			animator.model = mod;
		}
		else hasError = true;
	}
	void RenderComponent::GetArmatureTransforms(std::vector<glm::mat4>& mats) {
		//std::cout << "tram " << std::endl;
		if (model != nullptr) {
			if (model->armature != nullptr) {
				//std::cout << model->armature->bones.size() << "\n";
				std::vector<glm::mat4> modelT(model->armature->bones.size());
				for (int i = 0; i < model->armature->bones.size(); i++) {
					glm::mat4 loc = model->armature->bones[i].localMat;
					glm::mat4 inv = model->armature->bones[i].invModel;
					glm::vec3 pos = { 0,0,0 };
					glm::vec3 euler = { 0,0,0 };
					glm::vec3 scale = { 1,1,1 };
					glm::mat4 quater = glm::mat4(1);

					for (auto& p : animator.enabledAnimations) {
						for (int j = 0; j < model->animations.size(); j++){
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
					//std::cout << "out " << std::endl;
					//if(pos!=glm::vec3(0))
						//bug::out < "Different" < bug::end;
					glm::mat4 ani = glm::translate(glm::mat4(1),pos)
						* quater
						//* glm::rotate(euler.x, glm::vec3(1, 0, 0))
						//* glm::rotate(euler.z, glm::vec3(0, 0, 1))
						//* glm::rotate(euler.y, glm::vec3(0, 1, 0))
						* glm::scale(scale)
						;

					if (i == 0)
						modelT[i] = (loc)*ani;
					else
						modelT[i] = modelT[model->armature->bones[i].parent] * (loc * ani);

					mats[i] = (modelT[i]) * inv;
				}
			}
		}
	}
}