#include "FileManager.h"
#include <sys/stat.h>

#include "AssetManager.h"

#define error(x) if(x==true){data->hasError=true;return;}

namespace engine {

	int FileExist(const std::string& path) {
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}
	int WriteTextFile(const std::string& path_, std::vector<std::string> text) {
		std::string path = path_+".txt";
		std::ofstream file(path);
		std::vector<std::string> out;
		if (!file) {
			bug::out < bug::RED < "Cannot find file '" < path < "'\n";
			return NotFound;
		} else {
			for (int i = 0; i < text.size(); i++) {
				if (i != 0)
					file << "\n";
				file << text[i];
			} 
		}
		file.close();
		return Success;
	}

	std::vector<std::string> ReadFileList(const std::string& path_, int* err) {
		*err = Success;
		std::string path = path_+ ".txt";
		std::ifstream file(path);

		std::vector<std::string> out;
		if (!file) {
			*err = NotFound;
			bug::out < bug::RED <"Cannot find file '" < path  <"'\n";
			return out;
		}
		std::string line;
		while (getline(file, line))
		{
			out.push_back(line);
		}
		file.close();
		return out;
	}/*
	template<class T>
	T ReadT(std::ifstream* f) {
		T buf;
		f->read(reinterpret_cast<char*>(&buf), sizeof(T));
		return buf;
	}*/
	std::ifstream* fileToRead=nullptr;
	int fileAtByte = 0;
	int fileSize = 0;
	std::string filePath;
	void ReadFromFile(std::ifstream* f,std::string p) {
		if (f != nullptr) {
			fileAtByte = 0;
			fileSize = 0;
			filePath = p;
			fileToRead = f;
			f->seekg(0, f->end);
			fileSize = f->tellg();
			f->seekg(0, f->beg);
		}
	}
	/*
	Return the first read data
	*/
	template<typename T>
	bool Read(T* buffer,size_t count) {
		if (fileToRead != nullptr) {
			fileAtByte += sizeof(T) * count;
			if (fileAtByte > fileSize) {
				bug::out < bug::RED < "Corruption at '" < filePath < "' : missing " < (fileAtByte - fileSize) < " bytes\n";
				return true;
			}
			fileToRead->read(reinterpret_cast<char*>(&buffer[0]), sizeof(T) * count);
			return false;
		}
		return false;
	}
	template<typename T>
	bool Read(T* buffer) {
		if (fileToRead != nullptr) {
			fileAtByte += sizeof(T);
			if (fileAtByte > fileSize) {
				bug::out < bug::RED < "Corruption at '" < filePath < "' : missing " < (fileAtByte - fileSize) < " bytes\n";
				return true;
			}
			fileToRead->read(reinterpret_cast<char*>(&buffer[0]), sizeof(T));
			return false;
		}
		return false;
	}

	std::string ReadFile(std::string path, int* err) {
		*err = Success;
		path  += ".txt";
		std::ifstream file(path);
		if (!file) {
			*err = NotFound;
			bug::out < bug::RED < "Cannot find '" < path < "'\n";
			return "";
		}
		std::string text;
		std::string line;
		while (getline(file, line)) {
			text += line;
		}
		file.close();
		return text;
	}
	void LoadMaterial(Material* data, const std::string& path_) {
		if (data == nullptr)
			return;
		std::string path = "assets/materials/" + path_ + ".material";
		std::ifstream file(path, std::ios::binary);
		ReadFromFile(&file, path);
		if (!file) {
			file.close();
			bug::out < bug::RED < "Cannot find '" < path < "'\n";
			return;
		}

		bool dGeneral = bug::is("LoadMaterial");

		unsigned char lenMap;
		error(Read<unsigned char>(&lenMap));

		char* tempMap = new char[lenMap];
		error(Read<char>(tempMap, lenMap));
		if (lenMap > 0) {
			data->diffuse_map = "";
			for (int i = 0; i < lenMap; i++)
				data->diffuse_map += tempMap[i];
		}
		if (dGeneral) bug::outs < "Name:" < data->diffuse_map < bug::end;

		float d[3];
		error(Read<float>(d,3));
		data->diffuse_color = { d[0],d[1],d[2] };
		if (dGeneral) bug::outs < "Color:" < data->diffuse_color < bug::end;
		

		error(Read<float>(d,3));
		data->specular = { d[0],d[1],d[2] };
		if (dGeneral) bug::outs < "Specular:" < data->specular < bug::end;

		error(Read<float>(&data->shininess));
		if (dGeneral) bug::outs < "Shininess:" < data->shininess < bug::end;

		// Cleanup
		delete tempMap;
		file.close();

		// Dependecies
		if (data->diffuse_map!="blank")
			AddTextureAsset(data->diffuse_map);

	}
	void LoadMesh(Mesh* data, const std::string& path_) {
		if (data == nullptr)
			return;
		std::string path = "assets/meshes/"+path_+".mesh";
		std::ifstream file(path, std::ios::binary);
		ReadFromFile(&file,path);
		if (!file) {
			file.close();
			bug::out < bug::RED < "Cannot find '" < path < "'\n";
			return;
		}

		bool dGeneral = bug::is("LoadMesh");
		bool dAll = bug::is("LoadMesh.All");
		bool dWeights = bug::is("LoadMesh.Weights");
		bool dTriangles = bug::is("LoadMesh.Triangles");
		bool dVectors = bug::is("LoadMesh.Vectors");
		bool dColors = bug::is("LoadMesh.Colors");
		bool dNormals = bug::is("LoadMesh.Normals");

		unsigned char useArmature;
		error(Read<unsigned char>(&useArmature));
		if (dAll||dGeneral) bug::outs < "Armature:" < useArmature < bug::end;
	
		unsigned short pointCount;
		error(Read<unsigned short>(&pointCount));
		if (dAll||dGeneral) bug::outs < "Points:" < pointCount < bug::end;

		unsigned short textureCount;
		error(Read<unsigned short>(&textureCount));
		if (dAll||dGeneral) bug::outs < "Colors:" < textureCount < bug::end;
		
		unsigned char materialCount;
		error(Read<unsigned char>(&materialCount));
		if (dAll || dGeneral) bug::outs < "Materials:" < materialCount < bug::end;

		std::vector<std::string> materials;
		for (int i = 0; i < materialCount;i++) {
			unsigned char lenName;
			error(Read<unsigned char>(&lenName));
			
			char* materialName = new char[lenName];
			error(Read<char>(materialName,lenName));
			std::string name;
			for (int j = 0; j < lenName; j++)
				name += materialName[j];
			if (dAll || dGeneral) bug::outs < " Material"<i<":" < lenName< name < bug::end;

			materials.push_back(name);
		}
		/*
		int cStride = 3;
		std::string texName="blank";
		if (useTexture) {
			cStride = 2;
			unsigned char texLen;
			error(Read<unsigned char>(&texLen));
			if (bug::is("load_mesh_info"))
				bug::outs < "TextureLen:" < texLen < bug::end;
			char* name = new char[texLen];
			
			error(Read<char>(name, texLen));
			for (int i = 0; i < texLen; i++) {
				texName += name[i];
			}
			if (bug::is("load_mesh_info"))
				bug::outs < "Texture:" < texName < bug::end;
			delete name;
		}
		*/
		unsigned short weightCount=0;
		if (useArmature) {
			error(Read<unsigned short>(&weightCount));
			if (dAll || dWeights) bug::outs < "Weights:" < weightCount < bug::end;
		}
		unsigned short triangleCount;
		error(Read<unsigned short>(&triangleCount));
		if (dAll || dTriangles) bug::outs < "Triangles:" < triangleCount < bug::end;
		
		int uPointSize = 3 * pointCount;
		float* uPoint = new float[uPointSize];
		error(Read<float>(uPoint, uPointSize));
		if (dAll || dVectors) {
			bug::out < bug::LIME < "  Vectors\n";
			for (int i = 0; i < 3 * pointCount; i++) {
				bug::out < uPoint[i] < " ";
				if ((i + 1) / (3) == (float)(i + 1) / (3))
					bug::out < bug::end;
			}
		}

		int uTextureSize = textureCount * 3;
		float* uTexture = new float[uTextureSize];
		error(Read<float>(uTexture, uTextureSize));
		if (dAll || dColors) {
			bug::out < bug::LIME < "  Colors\n";
			for (int i = 0; i < textureCount * 3; i++) {
				bug::out < uTexture[i] < " ";
				if ((i + 1) / (3) == (float)(i + 1) / (3))
					bug::out < bug::end;
			}
		}

		// Weight
		int uWeightS = weightCount * 3;
		int* uWeightI = new int[uWeightS];
		float* uWeightF = new float[uWeightS];
		if (useArmature) {
			if (dAll || dWeights)
				bug::out < bug::LIME < "  Weights\n";
			for (int i = 0; i < weightCount; i++) {
				char index[3];
				float floats[3];
				error(Read<char>(index, 3));
				
				error(Read<float>(floats, 3));
				uWeightI[i * 3] = index[0];
				uWeightI[i * 3 + 1] = index[1];
				uWeightI[i * 3 + 2] = index[2];
				uWeightF[i * 3] = floats[0];
				uWeightF[i * 3 + 1] = floats[1];
				uWeightF[i * 3 + 2] = floats[2];
				if (dAll || dWeights)
					bug::outs < (int)uWeightI[i * 3] < (int)uWeightI[i * 3 + 1] < (int)uWeightI[i * 3 + 2] < uWeightF[i * 3] < uWeightF[i * 3 + 1] < uWeightF[i * 3 + 2] < bug::end;
			}
		}

		int tStride = 6;
		if (useArmature)
			tStride = 9;
		int trisS = triangleCount * tStride;
		unsigned short* tris = new unsigned short[trisS];
		error(Read<unsigned short>(tris, trisS));
		
		if (dAll || dTriangles) {
			bug::out <bug::LIME< "  Triangles\n";
			for (int i = 0; i < tStride * triangleCount; i++) {
				bug::out < tris[i] < " ";
				if ((i + 1) / (tStride) == (float)(i + 1) / (tStride))
					bug::out < bug::end;
			}
		}

		std::vector<unsigned short> indexNormal;
		std::vector<float> uNormal;
		for (int i = 0; i < triangleCount; i++) {
			for (int j = 0; j < 3 * 3; j++) {
				if (tris[i * tStride + j / 3 * 2] * 3 + j - j / 3 * 3 > uPointSize) {
					bug::out < bug::RED + "Corruption at '" + path < "' : Triangle Index\n";
					data->hasError = true;
					return;
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
			for (int j = 0; j < uNormal.size() / 3; j++) {
				if (uNormal[j * 3 + 0] == norm.x && uNormal[j * 3 + 1] == norm.y && uNormal[j * 3 + 2] == norm.z) {
					same = true;
					indexNormal.push_back(j);
					break;
				}
			}
			if (!same) {
				uNormal.push_back(norm.x);
				uNormal.push_back(norm.y);
				uNormal.push_back(norm.z);
				indexNormal.push_back(uNormal.size() / 3 - 1);
			}
		}
		if (dAll || dNormals) {
			bug::out < bug::LIME < "  Normals\n";
			for (int i = 0; i < uNormal.size(); i++) {
				bug::out < uNormal[i] < " ";
				if ((i + 1) / (3) == (float)(i + 1) / (3))
					bug::out < bug::end;
			}
		}
		
		std::vector<unsigned short> uniqueVertex;// [ posIndex,colorIndex,normalIndex,weightIndex, ...]
		unsigned int* triangleOut = new unsigned int[triangleCount * 3];

		int uvStride = 1+(tStride) / 3;
		for (int i = 0; i < triangleCount; i++) {
			for (int t = 0; t < 3; t++) {
				bool same = false;
				for (int v = 0; v < uniqueVertex.size() / (uvStride); v++) {
					if (uniqueVertex[v* uvStride] != tris[i * tStride + 0 + t * tStride / 3])
						continue;
					if (uniqueVertex[v * uvStride + 1] != indexNormal[i])
						continue;
					if (uniqueVertex[v * uvStride + 2] != tris[i * tStride + 1 + t * tStride / 3])
						continue;
					if (useArmature) {
						if (uniqueVertex[v * uvStride + 3] != tris[i * tStride + 2 + t * tStride / 3])
							continue;
					}
					same = true;
					triangleOut[i * 3 + t] = v;
					break;
				}
				if (!same) {
					triangleOut[i * 3 + t] = uniqueVertex.size()/(uvStride);

					uniqueVertex.push_back(tris[i * tStride + 0 + t * tStride / 3]);
					uniqueVertex.push_back(indexNormal[i]);
					uniqueVertex.push_back(tris[i * tStride + 1 + t * tStride / 3]);
					if (useArmature) {
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

		int vStride = 3 + 3 + 3;
		if (useArmature)
			vStride += 6;
		float* vertexOut = new float[(uniqueVertex.size() / uvStride) * vStride];
		for (int i = 0; i < uniqueVertex.size() / uvStride; i++) {
			// Position
			for (int j = 0; j < 3; j++) {
				if (uniqueVertex[i * uvStride] * 3 + j > uPointSize) {
					bug::out < bug::RED < "Corruption at '" < path < "' : Position Index\n";
					data->hasError=true;
					return;
				}
				vertexOut[i * vStride + j] = uPoint[uniqueVertex[i * uvStride] * 3 + j];
			}
			// Normal
			for (int j = 0; j < 3; j++) {
				if (uniqueVertex[i * uvStride + 1] * 3 + j > uNormal.size()) {
					bug::out < bug::RED < "Corruption at '" < path < "' : Normal Index\n";
					data->hasError = true;
					return;
				}
				vertexOut[i * vStride + 3 + j] = uNormal[uniqueVertex[i * uvStride + 1] * 3 + j];
			}
			// UV
			for (int j = 0; j < 3; j++) {
				if (uniqueVertex[i * uvStride + 2] * 3 + j > uTextureSize) {
					bug::out < bug::RED < "Corruption at '" < path < "' : Color Index\n";
					bug::out < (uniqueVertex[i * uvStride + 2] * 3 + j) < " > " < uTextureSize < bug::end;
					data->hasError = true;
					return;
				}else
					vertexOut[i * vStride + 3 + 3 + j] = uTexture[uniqueVertex[i * uvStride + 2] * 3 + j];
			}
			if (useArmature) {
				// Bone Index
				for (int j = 0; j < 3; j++) {
					if (uniqueVertex[i * uvStride + 3] * 3 + j > uWeightS) {
						bug::out < bug::RED < "Corruption at '" < path < "' : Bone Index\n";
						data->hasError = true;
						return;
					}
					vertexOut[i * vStride + 3 + 3 + 3 + j] = uWeightI[uniqueVertex[i * uvStride + 3] * 3 + j];
				}
				// Weight
				for (int j = 0; j < 3; j++) {
					if (uniqueVertex[i * uvStride + 3] * 3 + j > uWeightS) {
						bug::out < bug::RED < "Corruption at '" < path < "' : Weight Index\n";
						data->hasError = true;
						return;
					}
					vertexOut[i * vStride + 3 + 3 + 3 + 3 + j] = uWeightF[uniqueVertex[i * uvStride + 3] * 3 + j];
				}
			}
		}

		if (dAll || dGeneral) {
			bug::outs < bug::LIME < "VertexBuffer " < (uniqueVertex.size() / uvStride) < "*" < (vStride) < " IndexBuffer " < (triangleCount) < "*3" < bug::end;
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
		
		file.close();
		
		if (useArmature == 0) {
			data->shaderType = ShaderColor;
		} else if (useArmature == 1) {
			data->shaderType = ShaderColorBone;
		}
		for (std::string& mat : materials) {
			AddMaterialAsset(mat);
			data->materials.push_back(GetMaterialAsset(mat));
		}
		
		data->container.Setup(false, vertexOut, (uniqueVertex.size() / uvStride) * vStride, triangleOut, triangleCount * 3);
		data->container.SetAttrib(0, 3, vStride, 0);// Position
		data->container.SetAttrib(1, 3, vStride, 3);// Normal
		data->container.SetAttrib(2, 3, vStride, 3 + 3);// Color
		if (useArmature) {
			data->container.SetAttrib(3, 3, vStride, 3 + 3 + 3);// Bone Index
			data->container.SetAttrib(4, 3, vStride, 3 + 3 + 3 + 3);// Weight
		}

		// Cleanup
		delete uPoint;
		delete uTexture;
		delete uWeightI;
		delete uWeightF;
		delete tris;
		delete vertexOut;
		delete triangleOut;
	}
	void LoadAnimation(Animation* data, const std::string& path_) {
		if (data == nullptr)
			return;
		std::string path = "assets/animations/" + path_ + ".animation";
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			file.close();
			bug::out < bug::RED < "Cannot find '" < path < "'\n";
			return;
		}

		bool dGeneral = bug::is("LoadAnimation");
		bool dAll = bug::is("LoadAnimation.All");
		bool dFrames = bug::is("LoadAnimation.Frames");

		ReadFromFile(&file, path);
		
		data->name = path_;

		error(Read<unsigned short>(&data->frameStart));

		error(Read<unsigned short>(&data->frameEnd));
		if (dAll||dGeneral) bug::out < "Start-End: " < data->frameStart <"-"< data->frameEnd < bug::end;

		error(Read<float>(&data->defaultSpeed));
		if (dAll || dGeneral) bug::out < "Speed: " < data->defaultSpeed < bug::end;
		
		unsigned char objects;
		error(Read<unsigned char>(&objects));
		if (dAll || dGeneral) bug::out < "Object: " < (int)objects < bug::end;
		
		for (int i = 0; i < objects; i++) {
		
			int index;
			error(Read<int>(&index));

			unsigned short curves;
			error(Read<unsigned short>(&curves));

			if (dAll || dFrames) std::cout << "Object " << i << " " << curves << std::endl;

			bool curveB[13]{ 0,0,0,0,0,0,0,0,0,0,0,0,0 };
			for (int j = 12; j >= 0; j--) {
				if (0  <= curves - pow(2, j)) {
					curves -= pow(2, j);
					curveB[j] = 1;
				} else {
					curveB[j] = 0;
				}
			}

			data->objects[index]=FCurves();
			FCurves* fCurves = &data->objects[index];

			for (int j = 0; j < 13; j++) {
				if (curveB[j]) {
					unsigned short keys;
					error(Read<unsigned short>(&keys));
					if (dAll || dFrames) std::cout << " Curve " << j << " " << (int)keys << std::endl;
					
					fCurves->fcurves[j] = FCurve();
					FCurve* fcurve = &fCurves->fcurves[j];

					for (int k = 0; k < keys; k++) {
						char polation;
						error(Read<char>(&polation));
						
						unsigned short frame;
						error(Read<unsigned short>(&frame));

						float value;
						error(Read<float>(&value));
						if (dAll || dFrames) bug::outs < "  Key" < polation < frame < value < bug::end;
						
						fcurve->frames.push_back(Keyframe(polation, frame, value));
					}
				}
			}
		}

		// Cleanup
		file.close();
	}
	void LoadCollider(Collider* data, const std::string& path_) { // TODO: More efficient by removing unneccecery for loops
		std::string path = "assets/colliders/" + path_ + ".collider";
		std::ifstream file(path, std::ios::binary);
		ReadFromFile(&file,path);
		
		bool dGeneral = bug::is("LoadCollider");
		bool dAll = bug::is("LoadCollider.All");
		bool dVectors = bug::is("LoadCollider.Vectors");
		bool dQuads = bug::is("LoadCollider.Quads");

		std::uint16_t vC;
		error(Read<std::uint16_t>(&vC));
		if (dGeneral || dAll) std::cout << "Points: " << vC << std::endl;
		
		std::uint16_t qC;
		error(Read<std::uint16_t>(&qC));
		if (dGeneral || dAll) std::cout << "Quad: " << qC << std::endl;
		
		/* Not supported - REMEMBER TO DELETE WHEN TRIANGLES ARE SUPPORTED
		std::uint16_t tC;
		if (Read<std::uint16_t>(&vC)) return Corrupt;
		if (debug) std::cout << "Triangles: " << vC << std::endl;
		*/

		float furthest;
		error(Read<float>(&furthest));
		if (dGeneral || dAll) std::cout << "Furthest: " << furthest << std::endl;

		data->furthestPoint = furthest;

		int vS = vC * 3;
		int qS = qC * 4;
		// int tS = tC * 3;

		float* uV = new float[vS];
		error(Read<float>(uV,vS));

		std::uint16_t* uQ = new std::uint16_t[qS];
		error(Read<std::uint16_t>(uQ, qS));
		/*
		std::uint16_t* uT = new std::uint16_t[tS];
		if (Read<std::uint16_t>(uT, tS)) return Corrupt;
		*/
		if (dGeneral || dVectors) std::cout << "Vectors" << std::endl;
		for (int i = 0; i < vC; i++) {
			data->points.push_back(glm::vec3(uV[i * 3], uV[i * 3 + 1], uV[i * 3 + 2]));
			if (dGeneral || dVectors) std::cout << uV[i * 3] << " " << uV[i * 3 + 1] << " " << uV[i * 3 + 2] << std::endl;
		}
		if (dGeneral || dQuads) std::cout << "Quads" << std::endl;;
		for (int i = 0; i < qC; i++) {
			for (int j = 0; j < 4; j++) {
				data->quad.push_back(uQ[i * 4 + j]);
				if (dGeneral || dQuads) std::cout << uQ[i * 4 + j] << " ";
			}
			if (dGeneral || dQuads) std::cout << std::endl;
		}
		/*
		if (debug) std::cout << "Triangles" << std::endl;;
		for (int i = 0; i < tC; i++) {
			for (int j = 0; j < 3; j++) {
				da->quad.push_back(uT[i]);
				if (debug) std::cout << uQ[i * 3 + j] << " ";
			}
			if (debug) std::cout << std::endl;
		}
		*/
		// Cleanup
		delete uV;
		delete uQ;
		//delete uT; REMEMBER TO ADD THIS WHEN TRIANGLES ARE SUPPORTED
		file.close();
	}
	void LoadArmature(Armature* data, const std::string& path_) {
		std::string path = "assets/armatures/" + path_ + ".armature";
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			file.close();
			bug::out < bug::RED < "Cannot find '" < path < "'\n";
			data->hasError = true;
			return;
		}

		bool dGeneral = bug::is("LoadArmature");
		bool dAll = bug::is("LoadArmature.All");
		bool dBones = bug::is("LoadArmature.Bones");
		bool dMatrix = bug::is("LoadArmature.Matrix");

		ReadFromFile(&file, path);

		unsigned char boneCount;
		error(Read<unsigned char>(&boneCount));
		if (dGeneral||dAll) bug::out < "Bone Count: " < (int)boneCount < bug::end;

		// Acquire and Load Data
		for (int i = 0; i < boneCount; i++) {
			Bone b;
			error(Read<int>(&b.parent));
			if (dBones||dAll) bug::out < " Parent: " < (int)b.parent < bug::end;
			
			for (int x = 0; x < 4; x++){
				for (int y = 0; y < 4; y++) {
					error(Read<float>(&b.localMat[x][y]));
				}
			}
			if (dMatrix||dAll) bug::out < b.localMat < bug::end;
			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < 4; y++) {
					error(Read<float>(&b.invModel[x][y]));
				}
			}
			if (dMatrix||dAll) bug::out < b.invModel < bug::end;
			
			data->bones.push_back(b);
		}
		// Cleanup
		file.close();
	}
	void LoadModel(Model* data, const std::string& path_) {
		std::string path = "assets/models/" + path_ + ".model";
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			file.close();
			bug::out < bug::RED < "Cannot find '" < path < "'\n";
			data->hasError = true;
			return;
		}

		bool dGeneral = bug::is("LoadModel");
		bool dAll = bug::is("LoadModel.All");
		bool dMesh = bug::is("LoadModel.Mesh");
		bool dMatrix = bug::is("LoadModel.Matrix");

		ReadFromFile(&file, path);

		unsigned char lenArmature;
		error(Read<unsigned char>(&lenArmature));
		if (dGeneral||dAll) bug::out < "Armature Len: " < (int)lenArmature < bug::end;
		
		std::string armName = "";
		if (lenArmature>0) {
			char* cArmature = new char[lenArmature];
			error(Read<char>(cArmature, lenArmature));
			for (int j = 0; j < lenArmature; j++) {
				armName += cArmature[j];
			}
			if (dGeneral || dAll) bug::out < "Armature Name: " < armName < bug::end;
		}

		unsigned char animCount;
		error(Read<unsigned char>(&animCount));
		if (dGeneral || dAll) bug::out < "Animation Count: " < (int)animCount < bug::end;
		
		std::vector<std::string> animations;
		for (int i = 0; i < animCount; i++) {
			unsigned char lenName;
			error(Read<unsigned char>(&lenName));
			//if (bug::is("load_model_info")) bug::out < "Model Name Len: " < (int)lenName < bug::end;

			char* cName = new char[lenName];
			error(Read<char>(cName, lenName));
			std::string name = "";
			for (int j = 0; j < lenName; j++) {
				name += cName[j];
			}
			if (dGeneral || dAll) bug::out < "Animation Name: " < name < bug::end;
			animations.push_back(name);
		}

		unsigned char meshCount;
		error(Read<unsigned char>(&meshCount));
		if (dGeneral || dAll) bug::out < "Mesh Count: " < (int)meshCount < bug::end;

		std::vector<std::string> meshes;
		std::vector<glm::mat4> matrices;
		for (int i = 0; i < meshCount;i++) {
			unsigned char lenName;
			error(Read<unsigned char>(&lenName));
			//if (bug::is("load_model_mesh")) bug::out < "Model Name Len: " < (int)lenName < bug::end;

			char* cName=new char[lenName];
			error(Read<char>(cName, lenName));
			std::string name = "";
			for (int j = 0; j < lenName;j++) {
				name += cName[j];
			}
			if (dMesh || dAll) bug::out < "Model Name: " < name < bug::end;

			glm::mat4 mat(1);
			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < 4; y++) {
					error(Read<float>(&mat[x][y]));
				}
			}
			if (dMatrix||dAll) bug::out < mat < bug::end;

			meshes.push_back(name);
			matrices.push_back(mat);
		}

		unsigned char lenCollider;
		error(Read<unsigned char>(&lenCollider));
		if (dGeneral||dAll) bug::out < "Collider Len: " < (int)lenCollider < bug::end;


		std::string colliderName;
		if (lenCollider > 0) {
			char* cCollider = new char[lenCollider];
			error(Read<char>(cCollider, lenCollider));
			for (int j = 0; j < lenCollider; j++) {
				colliderName += cCollider[j];
			}
			if (dGeneral||dAll) bug::out < "Collider Name: " < colliderName < bug::end;
		}
	
		// Cleanup
		file.close();

		// Dependecies
		if (!armName.empty()) {
			AddArmatureAsset(armName);
			data->SetArmature(armName);
		}
		for (int i = 0; i < animations.size(); i++) {
			AddAnimationAsset(animations[i]);
			data->AddAnimation(animations[i]);
		}
		for (int i = 0; i < meshes.size();i++) {
			AddMeshAsset(meshes[i]);
			data->AddMesh(meshes[i], matrices[i]);
		}
		if (!colliderName.empty()) {
			AddColliderAsset(colliderName);
			data->SetCollider(colliderName);
		}
	}
}