#include "gonpch.h"

#include "RenderComponent.h"

namespace engone {

	void RenderComponent::SetModel(ModelAsset* modelAsset)
	{
		model = modelAsset;
		animator.model = modelAsset;
	}
	void RenderComponent::GetParentTransforms(std::vector<glm::mat4>& mats)
	{
		mats.resize(model->instances.size());
		if (model == nullptr)
			return;

		std::vector<glm::mat4> modelT(model->instances.size());
		//log::out << "go "<< "\n";
		for (int i = 0; i < model->instances.size(); i++) {
			AssetInstance& instance = model->instances[i];
			glm::mat4 loc = model->instances[i].localMat;
			glm::mat4 inv = model->instances[i].invModel;
			
			glm::vec3 pos = { 0,0,0 };
			glm::vec3 euler = { 0,0,0 };
			glm::vec3 scale = { 1,1,1 };
			glm::mat4 quater = glm::mat4(1);

			short usedChannels=0;

			for (int k = 0; k < animator.enabledAnimations.size();k++) {
				AnimationProperty& prop = animator.enabledAnimations[k];
				for (int j = 0; j < model->animations.size(); j++) {
					AnimationAsset* animation = model->animations[j];
					//log::out << "if " << prop.animationName <<" == "<<animation->baseName<<" & "<<prop.instanceName<<" == " <<instance.name<< "\n";
					if (prop.animationName == animation->baseName&&
						prop.instanceName == instance.name) {
					
						if (animation->objects.count(0) > 0) {// the object/instance uses transform object
							
							//log::out << "inst " << i << "\n";
							animation->objects[0].GetValues(prop.frame, prop.blend,
								pos, euler, scale, quater, &usedChannels);
							//log::out << " "<<pos.y <<" " << i << " " << k << " " << j << "\n";
						}
					}
				}
			}
			
			// Hello programmer! If you have issues where the matrix of an object is offset,
			//  you should clear parent inverse in blender. The offset is visible
			//  when using bpy.context.object.matrix_local. It doesn't show up in the viewport.

			// Hello again! Current issue here is the local matrix and animation colliding with each other.
			//  If there is an animation for a channel, then the local matrix should be ignored.
			
			/*
			for (int i = 0; i < 3;i++) {
				if (!((usedChannels<<i) & 1)) {
					pos[i] = loc[3][i];
				}
			}
			// Euler?
			for (int i = 0; i < 3; i++) {
				if (!((usedChannels << (6 + i)) & 1)) {
					scale[i] = glm::length(glm::vec3(loc[i]));
				}
			}
			
			const glm::mat3 rotMtx(
					glm::vec3(loc[0]) / scale[0],
					glm::vec3(loc[1]) / scale[1],
					glm::vec3(loc[2]) / scale[2]);
			glm::quat locQuat = glm::quat_cast(rotMtx);

			const glm::mat3 rotMtx2(
				glm::vec3(quater[0]) / scale[0],
				glm::vec3(quater[1]) / scale[1],
				glm::vec3(quater[2]) / scale[2]);
			glm::quat quat = glm::quat_cast(rotMtx2);

			for (int i = 0; i < 4; i++) {
				if (!((usedChannels << (9 + i)) & 1)) {
					quat[i] = (locQuat[i]);
				}
			}

			glm::mat4 dostuff = quater;//glm::mat4_cast(quat);
			*/
			glm::mat4 ani = glm::translate(glm::mat4(1), pos)
				* quater
				//* glm::rotate(euler.x, glm::vec3(1, 0, 0))
				//* glm::rotate(euler.z, glm::vec3(0, 0, 1))
				//* glm::rotate(euler.y, glm::vec3(0, 1, 0))
				* glm::scale(scale)
				;
			

			if (model->instances[i].parent == -1) {
				modelT[i] = (loc*ani);
				mats[i] = (ani);
				//log::out << loc <<" "<<i << "\n";
			} else {
				modelT[i] = modelT[model->instances[i].parent] * (loc*ani);
				mats[i] = (modelT[model->instances[i].parent] * (ani));
				//log::out << loc << " x " << i << "\n";
			}

			//mats[i] = (modelT[i]);
		}
	}
	void RenderComponent::GetArmatureTransforms(std::vector<glm::mat4>& mats, glm::mat4& instanceMat, AssetInstance& instance, ArmatureAsset* armature) {
		mats.resize(armature->bones.size());
		if (armature != nullptr) {
			std::vector<glm::mat4> modelT(armature->bones.size());
			for (int i = 0; i < armature->bones.size(); i++) {
				Bone& bone = armature->bones[i];
				glm::mat4 loc = bone.localMat;
				glm::mat4 inv = bone.invModel;
				glm::vec3 pos = { 0,0,0 };
				glm::vec3 euler = { 0,0,0 };
				glm::vec3 scale = { 1,1,1 };
				glm::mat4 quater = glm::mat4(1);

				short usedChannels=0;

				for (int k = 0; k < animator.enabledAnimations.size(); k++) {
					AnimationProperty& prop = animator.enabledAnimations[k];
					for (int j = 0; j < model->animations.size(); j++){
						AnimationAsset* animation = model->animations[j];
						if (prop.animationName == animation->baseName&&
							prop.instanceName == instance.name) {
							
							if (animation->objects.count(i) > 0) {
								
								animation->objects[i].GetValues(prop.frame,prop.blend,
									pos, euler, scale, quater, &usedChannels);
								//log::out << quater<<"\n";
							}
						}
					}
				}

				/*
				for (int i = 0; i < 3; i++) {
					if (!((usedChannels << i) & 1)) {
						pos[i] = loc[3][i];
					}
				}
				// Euler?
				for (int i = 0; i < 3; i++) {
					if (!((usedChannels << (6 + i)) & 1)) {
						scale[i] = glm::length(glm::vec3(loc[i]));
					}
				}

				const glm::mat3 rotMtx(
					glm::vec3(loc[0]) / scale[0],
					glm::vec3(loc[1]) / scale[1],
					glm::vec3(loc[2]) / scale[2]);
				glm::quat locQuat = glm::quat_cast(rotMtx);

				const glm::mat3 rotMtx2(
					glm::vec3(quater[0]) / scale[0],
					glm::vec3(quater[1]) / scale[1],
					glm::vec3(quater[2]) / scale[2]);
				glm::quat quat = glm::quat_cast(rotMtx2);

				for (int i = 0; i < 4; i++) {
					if (!((usedChannels << (9 + i)) & 1)) {
						quat[i] = (locQuat[i]);
					}
				}*/

				//glm::mat4 dostuff = glm::mat4_cast(quat);
				
				glm::mat4 ani = glm::translate(glm::mat4(1),pos)
					* quater
					//* glm::rotate(euler.x, glm::vec3(1, 0, 0))
					//* glm::rotate(euler.z, glm::vec3(0, 0, 1))
					//* glm::rotate(euler.y, glm::vec3(0, 1, 0))
					* glm::scale(scale)
					;

				if (i == 0) {
					modelT[i] = (loc*ani);
					//log::out << loc << modelT[i] <<" "<< i << "\n";
				} else {
					modelT[i] = modelT[armature->bones[i].parent] * (loc * ani);
					//log::out << loc <<" x "<<i << "\n";
				}

				mats[i] = (modelT[i]*inv);
			}
		}
	}
	/*
	void RenderComponent::GetArmatureTransforms(std::vector<glm::mat4>& mats, ArmatureAsset* armature) {
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
	*/
}