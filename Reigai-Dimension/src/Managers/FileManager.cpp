#include "FileManager.h"
#include <sys/stat.h>

namespace fManager {

	int FileExist(std::string path) {
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}
	/*
	.txt is added to path
	*/
	int WriteTextFile(std::string path, std::vector<std::string> text) {
		path += ".txt";
		std::ofstream file(path);
		std::vector<std::string> out;
		if (!file) {
			bug::out + bug::RED + "Cannot find file '" + path + "'\n";
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
		path += ".txt";
		std::ifstream file(path);

		std::vector<std::string> out;
		if (!file) {
			*err = NotFound;
			bug::out + bug::RED + "Cannot find file '" + path + "'\n";
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
	/*template<typename T,size_t N>
	void ReadBytes(std::ifstream *f, T (&d)[N]) {
		f->read(reinterpret_cast<char*>(&d[0]), sizeof(T)*N);
	}*/
	/*
	template<typename T, size_t N>
	void ReadBytes(std::ifstream* f, T* d) {
		f->read(reinterpret_cast<char*>(&d[0]), sizeof(T) * N);
	}
	/*
	Remember to delete heap allocation when done
	*/
	/*
	template<class T>
	T* ReadT(std::ifstream* f, int size) {
		T* buf = new T[size];
		f->read(reinterpret_cast<char*>(&buf[0]), sizeof(T)*size);
		return buf;
	}
	glm::mat4 ReadMat4(std::ifstream* f) {
		glm::mat4 buf(1);
		//f->read(reinterpret_cast<char*>(&buf[0]), sizeof(glm::mat4));
		return buf;
	}*/

	std::string ReadFile(std::string path, int* err) {
		*err = Success;
		path += ".txt";
		std::ifstream file(path);
		if (!file) {
			*err = NotFound;
			bug::out + bug::RED + "Cannot find file '" + path + "'\n";
			return "";
		}
		std::string text;
		std::string line;
		while (getline(file, line))
		{
			text += line;
		}
		file.close();
		return text;
	}
	int LoadMesh(MeshData* da, std::string path) {
		path += ".mesh";
		bool debug = false;
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			file.close();
			bug::out + bug::RED + "Cannot find file '" + path + "'\n";
			return NotFound;
		}
		int atByte = 0;
		file.seekg(0, file.end);
		int fileSize = file.tellg();
		file.seekg(0, file.beg);
		
		atByte += 7;// 4 * 6;
		if (atByte > fileSize) {
			bug::out + bug::RED + "Corruption at '" +path+"' : missing "+ (atByte-fileSize) + " bytes\n";
			return Corrupt;
		}

		char buf[2];
		unsigned short valu[3];
		//ReadBytes<unsigned short,3>(&file,valu);
		file.read(reinterpret_cast<char*>(&valu[0]), 2 * 3);// Vertex Count
		unsigned short posC = valu[0];
		unsigned short colorC = valu[1];
		unsigned short triC = valu[2];
		if (debug) {
			bug::out + "Points: " + posC + bug::end;
			bug::out + "Colors: " + colorC + bug::end;
			bug::out + "Triangles: " + triC + bug::end;
		}
		file.read(buf, 1);// Texture Name Length
		int texLen = (unsigned char)buf[0];
		if (debug)
			bug::out + "TextureLen: " + texLen + bug::end;

		int cStride = 4;
		std::string texName;
		if (texLen > 0) {
			cStride = 2;
			char* name = new char[texLen];
			atByte += texLen;
			if (atByte > fileSize) {
				bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
				return Corrupt;
			}
			file.read(name, texLen);// Texture Name
			for (int i = 0; i < texLen; i++) {
				texName += name[i];
			}
			if (debug)
				bug::out + "Texture: " + texName + bug::end;
			delete name;
		} else {
			if (debug)
				bug::out + "No Texture" + bug::end;
		}

		float* place = new float[6];
		file.read(reinterpret_cast<char*>(&place[0]), 4 * 6);
		if (debug) {
			bug::out + "Position " + place[0] + " " + place[1] + " " + place[2] + bug::end;
			bug::out + "Rotation " + place[3] + " " + place[4] + " " + place[5] + bug::end;
		}
		int uPosS = 3 * posC;
		int uColorS = colorC * cStride;
		int trisS = triC * 6;

		atByte += 4 * uPosS + 4 * uColorS + 2 * trisS;
		if (atByte > fileSize) {
			bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
			return Corrupt;
		}

		float* uPos = new float[uPosS];
		file.read(reinterpret_cast<char*>(&uPos[0]), 4 * uPosS); // Positions
		//std::cout << "Unique Pos" << std::endl;

		float* uColor = new float[uColorS];
		file.read(reinterpret_cast<char*>(&uColor[0]), 4 * uColorS);// Colors

		//std::cout << "Unique Color" << std::endl;

		unsigned short* tris = new unsigned short[trisS];
		file.read(reinterpret_cast<char*>(&tris[0]), 2 * trisS);// Triangles
		//std::cout << "Triangles" << std::endl;
		if (debug) {
			bug::out +bug::LIME+ "  Vectors\n";
			for (int i = 0; i < 3 * posC; i++) {
				bug::out + uPos[i] +" ";
				if ((i + 1) / (3) == (float)(i + 1) / (3))
					bug::out  +bug::end;
			}
			bug::out +bug::LIME+ "  Colors\n";
			for (int i = 0; i < colorC * cStride; i++) {
				bug::out + uColor[i] + " ";
				if ((i + 1) / (cStride) == (float)(i + 1) / (cStride))
					bug::out + bug::end;
			}
			bug::out +bug::LIME+ "  Triangles\n";
			for (int i = 0; i < 6 * triC; i++) {
				bug::out + tris[i] + " ";
				if ((i + 1) / (6) == (float)(i + 1) / (6))
					bug::out + bug::end;
			}
		}

		std::vector<unsigned short> triN;
		std::vector<float> uNormal;

		for (int i = 0; i < triC; i++) {
			for (int j = 0; j < 3 * 3; j++) {
				if (tris[i * 6 + j / 3 * 2] * 3 + j - j / 3 * 3 > uPosS) {
					bug::out +bug::RED+"Corruption at '"+path+"' : Triangle Index\n";
					return Corrupt;
				}
			}
			glm::vec3 p0(uPos[tris[i * 6 + 0] * 3 + 0], uPos[tris[i * 6 + 0] * 3 + 1], uPos[tris[i * 6 + 0] * 3 + 2]);
			glm::vec3 p1(uPos[tris[i * 6 + 2] * 3 + 0], uPos[tris[i * 6 + 2] * 3 + 1], uPos[tris[i * 6 + 2] * 3 + 2]);
			glm::vec3 p2(uPos[tris[i * 6 + 4] * 3 + 0], uPos[tris[i * 6 + 4] * 3 + 1], uPos[tris[i * 6 + 4] * 3 + 2]);
			//std::cout << p0.x << " " << p0.y << " " << p0.z << std::endl;
			//std::cout << p1.x << " " << p1.y << " " << p1.z << std::endl;
			//std::cout << p2.x << " " << p2.y << " " << p2.z << std::endl;
			glm::vec3 cro = glm::cross(p1 - p0, p2 - p0);
			//std::cout << cro.x << " " << cro.y << " " << cro.z << std::endl;
			glm::vec3 norm = glm::normalize(cro);
			//std::cout << norm.x << " " << norm.y << " " << norm.z << std::endl;

			int exist = -1;
			for (int j = 0; j < uNormal.size() / 3; j++) {
				if (uNormal[j * 3 + 0] == norm.x && uNormal[j * 3 + 1] == norm.y && uNormal[j * 3 + 2] == norm.z) {
					exist = j;
					break;
				}
			}
			if (exist == -1) {
				uNormal.push_back(norm.x);
				uNormal.push_back(norm.y);
				uNormal.push_back(norm.z);
				triN.push_back(uNormal.size() / 3 - 1);
			} else {
				triN.push_back(exist);
			}
		}
		//std::cout << "Calculated Normals " << std::endl;

		std::vector<unsigned short> indexing;
		unsigned int* tout = new unsigned int[triC * 3];
		for (int i = 0; i < triC; i++) {
			unsigned short index[3]{ -1,-1,-1 };
			for (int j = 0; j < 3; j++) {
				for (int s = 0; s < indexing.size() / 3; s++) {
					if (tris[i * 6 + j * 2] == indexing[s * 3] && tris[i * 6 + j * 2 + 1] == indexing[s * 3 + 1] && triN[i] == indexing[s * 3 + 2]) {
						index[j] = s;
						break;
					}
				}
				if (index[j] == (unsigned short)-1) {
					indexing.push_back(tris[i * 6 + j * 2]);
					indexing.push_back(tris[i * 6 + j * 2 + 1]);
					indexing.push_back(triN[i]);
					index[j] = indexing.size() / 3 - 1;
				}
			}
			tout[i * 3] = index[0];
			tout[i * 3 + 1] = index[1];
			tout[i * 3 + 2] = index[2];
		}

		float* vout = new float[(indexing.size() / 3) * (3 + cStride + 3)];
		for (int i = 0; i < indexing.size() / 3; i++) {
			// Position
			for (int j = 0; j < 3; j++) {
				if (indexing[i * 3] * 3 + j > uPosS) {
					bug::out + bug::RED + "Corruption at '" + path + "' : Position Index\n";
					return Corrupt;
				}
				vout[i * (3 + cStride + 3) + j] = uPos[indexing[i * 3] * 3 + j];
			}
			// Color
			for (int j = 0; j < cStride; j++) {
				if (indexing[i * 3 + 1] * cStride + j > uColorS) {
					bug::out + bug::RED + "Corruption at '" + path + "' : Color Index\n";
					return Corrupt;
				}
				vout[i * (3 + cStride + 3) + 3 + j] = uColor[indexing[i * 3 + 1] * cStride + j];
			}

			// Normal
			for (int j = 0; j < 3; j++) {
				if (indexing[i * 3 + 2] * 3 + j > uNormal.size()) {
					bug::out + bug::RED + "Corruption at '" + path + "' : Normal Index\n";
					return Corrupt;
				}
				vout[i * (3 + cStride + 3) + 3 + cStride + j] = uNormal[indexing[i * 3 + 2] * 3 + j];
			}
		}

		/*
		for (int i = 0; i < indexing.size() / 3 * (3 + cStride+3); i++) {
			std::cout << vout[i] << " ";
			if ((i + 1) / (3 + cStride+3) == (float)(i + 1) / (3 + cStride+3))
				std::cout << std::endl;
		}
		for (int i = 0; i < triC * 3; i++) {
			std::cout << tout[i] << " ";
			if ((i + 1) / 3 == (i + 1) / (float)3)
				std::cout << std::endl;
		}
		*/

		//std::cout << "VertexBuffer " << (indexing.size() / 3) << "*" << (3 + cStride + 3) << " IndexBuffer " << (triC) << "*3" << std::endl;

		// Used in actuall game

		if (da != nullptr) {
			da->texture = texName;
			if (texLen > 0) {
				da->material = MaterialType::TextureMat;
			} else {
				da->material = MaterialType::ColorMat;
			}
			da->position = glm::vec3(place[0], place[1], place[2]);
			da->rotation = glm::vec3(place[3], place[4], place[5]);
			da->container.Setup(false, vout, (indexing.size() / 3) * (3 + cStride + 3), tout, triC * 3);
			da->container.SetAttrib(0, 3, 3 + cStride + 3, 0);// Position
			da->container.SetAttrib(1, cStride, 3 + cStride + 3, 3);// Color
			da->container.SetAttrib(2, 3, 3 + cStride + 3, 3 + cStride);// Normal
		}

		// Cleanup
		delete place;
		delete uPos;
		delete uColor;
		delete tris;
		delete vout;
		delete tout;
		file.close();
		return Success;
	}
	int LoadAnim(AnimData* da, std::string path) {
		path += ".anim";
		bool debug = false;
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			file.close();
			bug::out + bug::RED + "Cannot find file '" + path + "'\n";
			return NotFound;
		}

		int atByte = 0;
		file.seekg(0, file.end);
		int fileSize = file.tellg();
		file.seekg(0, file.beg);

		atByte += 10;
		if (atByte > fileSize) {
			bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
			return Corrupt;
		}

		char buf[2];
		file.read(buf, 2);
		unsigned short start = 0x0000 | buf[1] << 8 | buf[0];

		file.read(buf, 2);
		unsigned short end = 0x0000 | buf[1] << 8 | buf[0];

		float speed;
		file.read(reinterpret_cast<char*>(&speed), 4);

		file.read(buf, 1);
		unsigned char loop = buf[0];

		file.read(buf, 1);
		unsigned char objects = buf[0];

		if (debug) {
			bug::out + "Start: " + start + bug::end;
			bug::out + "End: " + end + bug::end;
			bug::out + "Speed: " + speed + bug::end;
			bug::out + "Loop: " + (int)loop + bug::end;
			bug::out + "Objects: " + (int)objects + bug::end;
		}

		// Used in actual Game
		da->frameStart = start;
		da->frameEnd = end;
		da->defaultSpeed = speed;
		da->loop = loop;


		for (int i = 0; i < objects; i++) {
			atByte += 1;
			if (atByte > fileSize) {
				bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
				return Corrupt;
			}
			file.read(buf, 1);// Object Name Length
			unsigned char namelen = buf[0];
			char* cname = new char[namelen];
			atByte += namelen;
			if (atByte > fileSize) {
				bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
				return Corrupt;
			}
			file.read(cname, namelen);
			std::string name;
			for (int j = 0; j < namelen; j++) {
				name += cname[j];
			}
			atByte += 2;
			if (atByte > fileSize) {
				bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
				return Corrupt;
			}
			file.read(buf, 2);// Curves
			unsigned short curves = 0x0000 | buf[1] << 8 | buf[0];
			if (debug)
				std::cout << "Object " << name << " " << curves << std::endl;
			bool curveB[9]{ 0,0,0,0,0,0,0,0,0 };
			for (int j = 8; j >= 0; j--) {
				if (0 <= curves - pow(2, j)) {
					curves -= pow(2, j);
					curveB[j] = 1;
				} else {
					curveB[j] = 0;
				}
			}

			//da->curves[name] = FCurves();
			//FCurves* fCurves = &da->curves[name];

			for (int j = 0; j < 9; j++) {
				if (curveB[j]) {
					atByte += 2;
					if (atByte > fileSize) {
						bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
						return Corrupt;
					}
					file.read(buf, 2);// Keys
					unsigned short keys = 0x0000 | buf[1] << 8 | buf[0];
					if (debug)
						std::cout << " Curve " << j << " " << (int)keys << std::endl;

					for (int k = 0; k < keys; k++) {
						atByte += 1 + 2 + 4;
						if (atByte > fileSize) {
							bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
							return Corrupt;
						}
						file.read(buf, 1);// Polation
						char polation = buf[0];
						file.read(buf, 2);// Frame
						unsigned short frame = 0x0000 | buf[1] << 8 | buf[0];
						float v = 0;
						file.read(reinterpret_cast<char*>(&v), 4);// Value
						if (debug)
							std::cout << "  Key " << polation << " " << frame << " " << v << std::endl;

						//fCurves->keyframes[j].keyframes.push_back(Keyframe(polation, frame, v));
					}
				}
			}
			delete cname;
		}

		// Cleanup
		file.close();
		return Success;
	}
	int LoadColl(CollData* da, std::string path) { // TODO: More efficient by removing unneccecery for loops
		path += ".coll";
		bool debug = false;
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			file.close();
			bug::out + bug::RED + "Cannot find file '" + path + "'\n";
			return NotFound;
		}
		file.seekg(0, file.end);
		int fileSize = file.tellg();
		file.seekg(0, file.beg);

		int atByte = 2 * 3 + 4;
		if (atByte > fileSize) {
			bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
			return Corrupt;
		}

		char buf[2];
		std::uint16_t valu[3];
		file.read(reinterpret_cast<char*>(&valu[0]), 2 * 3);// Vertex Count
		std::uint16_t vC = valu[0];
		std::uint16_t qC = valu[1];
		std::uint16_t tC = valu[2];
		float furthest;
		file.read(reinterpret_cast<char*>(&furthest), 4);// Vertex Count
		da->furthestPoint = furthest;
		if (debug) {
			std::cout << "Points: " << vC << std::endl;
			std::cout << "Quads: " << qC << std::endl;
			std::cout << "Triangles: " << tC << std::endl;
			std::cout << "Furthest: " << furthest << std::endl;
		}

		int vS = vC * 3;
		int qS = qC * 4;
		int tS = tC * 3;

		atByte += 4 * vS + 2 * qS + 2 * tS;
		if (atByte > fileSize) {
			bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
			return Corrupt;
		}

		float* uV = new float[vS];
		file.read(reinterpret_cast<char*>(&uV[0]), 4 * vS); // Positions

		std::uint16_t* uQ = new std::uint16_t[qS];
		file.read(reinterpret_cast<char*>(&uQ[0]), 2 * qS);// Colors

		std::uint16_t* uT = new std::uint16_t[tS];
		file.read(reinterpret_cast<char*>(&uT[0]), 2 * tS);// Triangles

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
		if (debug) std::cout << "Triangles" << std::endl;;
		for (int i = 0; i < tC; i++) {
			for (int j = 0; j < 3; j++) {
				da->quad.push_back(uT[i]);
				if (debug) std::cout << uQ[i * 3 + j] << " ";
			}
			if (debug) std::cout << std::endl;
		}

		// Cleanup
		delete uV;
		delete uQ;
		delete uT;
		file.close();
		return Success;
	}
	int LoadBone(BoneData* da, std::string path) {
		path += ".bone";
		bool debug = false;
		std::ifstream file(path, std::ios::binary);
		if (!file) {
			file.close();
			bug::out + bug::RED + "Cannot find file '" + path + "'\n";
			return NotFound;
		}
		file.seekg(0, file.end);
		int fileSize = file.tellg();
		file.seekg(0, file.beg);

		int atByte = 2 * 3 + 4;
		if (atByte > fileSize) {
			bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
			return Corrupt;
		}

		char buf[2];
		std::uint16_t valu[3];
		file.read(reinterpret_cast<char*>(&valu[0]), 2 * 3);// Vertex Count
		std::uint16_t vC = valu[0];
		std::uint16_t qC = valu[1];
		std::uint16_t tC = valu[2];
		float furthest;
		file.read(reinterpret_cast<char*>(&furthest), 4);// Vertex Count
		//da->furthestPoint = furthest;
		if (debug) {
			std::cout << "Points: " << vC << std::endl;
			std::cout << "Quads: " << qC << std::endl;
			std::cout << "Triangles: " << tC << std::endl;
			std::cout << "Furthest: " << furthest << std::endl;
		}

		int vS = vC * 3;
		int qS = qC * 4;
		int tS = tC * 3;

		atByte += 4 * vS + 2 * qS + 2 * tS;
		if (atByte > fileSize) {
			bug::out + bug::RED + "Corruption at '" + path + "' : missing " + (atByte - fileSize) + " bytes\n";
			return Corrupt;
		}

		float* uV = new float[vS];
		file.read(reinterpret_cast<char*>(&uV[0]), 4 * vS); // Positions

		std::uint16_t* uQ = new std::uint16_t[qS];
		file.read(reinterpret_cast<char*>(&uQ[0]), 2 * qS);// Colors

		std::uint16_t* uT = new std::uint16_t[tS];
		file.read(reinterpret_cast<char*>(&uT[0]), 2 * tS);// Triangles

		if (debug) std::cout << "Vectors" << std::endl;
		for (int i = 0; i < vC; i++) {
			//da->points.push_back(glm::vec3(uV[i * 3], uV[i * 3 + 1], uV[i * 3 + 2]));
			if (debug) std::cout << uV[i * 3] << " " << uV[i * 3 + 1] << " " << uV[i * 3 + 2] << std::endl;
		}
		if (debug) std::cout << "Quads" << std::endl;;
		for (int i = 0; i < qC; i++) {
			for (int j = 0; j < 4; j++) {
				//da->quad.push_back(uQ[i * 4 + j]);
				if (debug) std::cout << uQ[i * 4 + j] << " ";
			}
			if (debug) std::cout << std::endl;
		}
		if (debug) std::cout << "Triangles" << std::endl;;
		for (int i = 0; i < tC; i++) {
			for (int j = 0; j < 3; j++) {
				//da->quad.push_back(uT[i]);
				if (debug) std::cout << uQ[i * 3 + j] << " ";
			}
			if (debug) std::cout << std::endl;
		}

		// Cleanup
		delete uV;
		delete uQ;
		delete uT;
		file.close();
		return Success;
	}
}