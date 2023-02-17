
#include "Engone/AssetModule.h"
#include "Engone/Components/Component.h"

#include "Engone/Utilities/Utilities.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Engone/vendor/stb_image/stb_image.h"
#define GLEW_STATIC
#include <GL/glew.h>

namespace engone {
	TrackerId Texture::trackerId="Texture";
	TrackerId Shader::trackerId="Shader";
	TrackerId Font::trackerId="Font";
	TrackerId MaterialAsset::trackerId="MaterialAsset";
	TrackerId AnimationAsset::trackerId="AnimationAsset";
	TrackerId MeshAsset::trackerId="MeshAsset";
	TrackerId ColliderAsset::trackerId="ColliderAsset";
	TrackerId ArmatureAsset::trackerId="ArmatureAsset";
	TrackerId ModelAsset::trackerId="ModelAsset";

	const char* toString(AssetType type) {
		switch (type) {
		case AssetTexture: return "Texture";
		case AssetFont: return "Font";
		case AssetShader: return "Shader";
		case AssetMaterial: return "Material";
		case AssetMesh: return "Mesh";
		case AssetAnimation: return "Animation";
		case AssetArmature: return "Armature";
		case AssetCollider: return "Collider";
		case AssetModel: return "Model";
		}
		return "";
	}
	Logger& operator<<(Logger& log, AssetType type) {
		return log << toString(type);
	}
	void Font::load(const std::string& path) {
		filePath = path;
		FileReader file(filePath +".txt", false);
		try {
			std::string str;
			file.readAll(&str);
			std::vector<std::string> list = SplitString(str,"\n");

			if (list.size() == 2) {
				charWid[0] = std::stoi(list.at(0));
				int num = std::stoi(list.at(1));
				for (uint32_t i = 1; i < 256; ++i) {
					charWid[i] = num;
				}
			} else {
				uint32_t i = 0;
				for (std::string s : list) {
					std::vector<std::string> list2 = SplitString(s, ",");
					for (std::string s2 : list2) {
						charWid[i] = std::stoi(s2);
						i++;
					}
				}
			}
			texture.load(filePath + ".png");
			if (!texture.valid()) {
				error = texture.error;
			}
		} catch (FileError err) {
			error = err;
		}
	}
	void Font::load(const void* bufferImg,uint32_t size, const void* bufferTxt, uint32_t size2) {

		std::string str;
		str.resize(size2 , 0);
		memcpy(str.data(), bufferTxt, size2);

		std::vector<std::string> list = SplitString(str, "\n");

		if (list.size() == 2) {
			charWid[0] = std::stoi(list.at(0));
			int num = std::stoi(list.at(1));
			for (uint32_t i = 1; i < 256; ++i) {
				charWid[i] = num;
			}
		} else {
			uint32_t i = 0;
			for (std::string s : list) {
				std::vector<std::string> list2 = SplitString(s, ",");
				for (std::string s2 : list2) {
					charWid[i] = std::stoi(s2);
					i++;
				}
			}
		}
		texture.load(bufferImg,size);
		if (!texture.valid()) {
			error = texture.error;
		}
	}
	float Font::getWidth(const std::string& str, float height) {
		float wid=0;
		float maxWidth = 0;
		for (uint32_t i = 0; i < str.length(); ++i) {
			unsigned char chr = str[i];
			if(chr!='\n')
				wid +=  height*(charWid[chr] / (float)charSize);

			if (chr == '\n'||i==str.length()-1) {
				if (maxWidth < wid)
					maxWidth = wid;
				wid = 0;
				continue;
			}
		}
		return maxWidth;
	}
	void MaterialAsset::load(const std::string& path) {
		filePath = path+".material";
		// free/reset data? no need though

		FileReader file(filePath);
		try {
			std::string diffuse_mapName;
			std::string root = getRootPath();
			file.read(&diffuse_mapName);
			//std::cout << "Path: "<< root << diffuse_mapName << "\n";
			if (diffuse_mapName.length() != 0) {
				if (m_parent) {
					diffuse_map = m_parent->set<Texture>(root + diffuse_mapName);
				} else {
					diffuse_map = new Texture();
					diffuse_map->load(root + diffuse_mapName);
					GetTracker().track(diffuse_map);
				}
			}

			//std::cout << "Diffuse "<<diffuse_map << "\n";

			file.read(&diffuse_color);
			file.read(&specular);
			file.read(&shininess);

			//std::cout << diffuse_color[0] << " " << diffuse_color[1] << " " << diffuse_color[2] << "\n";
		}
		catch (FileError err) {
			error = err;
			//Logging({ "AssetManager","Material",toString(err) + ": " + path }, LogStatus::Error);
		}
		//file.close();
		// add diffuse_map as a texture asset
	}
	static MaterialAsset* loaded[4];
	void MaterialAsset::bind(Shader* shader, int index) {
		if (shader != nullptr) {

			//if (this == loaded[index]) return;
			loaded[index] = this;

			if (diffuse_map != nullptr) {
				diffuse_map->bind(index + 1);
				//BindTexture(index + 1, material->diffuse_map);// + 1 because of shadow_map on 0
				//std::cout << "PassMaterial - texture not bound!\n";
				shader->setInt("uMaterials[" + std::to_string(index) + "].diffuse_map", index + 1);
				shader->setInt("uMaterials[" + std::to_string(index) + "].useMap", 1);
			} else {
				shader->setInt("uMaterials[" + std::to_string(index) + "].useMap", 0);
			}
			shader->setVec3("uMaterials[" + std::to_string(index) + "].diffuse_color", diffuse_color);
			shader->setVec3("uMaterials[" + std::to_string(index) + "].specular", specular);
			shader->setFloat("uMaterials[" + std::to_string(index) + "].shininess", shininess);
		} else {
			//std::cout << "shader or material is nullptr in Passmaterial\n";
		}
	}
	Keyframe::Keyframe(PolationType polation, unsigned short frame, float value)
		: polation(polation), frame(frame), value(value)
	{
	}
	FCurve::FCurve() {}
	Keyframe& FCurve::get(int index)
	{
		return frames.at(index);
	}
	void FCurve::add(Keyframe keyframe)
	{
		frames.push_back(keyframe);
	}
	FCurve& Channels::get(ChannelType channel)
	{
		return fcurves[channel];
	}
	void Channels::add(ChannelType channel, FCurve fcurve)
	{
		fcurves[channel] = fcurve;
	}
	void Channels::getValues(int frame, float blend, glm::vec3& pos, glm::vec3& euler, glm::vec3& scale, glm::mat4& quater, short* usedChannels)
	{
		float build[9]{ 0,0,0,0,0,0,1,1,1 };
		glm::quat q0 = glm::quat(1, 0, 0, 0);
		glm::quat q1 = glm::quat(1, 0, 0, 0);

		// q0[3] == q0[].w == 1

		float slerpT = 0;
		for (auto& [type, curve] : fcurves) {
			Keyframe* a = nullptr;
			Keyframe* b = nullptr;
			for (Keyframe& k : curve.frames) {
				if (k.frame == frame) { // the keyframe is at the current time frame.
					a = &k;
					break;
				}
				if (k.frame < frame) { // set a to the keyframe below the time frame at this moment in time.
					a = &k;
					continue;
				}
				if (k.frame > frame) { // set b to the keyframe directly after the time frame.
					if (a == nullptr)
						a = &k;
					else
						b = &k;
					break;
				}
			}
			if (a == nullptr && b == nullptr) { // no keyframes
				//std::cout << "no frames "<< type <<" " << curve.second.frames.size() << std::endl;
				continue;
			}
			if (b == nullptr) { // one keyframe ( "a" )
				//std::cout << "one frame "<<a->frame << std::endl;
				if (type > ScaZ) {
					slerpT = 0;
					q0[type - QuaX] = a->value;
					*usedChannels = *usedChannels | (1 << type);
					continue;
				}
				else {
					build[type] = a->value;
					*usedChannels = *usedChannels | (1 << type);
					continue;
				}
			}
			//log::out << a->frame << " " << b->frame << "\n";
			// lerping between to keyframes
			float lerp = 0;
			if (a->polation == Constant) {
				lerp = 0;
			}
			else if (a->polation == Linear) {
				lerp = ((float)frame - a->frame) / (b->frame - a->frame);
			}
			else if (a->polation == Bezier) {
				lerp = bezier((float)frame, a->frame, b->frame);
			}

			if (type > ScaZ) {
				slerpT = lerp;
				q0[type - QuaX] = a->value;
				q1[type - QuaX] = b->value;
				*usedChannels = *usedChannels | (1 << type);
			}
			else {
				build[type] = a->value * (1 - lerp) + b->value * lerp;
				*usedChannels = *usedChannels | (1 << type);
			}
		}

		pos.x += build[0] * blend;
		pos.y += build[1] * blend;
		pos.z += build[2] * blend;

		euler.x += build[3] * blend;
		euler.y += build[4] * blend;
		euler.z += build[5] * blend;

		scale.x *= build[6] * blend + (1 - blend);
		scale.y *= build[7] * blend + (1 - blend);
		scale.z *= build[8] * blend + (1 - blend);

		//glm::mat4 matis = glm::mat4_cast(glm::slerp(glm::quat(0, 0, 0, 1),glm::slerp(q0, q1, slerpT), 1.f));
		//glm::mat4 matis = glm::mat4_cast(glm::slerp(q0, q1, slerpT));
		//bug::out < matis<"\n";
		//quater *= glm::slerp(q0, q1, slerpT);

		//log::out << q0 <<" - "<<q1 << "\n";

		//std::cout << slerpT << std::endl;

		//bug::outs < "b " <build[1] < "\n";

		//log::out << slerpT << "\n";

		//quater *= glm::toMat4(q0);

		if (q0 == q1 || slerpT == 0) {
			quater *= glm::mat4_cast(glm::normalize(glm::slerp(glm::quat(1, 0, 0, 0), q0, blend)));
		}
		else if (slerpT == 1) {
			quater *= glm::mat4_cast(glm::normalize(glm::slerp(glm::quat(1, 0, 0, 0), q1, blend)));
		}
		else {// Expensive. Is it possible to optimize?
			//log::out << "blend "<<q0 << " "<<q1 <<" "<<blend << "\n";
			quater *= glm::mat4_cast(glm::normalize(glm::slerp(glm::quat(1, 0, 0, 0), glm::slerp(q0, q1, slerpT), blend)));
		}
	}
	Channels& AnimationAsset::get(unsigned short i)
	{
		return objects[i];
	}
	void AnimationAsset::addObjectChannels(int objectIndex, Channels channels)
	{
		objects[objectIndex] = channels;
	}
	void AnimationAsset::modify(unsigned short startFrame, unsigned short endFrame)
	{
		frameStart = startFrame;
		frameEnd = endFrame;
	}
	void AnimationAsset::modify(unsigned short startFrame, unsigned short endFrame, float speed)
	{
		frameStart = startFrame;
		frameEnd = endFrame;
		defaultSpeed = speed;
	}
	void AnimationAsset::load(const std::string& path)
	{
		filePath = path+".animation";
		// clear data
		// clear data in side channels?
		objects.clear();

		FileReader file(filePath);
		try {
			file.read(&frameStart);
			file.read(&frameEnd);
			file.read(&defaultSpeed);
			
			uint8_t objectCount;
			file.read(&objectCount);

			//log::out << "obs " << objectCount << "\n";

			for (uint32_t i = 0; i < objectCount; ++i) {
				uint16_t index, curves;
				
				file.read(&index);
				file.read(&curves);
				
				//log::out << "index " << index << "\n";
				//log::out << "curve " << curves << "\n";

				bool curveB[13]{ 0,0,0,0,0,0,0,0,0,0,0,0,0 };
				for (int j = 12; j >= 0; --j) {
					if (0 <= curves - (uint16_t)pow(2, j)) {
						curves -= (uint16_t)pow(2, j);
						curveB[j] = 1;
					}
					else {
						curveB[j] = 0;
					}
				}

				objects[index] = Channels();
				Channels* channels = &objects[index];

				//const std::string curve_order[]{ "PX","PY","PZ","RX","RY","RZ","SX","SY","SZ","QX","QY","QZ","QW" };
				for (ChannelType cha = PosX; cha < 13; cha = (ChannelType)(cha + 1)) {
					if (curveB[cha]) {
						uint16_t keys;
						file.read(&keys);

						channels->fcurves[cha] = FCurve();
						FCurve* fcurve = &channels->fcurves[cha];

						for (uint32_t k = 0; k < keys; ++k) {
							PolationType polation;
							file.read(&polation); // 1 byte

							uint16_t frame;
							file.read(&frame);

							float value;
							file.read(&value);

							fcurve->frames.push_back(Keyframe(polation, frame, value));
						}
					}
				}
			}
		}
		catch (FileError err) {
			error = err;
			//Logging({ "AssetManager","Animation",toString(err) + ": " + path }, LogStatus::Error);
		}
	}
	void MeshAsset::load(const std::string& path)
	{
		filePath = path + ".mesh";
		// clear data
		materials.clear();
		//buffer.Uninit();

		// NOTE: if possible, clean up this function a bit. There are memory allocations everywhere and a lot of things going on.

		FileReader file(filePath);
		try {
			uint16_t pointCount;
			uint16_t textureCount;
			uint8_t materialCount;
			file.read(&meshType);
			file.read(&pointCount);
			file.read(&textureCount);
			file.read(&materialCount);
			//std::cout << "uhu\n";
			std::string root = getRootPath();
			for (uint32_t i = 0; i < materialCount && i< MeshAsset::maxMaterials; ++i) {
				std::string materialName;
				file.read(&materialName);

				//std::cout << "Matloc: " << root<<materialName<< "\n";
				MaterialAsset* asset;
				if (m_parent) {
					asset = m_parent->set<MaterialAsset>(root + materialName);
				} else {
					asset = new MaterialAsset(root + materialName);
					GetTracker().track(asset);
				}
				if(asset)
					materials.push_back(asset);
				else{
					//std::cout << "Damn error\n";
				}
				
				//std::cout << materials.back()->error << " err\n";
			}
			if (materialCount == 0) {
				MaterialAsset* asset;
				if (m_parent) {
					asset = m_parent->get<MaterialAsset>("defaultMaterial");
					if (!asset) {
						asset = new MaterialAsset();
						m_parent->set<MaterialAsset>("defaultMaterial", asset);
					}
				} else {
					asset = new MaterialAsset();
					GetTracker().track(asset);
				}
				materials.push_back(asset);
			}
			//std::cout << "uh2u\n";
			uint16_t weightCount = 0,triangleCount;
			if(meshType==MeshType::Boned){
				file.read(&weightCount);
			}
			file.read(&triangleCount);
			
			//std::cout << "Points " << pointCount << " Textures " << textureCount <<" Triangles: "<<triangleCount<<" Weights "<<weightCount<<" Mats " << (int)materialCount << "\n";
			uint32_t uPointCount = 3 * pointCount;
			uint32_t uPointSize = sizeof(float)*uPointCount;
			float* uPoint = (float*)Allocate(uPointSize);
			file.read(uPoint, uPointCount);

			uint32_t uTextureCount = textureCount * 3;
			uint32_t uTextureSize = sizeof(float)*uTextureCount;
			
			float* uTexture = (float*)Allocate(uTextureSize);

			file.read(uTexture, uTextureCount);

			// Weight
			uint32_t uWeightS = weightCount * 3;

			int uWeightISize = sizeof(int) * uWeightS;
			int* uWeightI = (int*)Allocate(uWeightISize);

			int uWeightFSize = sizeof(float) * uWeightS;
			float* uWeightF = (float*)Allocate(uWeightFSize);

			if (meshType==MeshType::Boned) {
				char index[3];
				float floats[3];
				for (uint32_t i = 0; i < weightCount; ++i) {
					file.read(index, 3);

					file.read(floats, 3);
					uWeightI[i * 3] = index[0];
					uWeightI[i * 3 + 1] = index[1];
					uWeightI[i * 3 + 2] = index[2];
					uWeightF[i * 3] = floats[0];
					uWeightF[i * 3 + 1] = floats[1];
					uWeightF[i * 3 + 2] = floats[2];
				}
			}

			uint32_t tStride = 6;
			if (meshType==MeshType::Boned)
				tStride = 9;
			uint32_t trisS = triangleCount * tStride;

			int trisSize = sizeof(uint16_t) * trisS;
			uint16_t* tris = (uint16_t*)Allocate(trisSize);
			//std::cout << "head: "<<file.readHead << "\n";

			//file.file.read(reinterpret_cast<char*>(tris), trisS*2);
			file.read(tris, trisS);
			
			//std::cout << "err? " << file.error << "\n";
			//std::cout << file.readHead << " "<<trisS<< "\n";

			//std::cout << "stride " << tStride << "\n";
			for (uint32_t i = 0; i < trisS;++i) {
				 //for (int j = 0; j < tStride;j++) {
					//std::cout << tris[i]<<" ";
				//}
				//std::cout << "\n";
			}

			std::vector<uint16_t> indexNormal;
			std::vector<float> uNormal;
			for (uint32_t i = 0; i < triangleCount; ++i) {
				for (uint32_t j = 0; j < 3; ++j) {
					if (tris[i * tStride + j * tStride / 3] * 3u + 2u >= uPointCount) {
						//std::cout << "Corruption at '" << i <<" "<< (i * tStride)<<" "<<(j * tStride / 3) <<" "<< tris[i*tStride+j*tStride/3] << "' : Triangle Index\n";
						throw FileErrorCorrupted;
					}
				}
				glm::vec3 p0(uPoint[tris[i * tStride + 0 * tStride / 3] * 3 + 0], uPoint[tris[i * tStride + 0 * tStride / 3] * 3 + 1], uPoint[tris[i * tStride + 0 * tStride / 3] * 3 + 2]);
				glm::vec3 p1(uPoint[tris[i * tStride + 1 * tStride / 3] * 3 + 0], uPoint[tris[i * tStride + 1 * tStride / 3] * 3 + 1], uPoint[tris[i * tStride + 1 * tStride / 3] * 3 + 2]);
				glm::vec3 p2(uPoint[tris[i * tStride + 2 * tStride / 3] * 3 + 0], uPoint[tris[i * tStride + 2 * tStride / 3] * 3 + 1], uPoint[tris[i * tStride + 2 * tStride / 3] * 3 + 2]);
				//std::cout << p0.x << " " << p0.y << " " << p0.z << std::endl;
				//std::cout << p1.x << " " << p1.y << " " << p1.z << std::endl;
				//std::cout << p2.x << " " << p2.y << " " << p2.z << std::endl;
				glm::vec3 cro = glm::cross(p1 - p0, p2 - p0);
				//std::cout << cro.x << " " << cro.y << " " << cro.z << std::endl;
				glm::vec3 norm = glm::normalize(cro);
				//std::cout << norm.x << " " << norm.y << " " << norm.z << std::endl;

				bool same = false;
				for (uint32_t j = 0; j < uNormal.size() / 3; ++j) {
					if (uNormal[j * 3 + 0] == norm.x && uNormal[j * 3 + 1] == norm.y && uNormal[j * 3 + 2] == norm.z) {
						same = true;
						indexNormal.push_back((uint16_t)j);
						break;
					}
				}
				if (!same) {
					uNormal.push_back(norm.x);
					uNormal.push_back(norm.y);
					uNormal.push_back(norm.z);
					indexNormal.push_back((uint16_t)(uNormal.size() / 3 - 1));
				}
			}

			std::vector<unsigned short> uniqueVertex;// [ posIndex,colorIndex,normalIndex,weightIndex, ...]

			int triangleOutSize = sizeof(uint32_t) * triangleCount * 3;
			uint32_t* triangleOut = (uint32_t*)Allocate(triangleOutSize);

			uint32_t uvStride = 1 + (tStride) / 3;
			for (uint32_t i = 0; i < triangleCount; ++i) {
				for (uint32_t t = 0; t < 3; ++t) {
					bool same = false;
					for (uint32_t v = 0; v < uniqueVertex.size() / (uvStride); ++v) {
						if (uniqueVertex[v * uvStride] != tris[i * tStride + 0 + t * tStride / 3])
							continue;
						if (uniqueVertex[v * uvStride + 1] != indexNormal[i])
							continue;
						if (uniqueVertex[v * uvStride + 2] != tris[i * tStride + 1 + t * tStride / 3])
							continue;
						if (meshType==MeshType::Boned) {
							if (uniqueVertex[v * uvStride + 3] != tris[i * tStride + 2 + t * tStride / 3])
								continue;
						}
						same = true;
						triangleOut[i * 3 + t] = v;
						break;
					}
					if (!same) {
						triangleOut[i * 3 + t] = uniqueVertex.size() / (uvStride);

						uniqueVertex.push_back(tris[i * tStride + 0 + t * tStride / 3]);
						uniqueVertex.push_back(indexNormal[i]);
						uniqueVertex.push_back(tris[i * tStride + 1 + t * tStride / 3]);
						if (meshType==MeshType::Boned) {
							uniqueVertex.push_back(tris[i * tStride + 2 + t * tStride / 3]);
						}
					}
				}
			}
			/*
			if (bug::is("load_mesh_?")) {
				bug::out < bug::LIME < "  Special" < bug::end;
				for (int i = 0; i < uniqueVertex.size() / (uvStride); i++) {
					for (int j = 0; j < uvStride; j++) {
						bug::out < uniqueVertex[i * uvStride + j];
					}
					bug::out < bug::end;
				}
			}
			*/

			uint32_t vStride = 3 + 3 + 3;
			if (meshType == MeshType::Boned)
				vStride += 6;

			int vertexOutSize = sizeof(float)*(uniqueVertex.size() / uvStride) * vStride;
			float* vertexOut = (float*)Allocate(vertexOutSize);

			for (uint32_t i = 0; i < uniqueVertex.size() / uvStride; i++) {
				// Position
				for (uint32_t j = 0; j < 3; ++j) {
					if (uniqueVertex[i * uvStride] * 3 + j > uPointCount) {
						//bug::out < bug::RED < "Corruption at '" < path < "' : Position Index\n";
						throw FileErrorCorrupted;
					}
					vertexOut[i * vStride + j] = uPoint[uniqueVertex[i * uvStride] * 3 + j];
				}
				// Normal
				for (uint32_t j = 0; j < 3; ++j) {
					if (uniqueVertex[i * uvStride + 1] * 3 + j > uNormal.size()) {
						//bug::out < bug::RED < "Corruption at '" < path < "' : Normal Index\n";
						throw FileErrorCorrupted;
					}
					vertexOut[i * vStride + 3 + j] = uNormal[uniqueVertex[i * uvStride + 1] * 3 + j];
				}
				// UV
				for (uint32_t j = 0; j < 3; ++j) {
					if (uniqueVertex[i * uvStride + 2] * 3 + j > uTextureSize) {
						//bug::out < bug::RED < "Corruption at '" < path < "' : Color Index\n";
						//bug::out < (uniqueVertex[i * uvStride + 2] * 3 + j) < " > " < uTextureSize < bug::end;
						throw FileErrorCorrupted;
					}
					else
						vertexOut[i * vStride + 3 + 3 + j] = (float)uTexture[uniqueVertex[i * uvStride + 2] * 3 + j];
				}
				if (meshType == MeshType::Boned) {
					// Bone Index
					for (uint32_t j = 0; j < 3; ++j) {
						if (uniqueVertex[i * uvStride + 3] * 3 + j > uWeightS) {
							//bug::out < bug::RED < "Corruption at '" < path < "' : Bone Index\n";
							throw FileErrorCorrupted;
						}
						vertexOut[i * vStride + 3 + 3 + 3 + j] = (float)uWeightI[uniqueVertex[i * uvStride + 3] * 3 + j];
					}
					// Weight
					for (uint32_t j = 0; j < 3; ++j) {
						if (uniqueVertex[i * uvStride + 3] * 3 + j > uWeightS) {
							//bug::out < bug::RED < "Corruption at '" < path < "' : Weight Index\n";
							throw FileErrorCorrupted;
						}
						vertexOut[i * vStride + 3 + 3 + 3 + 3 + j] = (float)uWeightF[uniqueVertex[i * uvStride + 3] * 3 + j];
					}
				}
			}

			/*
			for (int i = 0; i < (uniqueVertex.size() / uvStride) * vStride; i++) {
				std::cout << vertexOut[i] << " ";
				if ((i + 1) / (vStride) == (float)(i + 1) / (vStride))
					std::cout << std::endl;
			}
			for (int i = 0; i < triangleCount*3; i++) {
				std::cout << triangleOut[i] << " ";
				if ((i + 1) / (3) == (float)(i + 1) / (3))
					std::cout << std::endl;
			}*/
			vertexBuffer.setData((uniqueVertex.size() / uvStride)* vStride * sizeof(float), vertexOut);
			indexBuffer.setData(triangleCount * 3 * sizeof(float), triangleOut);
			
			vertexArray.addAttribute(3);// Position
			vertexArray.addAttribute(3);// Normal
			if (meshType == MeshType::Boned) {
				vertexArray.addAttribute(3);// Color
				vertexArray.addAttribute(3);// Bone Index
				vertexArray.addAttribute(3,&vertexBuffer);// Weight
			} else {
				vertexArray.addAttribute(3, &vertexBuffer);// Color
				vertexArray.addAttribute(4, 1);
				vertexArray.addAttribute(4, 1);
				vertexArray.addAttribute(4, 1);
				vertexArray.addAttribute(4, 1, nullptr);// empty instance buffer	
			}		

			//buffer.Init(false, vertexOut, (uniqueVertex.size() / uvStride) * vStride, triangleOut, triangleCount * 3);
			//buffer.SetAttrib(0, 3, vStride, 0);// Position
			//buffer.SetAttrib(1, 3, vStride, 3);// Normal
			//buffer.SetAttrib(2, 3, vStride, 3 + 3);// Color
			//if (meshType == MeshType::Boned) {
			//	buffer.SetAttrib(3, 3, vStride, 3 + 3 + 3);// Bone Index
			//	buffer.SetAttrib(4, 3, vStride, 3 + 3 + 3 + 3);// Weight
			//}

			// Cleanup

			//delete[] uPoint;
			//delete[] uTexture;
			//delete[] uWeightI;
			//delete[] uWeightF;
			//delete[] tris;
			//delete[] vertexOut;
			//delete[] triangleOut;

			Free(uPoint, uPointSize);
			Free(uTexture, uTextureSize);
			Free(uWeightI, uWeightISize);
			Free(uWeightF, uWeightFSize);
			Free(tris, trisSize);
			Free(vertexOut, vertexOutSize);
			Free(triangleOut, triangleOutSize);
		}
		catch (FileError err) {/*
			if (err == MissingData) {
				std::cout << "Missing Data\n";
			}
			else if(err==CorruptedData){
				std::cout << "Corrupted Data\n";
			}
			std::cout <<path<<" error "<<err<<"\n";*/
			error = err;
			//Logging({ "AssetManager","Mesh",toString(err) + ": " + path }, LogStatus::Error);
		}
		//file.close();
	}
	void ColliderAsset::load(const std::string& path) {
		filePath = path + ".collider";

		map.heights.clear();
		map.heights.shrink_to_fit();

		FileReader file(filePath);
		try {
			file.read(&colliderType);

			//log::out << (int)colliderType << " type\n";

			switch (colliderType) {
			case CubeType: {
				file.read(&cube.size);
				break;
			}
			case SphereType:{
				file.read(&sphere.radius);
				break;
			}
			case MapType: {
				file.read(&map.gridColumns);
				file.read(&map.gridRows);
				file.read(&map.minHeight);
				file.read(&map.maxHeight);
				file.read(&map.scale);
				uint32_t count = map.gridColumns * map.gridRows;
				if (count > 1000000){
					log::out << "Something may be corrupted\n"; // to prevent a corruption from allocating insane amounts of memory.
					throw FileErrorCorrupted;
				}
				map.heights.resize(count);
				file.read(map.heights.data(), count);

				//uint16_t triCount,pointCount;
				//file.read(&pointCount);
				//file.read(&triCount);
				//file.read(&furthest);

				///*if (triCount > 100000 || pointCount > 100000||furthest<0){
				//	log::out << "something may be corrupted\n";
				//	throw CorruptedData;
				//}*/

				//points.resize(pointCount);
				//tris.resize(triCount*3);
				//for (uint32_t i = 0; i < points.size();++i) {
				//	file.read(&points[i]);
				//}
				//for (uint32_t i = 0; i < tris.size(); ++i) {
				//	file.read(&tris[i]);
				//}

				break;
			}
			case CapsuleType: {
				file.read(&capsule.radius);
				file.read(&capsule.height);
				break;
			}
			}
		}
		catch (FileError err) {
			error = err;
			//Logging({ "AssetManager","Collider",toString(err) + ": " + path }, LogStatus::Error);
		}
	}
	void ArmatureAsset::load(const std::string& path){
		filePath = path + ".armature";
		bones.clear();

		FileReader file(filePath);
		try {
			uint8_t boneCount;
			file.read(&boneCount);

			//log::out << path << "\n";
			//log::out << boneCount << "\n";
	
			// Acquire and Load Data
			for (uint32_t i = 0; i < boneCount; ++i) {
				Bone b;
				file.read(&b.parent);
				//log::out << b.parent << "\n";
				//log::out << i<<" matrix" << "\n";
				file.read(&b.localMat);
				file.read(&b.invModel);

				bones.push_back(b);
			}
		}
		catch (FileError err) {
			error = err;
			//Logging({ "AssetManager","Armature",toString(err) + ": " + path }, LogStatus::Error);
		}
		//file.close();
	}
	void ModelAsset::load(const std::string& path) {
		filePath = path+".model";

		instances.clear();
		animations.clear();

		FileReader file(filePath);
		try {
			std::string root = getRootPath();

			uint16_t instanceCount;
			file.read(&instanceCount);
			for (uint32_t i = 0; i < instanceCount; ++i) {
				instances.push_back({});
				AssetInstance& instance = instances.back();

				file.read(&instance.name);

				uint8_t instanceType;
				file.read(&instanceType);

				std::string name;
				file.read(&name);

				//std::cout << "intance "<<name << "\n";

				switch (instanceType) {
				case 0:
					if (m_parent) {
						instance.asset = m_parent->set<MeshAsset>(root + name);
					} else {
						instance.asset = new MeshAsset(root + name);
						GetTracker().track((MeshAsset*)instance.asset);
					}
					break;
				case 1:
					if (m_parent) {
						instance.asset = m_parent->set<ArmatureAsset>(root + name);
					} else {
						instance.asset = new ArmatureAsset(root + name);
						GetTracker().track((ArmatureAsset*)instance.asset);
					}
					break;
				case 2:
					if (m_parent) {
						instance.asset = m_parent->set<ColliderAsset>(root + name);
					} else {
						instance.asset = new ColliderAsset(root + name);
						GetTracker().track((ColliderAsset*)instance.asset);
					}
					break;
				}

				file.read(&instance.parent);
				file.read(&instance.localMat);
				//file.read(&instance.invModel);
			}
			
			uint16_t animationCount;
			file.read(&animationCount);
			for (uint32_t i = 0; i < animationCount; ++i) {
				std::string name;
				file.read(&name);
				if (m_parent) {
					animations.push_back(m_parent->set<AnimationAsset>(root + name));
				} else {
					// ISSUE: Memory leak, this is not owned by assets, since assets doesn't exist, but it is still created.
					// ModelAsset should destroy it but how would it know if it has "ownership". vector with bool for each animations?
					AnimationAsset* as = new AnimationAsset(root + name);
					animations.push_back(as);
				}
			}
		}
		catch (FileError err) {
			error = err;
			//Logging({ "AssetManager","Model",toString(err) + ": " + path }, LogStatus::Error);
		}
	}
	std::vector<glm::mat4> ModelAsset::getParentTransforms(Animator* animator) {
		std::vector<glm::mat4> mats(instances.size());
		std::vector<glm::mat4> modelT(instances.size());

		for (uint32_t i = 0; i < instances.size(); ++i) {
			AssetInstance& instance = instances[i];
			glm::mat4 loc = instances[i].localMat;
			glm::mat4 inv = instances[i].invModel;

			glm::vec3 pos = { 0,0,0 };
			glm::vec3 euler = { 0,0,0 };
			glm::vec3 scale = { 1,1,1 };
			glm::mat4 quater = glm::mat4(1);

			short usedChannels = 0;

			if (animator) {
				for (uint32_t k = 0; k < Animator::maxAnimations; ++k) {
					if (animator->enabledAnimations[k].asset) {
						AnimatorProperty& prop = animator->enabledAnimations[k];
						for (uint32_t j = 0; j < animations.size(); ++j) {
							AnimationAsset* animation = animations[j];
							//log::out << "if " << prop.animationName <<" == "<<animation->baseName<<" & "<<prop.instanceName<<" == " <<instance.name<< "\n";
							if (prop.asset == animation &&
								prop.instanceName == instance.name) {

								if (animation->objects.count(0) > 0) {// the object/instance uses transform object

									//log::out << "inst " << i << "\n";
									animation->objects[0].getValues((int)prop.frame, prop.blend,
										pos, euler, scale, quater, &usedChannels);
									//log::out << " "<<pos.y <<" " << i << " " << k << " " << j << "\n";
								}
							}
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

			glm::mat4 ani = glm::mat4(1);
			if (animator) {
				ani = glm::translate(glm::mat4(1), pos)
					* quater
					//* glm::rotate(euler.x, glm::vec3(1, 0, 0))
					//* glm::rotate(euler.z, glm::vec3(0, 0, 1))
					//* glm::rotate(euler.y, glm::vec3(0, 1, 0))
					* glm::scale(scale)
					;
			}

			if (instances[i].parent == -1) {
				modelT[i] = (loc * ani);
				mats[i] = (ani);
				//log::out << loc <<" "<<i << "\n";
			} else {
				modelT[i] = modelT[instances[i].parent] * (loc * ani);
				mats[i] = (modelT[instances[i].parent] * (ani));
				//log::out << loc << " x " << i << "\n";
			}

			//mats[i] = (modelT[i]);
		}
		return mats;
	}
	std::vector<glm::mat4> ModelAsset::getArmatureTransforms(Animator* animator, glm::mat4& instanceMat, AssetInstance* instance, ArmatureAsset* armature,std::vector<glm::mat4>* boneMats) {
		std::vector<glm::mat4> mats(armature->bones.size());
		if (armature != nullptr) {
			std::vector<glm::mat4> modelT(armature->bones.size());

			for (uint32_t i = 0; i < armature->bones.size(); ++i) {
				Bone& bone = armature->bones[i];
				glm::mat4 loc = bone.localMat;
				glm::mat4 inv = bone.invModel;
				glm::vec3 pos = { 0,0,0 };
				glm::vec3 euler = { 0,0,0 };
				glm::vec3 scale = { 1,1,1 };
				glm::mat4 quater = glm::mat4(1);

				short usedChannels = 0;

				for (uint32_t k = 0; k < Animator::maxAnimations; ++k) {
					if (animator->enabledAnimations[k].asset) {

						AnimatorProperty& prop = animator->enabledAnimations[k];
						for (uint32_t j = 0; j < animations.size(); ++j) {
							AnimationAsset* animation = animations[j];
							
							if (prop.asset == animation &&
								prop.instanceName == instance->name) {

								if (animation->objects.count((uint16_t)i) > 0u) {

									animation->objects[(uint16_t)i].getValues((int)prop.frame, prop.blend,
										pos, euler, scale, quater, &usedChannels);
									//log::out << " " << pos.y << " " << i << " " << k << " " << j << "\n";
									//log::out << quater<<"\n";
								}
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

				glm::mat4 ani = glm::translate(glm::mat4(1), pos)
					* quater
					//* glm::rotate(euler.x, glm::vec3(1, 0, 0))
					//* glm::rotate(euler.z, glm::vec3(0, 0, 1))
					//* glm::rotate(euler.y, glm::vec3(0, 1, 0))
					* glm::scale(scale)
					;

				if (i == 0) {
					modelT[i] = (loc * ani);
					//log::out << loc << modelT[i] <<" "<< i << "\n";
				} else {
					modelT[i] = modelT[armature->bones[i].parent] * (loc * ani);
					//log::out << loc <<" x "<<i << "\n";
				}

				mats[i] = (modelT[i] * inv);
			}
			if (boneMats) {
				boneMats->resize(armature->bones.size());
				memcpy_s(boneMats->data(), boneMats->size() * sizeof(glm::mat4), modelT.data(), modelT.size() * sizeof(glm::mat4));
			}
		}
		return mats;
	}
	Assets::~Assets() {
		removeAssets<Texture>();
		removeAssets<Font>();
		removeAssets<Shader>();
		removeAssets<MaterialAsset>();
		removeAssets<MeshAsset>();
		removeAssets<AnimationAsset>();
		removeAssets<ArmatureAsset>();
		removeAssets<ColliderAsset>();
		removeAssets<ModelAsset>();
		// the lists should destroy themselves.
	}
}