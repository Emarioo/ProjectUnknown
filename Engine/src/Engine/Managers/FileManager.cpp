#include "FileManager.h"
#include <sys/stat.h>

namespace engine {

	int FileExist(std::string path) {
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}
	int WriteTextFile(std::string path, std::vector<std::string> text) {
		path += ".txt";
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

	std::vector<std::string> ReadFileList(std::string path, int* err) {
		*err = Success;
		path  += ".txt";
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
	int LoadMesh(MeshData* da, std::string path) {
		path = "assets/meshes/"+path+".mesh";
		bool debug = false;
		std::ifstream file(path, std::ios::binary);
		ReadFromFile(&file,path);
		if (!file) {
			file.close();
			bug::out < bug::RED < "Cannot find '" < path < "'\n";
			return NotFound;
		}

		unsigned char type;
		if(Read<unsigned char>(&type)) return Corrupt;
		if (debug) bug::outs < "Type:" < type < bug::end;
		bool useWeight = false;
		bool useTexture = false;
		if (type == 0) {

		} else if (type == 1) {
			useTexture = true;
		} else if (type == 2) {
			useWeight = true;
		} else if (type == 3) {
			useWeight = true;
			useTexture = true;
		}
	
		unsigned short posC;
		if (Read<unsigned short>(&posC)) return Corrupt;
		if (debug) bug::outs < "Points:" < posC < bug::end;

		unsigned short colorC;
		if (Read<unsigned short>(&colorC)) return Corrupt;
		if (debug) bug::outs < "Colors:" < colorC < bug::end;
		
		int cStride = 3;
		std::string texName;
		if (useTexture) {
			cStride = 2;
			unsigned char texLen;
			if (Read<unsigned char>(&texLen)) return Corrupt;
			if (debug)
				bug::outs < "TextureLen:" < texLen < bug::end;
			char* name = new char[texLen];
			
			if(Read<char>(name, texLen)) return Corrupt;
			for (int i = 0; i < texLen; i++) {
				texName += name[i];
			}
			if (debug)
				bug::outs < "Texture:" < texName < bug::end;
			delete name;
		}
		unsigned short weightC=0;
		if (useWeight) {
			if (Read<unsigned short>(&weightC)) return Corrupt;
			if (debug) bug::outs < "Weights:" < weightC < bug::end;
		}
		unsigned short triC;
		if(Read<unsigned short>(&triC)) return Corrupt;
		if (debug) bug::outs < "Triangles:" < triC < bug::end;
		
		int uPosS = 3 * posC;
		float* uPos = new float[uPosS];
		if(Read<float>(uPos, uPosS)) return Corrupt;
		if (debug&&false) {
			bug::out < bug::LIME < "  Vectors\n";
			for (int i = 0; i < 3 * posC; i++) {
				bug::out < uPos[i] < " ";
				if ((i + 1) / (3) == (float)(i + 1) / (3))
					bug::out < bug::end;
			}
		}

		int uColorS = colorC * cStride;
		float* uColor = new float[uColorS];
		if (Read<float>(uColor, uColorS)) return Corrupt;
		if (debug) {
			bug::out < bug::LIME < "  Colors\n";
			for (int i = 0; i < colorC * cStride; i++) {
				bug::out < uColor[i] < " ";
				if ((i + 1) / (cStride) == (float)(i + 1) / (cStride))
					bug::out < bug::end;
			}
		}

		// Weight
		int uWeightS = weightC * 3;
		int* uWeightI = new int[uWeightS];
		float* uWeightF = new float[uWeightS];
		if (useWeight) {
			if (debug)
				bug::out < bug::LIME < "  Weights\n";
			for (int i = 0; i < weightC; i++) {
				char index[3];
				float floats[3];
				if (Read<char>(index, 3)) return Corrupt;
				
				if (Read<float>(floats, 3)) return Corrupt;
				uWeightI[i * 3] = index[0];
				uWeightI[i * 3 + 1] = index[1];
				uWeightI[i * 3 + 2] = index[2];
				uWeightF[i * 3] = floats[0];
				uWeightF[i * 3 + 1] = floats[1];
				uWeightF[i * 3 + 2] = floats[2];
				if (debug)
					bug::outs < (int)uWeightI[i * 3] < (int)uWeightI[i * 3 + 1] < (int)uWeightI[i * 3 + 2] < uWeightF[i * 3] < uWeightF[i * 3 + 1] < uWeightF[i * 3 + 2] < bug::end;
			}
		}

		int tStride = 6;
		if (useWeight)
			tStride = 9;
		int trisS = triC * tStride;
		unsigned short* tris = new unsigned short[trisS];
		if (Read<unsigned short>(tris, trisS)) return Corrupt;
		
		if (debug) {
			bug::out <bug::LIME< "  Triangles\n";
			for (int i = 0; i < tStride * triC; i++) {
				bug::out < tris[i] < " ";
				if ((i + 1) / (tStride) == (float)(i + 1) / (tStride))
					bug::out < bug::end;
			}
		}

		std::vector<unsigned short> indexNormal;
		std::vector<float> uniqueNormal;
		for (int i = 0; i < triC; i++) {
			for (int j = 0; j < 3 * 3; j++) {
				if (tris[i * tStride + j / 3 * 2] * 3 + j - j / 3 * 3 > uPosS) {
					bug::out < bug::RED + "Corruption at '" + path < "' : Triangle Index\n";
					return Corrupt;
				}
			}
			glm::vec3 p0(uPos[tris[i * tStride + 0 * tStride / 3] * 3 + 0], uPos[tris[i * tStride + 0 * tStride / 3] * 3 + 1], uPos[tris[i * tStride + 0 * tStride / 3] * 3 + 2]);
			glm::vec3 p1(uPos[tris[i * tStride + 1 * tStride / 3] * 3 + 0], uPos[tris[i * tStride + 1 * tStride / 3] * 3 + 1], uPos[tris[i * tStride + 1 * tStride / 3] * 3 + 2]);
			glm::vec3 p2(uPos[tris[i * tStride + 2 * tStride / 3] * 3 + 0], uPos[tris[i * tStride + 2 * tStride / 3] * 3 + 1], uPos[tris[i * tStride + 2 * tStride / 3] * 3 + 2]);
			//std::cout << p0.x << " " << p0.y << " " << p0.z << std::endl;
			//std::cout << p1.x << " " << p1.y << " " << p1.z << std::endl;
			//std::cout << p2.x << " " << p2.y << " " << p2.z << std::endl;
			glm::vec3 cro = glm::cross(p1 - p0, p2 - p0);
			//std::cout << cro.x << " " << cro.y << " " << cro.z << std::endl;
			glm::vec3 norm = glm::normalize(cro);
			//std::cout << norm.x << " " << norm.y << " " << norm.z << std::endl;

			bool same = false;
			for (int j = 0; j < uniqueNormal.size() / 3; j++) {
				if (uniqueNormal[j * 3 + 0] == norm.x && uniqueNormal[j * 3 + 1] == norm.y && uniqueNormal[j * 3 + 2] == norm.z) {
					same = true;
					indexNormal.push_back(j);
					break;
				}
			}
			if (!same) {
				uniqueNormal.push_back(norm.x);
				uniqueNormal.push_back(norm.y);
				uniqueNormal.push_back(norm.z);
				indexNormal.push_back(uniqueNormal.size() / 3 - 1);
			}
		}
		if (debug)
		{
			bug::out < bug::LIME < "  Normals\n";
			for (int i = 0; i < uniqueNormal.size(); i++) {
				bug::out < uniqueNormal[i] < " ";
				if ((i + 1) / (3) == (float)(i + 1) / (3))
					bug::out < bug::end;
			}
		}
		
		std::vector<unsigned short> uniqueVertex;// [ posIndex,colorIndex,normalIndex,weightIndex, ...]
		unsigned int* tout = new unsigned int[triC * 3];

		int uvStride = 1+(tStride) / 3;
		for (int i = 0; i < triC; i++) {
			for (int t = 0; t < 3; t++) {
				bool same = false;
				for (int v = 0; v < uniqueVertex.size() / (uvStride); v++) {
					if (uniqueVertex[v* uvStride] != tris[i * tStride + 0 + t * tStride / 3])
						continue;
					if (uniqueVertex[v * uvStride + 1] != indexNormal[i])
						continue;
					if (uniqueVertex[v * uvStride + 2] != tris[i * tStride + 1 + t * tStride / 3])
						continue;
					if (useWeight) {
						if (uniqueVertex[v * uvStride + 3] != tris[i * tStride + 2 + t * tStride / 3])
							continue;
					}
					same = true;
					tout[i * 3 + t] = v;
					break;
				}
				if (!same) {
					tout[i * 3 + t] = uniqueVertex.size()/(uvStride);

					uniqueVertex.push_back(tris[i * tStride + 0 + t * tStride / 3]);
					uniqueVertex.push_back(indexNormal[i]);
					uniqueVertex.push_back(tris[i * tStride + 1 + t * tStride / 3]);
					if (useWeight) {
						uniqueVertex.push_back(tris[i * tStride + 2 + t * tStride / 3]);
					}
				}
			}
		}
		if (debug) {
			bug::out < bug::LIME < "  Special" < bug::end;
			for (int i = 0; i < uniqueVertex.size() / (uvStride); i++) {
				for (int j = 0; j < uvStride; j++) {
					bug::out < uniqueVertex[i * uvStride + j];
				}
				bug::out < bug::end;
			}
		}

		int vStride = 3 + cStride + 3;
		if (useWeight)
			vStride += 6;
		float* vout = new float[(uniqueVertex.size() / uvStride) * vStride];
		for (int i = 0; i < uniqueVertex.size() / uvStride; i++) {
			// Position
			for (int j = 0; j < 3; j++) {
				if (uniqueVertex[i * uvStride] * 3 + j > uPosS) {
					bug::out < bug::RED < "Corruption at '" < path < "' : Position Index\n";
					return Corrupt;
				}
				vout[i * vStride + j] = uPos[uniqueVertex[i * uvStride] * 3 + j];
			}
			// Normal
			for (int j = 0; j < 3; j++) {
				if (uniqueVertex[i * uvStride + 1] * 3 + j > uniqueNormal.size()) {
					bug::out < bug::RED < "Corruption at '" < path < "' : Normal Index\n";
					return Corrupt;
				}
				vout[i * vStride + 3 + j] = uniqueNormal[uniqueVertex[i * uvStride + 1] * 3 + j];
			}
			// UV
			for (int j = 0; j < cStride; j++) {
				if (uniqueVertex[i * uvStride + 2] * cStride + j > uColorS) {
					bug::out < bug::RED < "Corruption at '" < path < "' : Color Index\n";
					return Corrupt;
				}
				vout[i * vStride + 3 + 3 + j] = uColor[uniqueVertex[i * uvStride + 2] * cStride + j];
			}
			if (useWeight) {
				// Bone Index
				for (int j = 0; j < 3; j++) {
					if (uniqueVertex[i * uvStride + 3] * 3 + j > uWeightS) {
						bug::out < bug::RED < "Corruption at '" < path < "' : Bone Index\n";
						return Corrupt;
					}
					vout[i * vStride + 3 + cStride+3 + j] = uWeightI[uniqueVertex[i * uvStride + 3] * 3 + j];
				}
				// Weight
				for (int j = 0; j < 3; j++) {
					if (uniqueVertex[i * uvStride + 3] * 3 + j > uWeightS) {
						bug::out < bug::RED < "Corruption at '" < path < "' : Weight Index\n";
						return Corrupt;
					}
					vout[i * vStride + 3 + cStride+3+3 + j] = uWeightF[uniqueVertex[i * uvStride + 3] * 3 + j];
				}
			}

		}

		if (debug) {
			bug::outs < bug::LIME < "VertexBuffer " < (uniqueVertex.size() / uvStride) < "*" < (vStride) < " IndexBuffer " < (triC) < "*3" < bug::end;
		}
		/*
		for (int i = 0; i < (uniqueVertex.size() / uvStride) * vStride; i++) {
			std::cout << vout[i] << " ";
			if ((i + 1) / (vStride) == (float)(i + 1) / (vStride))
				std::cout << std::endl;
		}
		for (int i = 0; i < triC*3; i++) {
			std::cout << tout[i] << " ";
			if ((i + 1) / (3) == (float)(i + 1) / (3))
				std::cout << std::endl;
		}
		*/

		float vertices[] = {
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
		};
		unsigned int indices[36];
		for (int i = 0; i < 36;i++)
			indices[i] = i;

		if (da != nullptr) {
			if (!da->hasError) {
				da->material.normal_map2 = "brickwall_normal";
				if (type == 0) {
					da->shaderType = ShaderColor;
					da->material.diffuse_map1 = "blank";
				} else if (type == 1) {
					da->shaderType = ShaderColor;
					//da->material.diffuse_map1 = "brickwall";
					da->material.diffuse_map1 = texName;
				} else if (type == 2) {
					da->shaderType = ShaderColorBone;
				} else if (type == 3) {
					da->shaderType = ShaderUVBone;
				}
				/*
				da->container.Setup(false, vertices, 36, indices, 36);
				da->container.SetAttrib(0, 3, 8, 0);// Position
				da->container.SetAttrib(1, 3, 8, 3);// Normal
				da->container.SetAttrib(2, 2, 8, 6);// UV
				*/
				
				da->container.Setup(false, vout, (uniqueVertex.size() / uvStride) * vStride, tout, triC * 3);
				da->container.SetAttrib(0, 3, vStride, 0);// Position
				da->container.SetAttrib(1, 3, vStride, 3);// Normal
				da->container.SetAttrib(2, cStride, vStride, 3 + 3);// Color
				if (useWeight) {
					da->container.SetAttrib(3, 3, vStride, 3 + cStride + 3);// Bone Index
					da->container.SetAttrib(4, 3, vStride, 3 + cStride + 3 + 3);// Weight
				}
				
			}
		}

		// Cleanup
		delete uPos;
		delete uColor;
		delete uWeightI;
		delete uWeightF;
		delete tris;
		delete vout;
		delete tout;
		file.close();
		return Success;
	}
	int LoadAnim(AnimData* da, std::string path) {
		path = "assets/animations/" + path + ".anim";
		bool debug = 0;
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			file.close();
			bug::out < bug::RED < "Cannot find '" < path < "'\n";
			return NotFound;
		}
		ReadFromFile(&file, path);
		
		unsigned short start;
		if (Read<unsigned short>(&start)) return Corrupt;

		unsigned short end;
		if (Read<unsigned short>(&end)) return Corrupt;
		if (debug) bug::out < "Start-End: " < start <"-"<end< bug::end;

		float speed;
		if (Read<float>(&speed)) return Corrupt;
		if (debug) bug::out < "Speed: " < speed < bug::end;
		
		unsigned char objects;
		if (Read<unsigned char>(&objects)) return Corrupt;
		if (debug) bug::out < "Object: " < (int)objects < bug::end;

		if (da != nullptr) {
			da->frameStart = start;
			da->frameEnd = end;
			da->defaultSpeed = speed;
		}
		for (int i = 0; i < objects; i++) {
		
			int index;
			if (Read<int>(&index)) return Corrupt;

			unsigned short curves;
			if (Read<unsigned short>(&curves)) return Corrupt;

			if (debug)
				std::cout << "Object " << i << " " << curves << std::endl;

			bool curveB[10]{ 0,0,0,0,0,0,0,0,0,0 };
			for (int j = 9; j >= 0; j--) {
				if (0  <= curves - pow(2, j)) {
					curves -= pow(2, j);
					curveB[j] = 1;
				} else {
					curveB[j] = 0;
				}
			}

			da->objects[index]=(FCurves());
			FCurves* fCurves = &da->objects[index];

			for (int j = 0; j < 9; j++) {
				if (curveB[j]) {
					unsigned short keys;
					if (Read<unsigned short>(&keys)) return Corrupt;
					if (debug)
						std::cout << " Curve " << j << " " << (int)keys << std::endl;
					
					fCurves->fcurves[j] = FCurve();
					FCurve* fcurve = &fCurves->fcurves[j];

					for (int k = 0; k < keys; k++) {
						char polation;
						if (Read<char>(&polation)) return Corrupt;
						
						unsigned short frame;
						if (Read<unsigned short>(&frame)) return Corrupt;

						float value;
						if (Read<float>(&value)) return Corrupt;
						if (debug)
							bug::outs < "  Key" < polation < frame < value < bug::end;
						
						fcurve->frames.push_back(Keyframe(polation, frame, value));
					}
				}
			}
			//delete cname;
		}
		// Cleanup
		file.close();
		return Success;
	}
	int LoadColl(CollData* da, std::string path) { // TODO: More efficient by removing unneccecery for loops
		path = "assets/colliders/" + path + ".coll";
		bool debug = false;
		std::ifstream file(path, std::ios::binary);
		ReadFromFile(&file,path);
		
		std::uint16_t vC;
		if(Read<std::uint16_t>(&vC)) return Corrupt;
		if (debug) std::cout << "Points: " << vC << std::endl;

		std::uint16_t qC;
		if (Read<std::uint16_t>(&qC)) return Corrupt;
		if (debug) std::cout << "Quad: " << qC << std::endl;
		
		/* Not supported - REMEMBER TO DELETE WHEN TRIANGLES ARE SUPPORTED
		std::uint16_t tC;
		if (Read<std::uint16_t>(&vC)) return Corrupt;
		if (debug) std::cout << "Triangles: " << vC << std::endl;
		*/

		float furthest;
		if (Read<float>(&furthest)) return Corrupt;
		if (debug) std::cout << "Furthest: " << furthest << std::endl;

		da->furthestPoint = furthest;

		int vS = vC * 3;
		int qS = qC * 4;
		// int tS = tC * 3;

		float* uV = new float[vS];
		if (Read<float>(uV,vS)) return Corrupt;

		std::uint16_t* uQ = new std::uint16_t[qS];
		if (Read<std::uint16_t>(uQ, qS)) return Corrupt;
		/*
		std::uint16_t* uT = new std::uint16_t[tS];
		if (Read<std::uint16_t>(uT, tS)) return Corrupt;
		*/
		if (debug) std::cout << "Vectors" << std::endl;
		for (int i = 0; i < vC; i++) {
			da->points.push_back(glm::vec3(uV[i * 3], uV[i * 3 + 1], uV[i * 3 + 2]));
			if (debug) std::cout << uV[i * 3] << " " << uV[i * 3 + 1] << " " << uV[i * 3 + 2] << std::endl;
		}
		if (debug) std::cout << "Quads" << std::endl;;
		for (int i = 0; i < qC; i++) {
			for (int j = 0; j < 4; j++) {
				da->quad.push_back(uQ[i * 4 + j]);
				if (debug) std::cout << uQ[i * 4 + j] << " ";
			}
			if (debug) std::cout << std::endl;
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
		return Success;
	}
	int LoadBone(BoneData* da, std::string path) {
		path = "assets/bones/" + path + ".bone";
		bool debug = 0;
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			file.close();
			bug::out < bug::RED < "Cannot find '" < path < "'\n";
			return NotFound;
		}
		ReadFromFile(&file, path);

		unsigned char boneCount;
		if (Read<unsigned char>(&boneCount)) return Corrupt;
		if (debug) bug::out < "Bone Count: " < (int)boneCount < bug::end;

		// Acquire and Load Data
		for (int i = 0; i < boneCount; i++) {
			Bone b;
			if (Read<int>(&b.parent)) return Corrupt;
			if (debug) bug::out < " Parent: " < (int)b.parent < bug::end;
			/*
			glm::vec3 loc(0);
			glm::vec3 rot(0);
			glm::vec3 sca(0);
			if (Read<glm::vec3>(&loc)) return Corrupt;
			//if (Read<glm::vec3>(&rot)) return Corrupt;
			//if (Read<glm::vec3>(&sca)) return Corrupt;
			bug::out < loc < bug::end;
			b.localMat = glm::translate(loc);
				/*glm::rotate(rot.x,glm::vec3(1,0,0))
				*glm::rotate(rot.y,glm::vec3(0,1,0))
				*glm::rotate(rot.z,glm::vec3(0,0,1))
				*glm::scale(sca);/*
			if (Read<glm::vec3>(&loc)) return Corrupt;
			//if (Read<glm::vec3>(&rot)) return Corrupt;
			//if (Read<glm::vec3>(&sca)) return Corrupt;
			b.invModel = glm::translate(loc);
				/* glm::rotate(rot.x, glm::vec3(1, 0, 0))
				* glm::rotate(rot.y, glm::vec3(0, 1, 0))
				* glm::rotate(rot.z, glm::vec3(0, 0, 1))
				* glm::scale(sca);/*
			*/
			for (int x = 0; x < 4; x++){
				for (int y = 0; y < 4; y++) {
					if (Read<float>(&b.localMat[x][y])) return Corrupt;
				}
			}
			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < 4; y++) {
					if (Read<float>(&b.invModel[x][y])) return Corrupt;
				}
			}

			if (debug) bug::out < b.localMat < bug::end;
			if (debug) bug::out < b.invModel < bug::end;
			
			da->bones.push_back(b);
		}

		//bug::out < (glm::translate(glm::vec3(2, 3, 4))) < bug::end;
		//bug::out < (glm::rotate(0.5f,glm::vec3(1, 0, 0))) < bug::end;

		// Cleanup

		file.close();
		return Success;
	}
}