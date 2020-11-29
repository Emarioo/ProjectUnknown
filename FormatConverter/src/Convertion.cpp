#include "Convertion.h"

#include <fstream>
#include <vector>
#include <sstream>
#include "glm/glm.hpp"
std::vector<std::string> SplitString(std::string text, std::string delim) {
	std::vector<std::string> out;
	unsigned int at = 0;
	while ((at = text.find(delim)) != std::string::npos) {
		out.push_back(text.substr(0, at));
		text.erase(0, at + delim.length());
	}
	out.push_back(text);
	return out;
}
int ConvertCollider(std::string ipath) {// Convert blender py rawcol to .col. Parameters without formats
	std::string opath = ipath.substr(0, ipath.length() - 7) + "coll";
	int err;
	std::ifstream ifile(ipath);
	if (!ifile) {
		std::cout << "Could not find " << ipath << std::endl;
		ifile.close();
		return NotFound;
	}
	std::ofstream file(opath, std::ios::binary);
	if (!file) {
		std::cout << "Could not create/find " << opath << std::endl;
		file.close();
		return NotFound;
	}

	std::string line;
	int setup = 0;
	int ind = 0;
	while (getline(ifile, line)) {
		ind++;
		if (line[0] == '#' || line == "")
			continue;
		std::vector<std::string> set = SplitString(line, " ");
		if (set[0] == "coll") {
			if (set.size() != 5) {
				if (set.size() < 5)
					std::cout << "Syntax Error  at [" << ind << "] : Col to few parameters" << std::endl;
				else if (set.size() > 5)
					std::cout << "Syntax Error at [" << ind << "] : Col to many parameters" << std::endl;
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			char buf[2];
			unsigned short vc = std::stoi(set[1]);
			buf[0] = (char)(vc);
			buf[1] = (char)(vc >> 8);
			file.write(buf, 2);// Vertex Count

			unsigned short qc = std::stoi(set[2]);
			buf[0] = (char)(qc);
			buf[1] = (char)(qc >> 8);
			file.write(buf, 2);// Quad Count

			unsigned short tc = std::stoi(set[3]);
			buf[0] = (char)(tc);
			buf[1] = (char)(tc >> 8);
			file.write(buf, 2);// Triangle Count

			float furthest = std::stoi(set[4]);
			//char* buf = reinterpret_cast<char*>(&furthest);
			file.write(reinterpret_cast<char*>(&furthest), 4); // Furthest point

			setup++;
		} else if (setup != 1) {
			std::cout << "Syntax Error [" << ind << "] : Missing col Info" << std::endl;
			std::cout << " " << line << std::endl;
			file.close();
			return Syntax;
		} else if (set[0] == "v") {
			if (set.size() != 4) {
				if (set.size() > 4) {
					std::cout << "Syntax Error [" << ind << "] : Has << " << (set.size() - 4) << " additional parameter(s)" << std::endl;
				} else {
					std::cout << "Syntax Error [" << ind << "] : Missing " << (4 - set.size()) << " parameter(s)" << std::endl;
				}
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			float v = 0;
			for (int i = 0; i < 3; i++) {
				v = std::stof(set[1 + i]);
				char* buf = reinterpret_cast<char*>(&v);
				file.write(buf, sizeof(buf)); // Position
			}
		} else if (set[0] == "q") {
			if (set.size() != 5) {
				if (set.size() > 5) {
					std::cout << "Syntax Error [" << ind << "] : Has << " << (set.size() - 5) << " additional parameter(s)" << std::endl;
				} else {
					std::cout << "Syntax Error [" << ind << "] : Missing " << (5 - set.size()) << " parameter(s)" << std::endl;
				}
			}
			char buf[2];
			unsigned short v = 0;
			for (int i = 0; i < 4; i++) {
				v = std::stoi(set[1 + i]);
				buf[0] = (char)(v);
				buf[1] = (char)(v >> 8);
				file.write(buf, sizeof(buf));// Index
			}
		} else if (set[0] == "t") {
			if (set.size() != 4) {
				if (set.size() > 4) {
					std::cout << "Syntax Error [" << ind << "] : Has << " << (set.size() - 4) << " additional parameter(s)" << std::endl;
				} else {
					std::cout << "Syntax Error [" << ind << "] : Missing " << (4 - set.size()) << " parameter(s)" << std::endl;
				}
			}
			char buf[2];
			unsigned short v = 0;
			for (int i = 0; i < 3; i++) {
				v = std::stoi(set[1 + i]);
				buf[0] = (char)(v);
				buf[1] = (char)(v >> 8);
				file.write(buf, sizeof(buf));// Index
			}
		}
	}
	std::cout << "Successfully converted " << ipath << " to " << opath << std::endl;
	file.close();
	return Success;
}
int ReadCollider(std::string path) {
	CollData* da = new CollData();
	bool debug = true;
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		std::cout << "Could not find " << path << std::endl;
		file.close();
		return 0;
	}
	file.seekg(0, file.end);
	int fileSize = file.tellg();
	file.seekg(0, file.beg);

	int atByte = 2 * 3 + 4;
	if (atByte > fileSize) {
		std::cout << "Corrupt Error: At byte " << atByte << std::endl;
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
		std::cout << "Corrupt Error: At byte " << atByte << std::endl;
		return Corrupt;
	}

	float* uV = new float[vS];
	file.read(reinterpret_cast<char*>(&uV[0]), 4 * vS); // Positions
	
	std::uint16_t* uQ = new std::uint16_t[qS];
	file.read(reinterpret_cast<char*>(&uQ[0]), 2 * qS);// Colors

	std::uint16_t* uT = new std::uint16_t[tS];
	file.read(reinterpret_cast<char*>(&uT[0]), 2 * tS);// Triangles
	
	for (int i = 0; i < vC;i++) {
		da->points.push_back(glm::vec3(uV[i*3], uV[i * 3+1], uV[i * 3+2]));
		if (debug) std::cout << "V " << uV[i * 3] << " " << uV[i * 3 + 1] << " " << uV[i * 3 + 2] << std::endl;
	}
	for (int i = 0; i < qC; i++) {
		if(debug) std::cout << "Q";
		for (int j = 0; j < 4;j++) {
			da->quad.push_back(uQ[i*4+j]);
			if(debug) std::cout << " " << uQ[i * 4 + j];
		}
		if(debug) std::cout << std::endl;
	}
	for (int i = 0; i < tC; i++) {
		if (debug) std::cout << "T";
		for (int j = 0; j < 3; j++) {
			da->quad.push_back(uT[i]);
			if (debug) std::cout << " " << uQ[i * 3 + j];
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
int ConvertMesh(std::string ipath) {// Convert blender py rawmesh to .mesh. Parameters without formats
	std::string opath = ipath.substr(0, ipath.length() - 7) + "mesh";
	int err;
	std::ifstream ifile(ipath);
	if (!ifile) {
		std::cout << "Could not find " << ipath << std::endl;
		ifile.close();
		return NotFound;
	}
	std::ofstream file(opath, std::ios::binary);
	if (!file) {
		std::cout << "Could not create/find " << opath << std::endl;
		file.close();
		return NotFound;
	}

	int setup = 0;
	bool tex = false;
	std::string line;
	int ind = 0;
	while(getline(ifile,line)){
		ind++;
		if (line[0] == '#'||line=="")
			continue;
		std::vector<std::string> set = SplitString(line, " ");
		if (set[0] == "mesh") {
			if (set.size() != 4 && set.size() != 5) {
				if(set.size()<4)
					std::cout << "Syntax Error  at ["<<ind<<"] : Mesh to few parameters" << std::endl;
				else if(set.size()>5)
					std::cout << "Syntax Error at [" << ind << "] : Mesh to many parameters" << std::endl;
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			char buf[2];
			unsigned short vc = std::stoi(set[1]);
			buf[0] = (char)(vc);
			buf[1] = (char)(vc >> 8);
			file.write(buf, 2);// Vertex Count

			unsigned short cc = std::stoi(set[2]);
			buf[0] = (char)(cc);
			buf[1] = (char)(cc >> 8);
			std::cout << "Buf "<<(int)buf[0]<<" "<<(int)buf[1] << std::endl;
			unsigned short colorC = buf[1] << 8 | buf[0];
			std::cout << "Yeet " <<colorC << std::endl;
			file.write(buf, 2);// Color Count

			unsigned short tc = std::stoi(set[3]);
			buf[0] = (char)(tc);
			buf[1] = (char)(tc >> 8);
			file.write(buf, 2);// Triangle Count
			if (set.size() == 5) {
				tex = true;
				buf[0] = (unsigned char)set[4].length();
				file.write(buf, 1);// Texture Name Length
				file.write(set[4].c_str(), set[4].length());// Texture Name
			} else {
				tex = false;
				buf[0] = 0;
				file.write(buf, 1);// Texture Length = 0
			}
			setup++;
		}else if (set[0] == "place") {
			if (set.size() != 7) {
				if (set.size() < 7)
					std::cout << "Syntax Error  at [" << ind << "] : Mesh to few parameters" << std::endl;
				else if (set.size() > 7)
					std::cout << "Syntax Error at [" << ind << "] : Mesh to many parameters" << std::endl;
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			float v = 0;
			for (int i = 0; i < 6; i++) {
				v = std::stof(set[1 + i]);
				char* buf = reinterpret_cast<char*>(&v);
				file.write(buf, sizeof(buf)); // Position and Rotation
			}
			setup++;
		} else if (setup!=2) {
			std::cout << "Syntax Error [" << ind << "] : Missing " << (setup - 2) << " Info(s) Before" << std::endl;
			std::cout << " " << line << std::endl;
			file.close();
			return Syntax;
		} else if (set[0] == "v") {
			if (set.size()!=4) {
				if (set.size() > 4) {
					std::cout << "Syntax Error [" << ind << "] : Has << "<< (set.size()-4) <<" additional parameter(s)" << std::endl;
				} else {
					std::cout << "Syntax Error [" << ind << "] : Missing " << (4 - set.size()) << " parameter(s)" << std::endl;
				}
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			float v = 0;
			for (int i = 0; i < 3; i++) {
				v = std::stof(set[1 + i]);
				char* buf = reinterpret_cast<char*>(&v);
				file.write(buf, sizeof(buf)); // Position
			}
		} else if (set[0] == "c") {
			if (tex) {
				if (set.size() != 3) {
					if (set.size() > 3) {
						std::cout << "Syntax Error [" << ind << "] : Has << " << (set.size() - 3) << " additional parameter(s)" << std::endl;
					} else {
						std::cout << "Syntax Error [" << ind << "] : Missing " << (3 - set.size()) << " parameter(s)" << std::endl;
					}
					std::cout << " " << line << std::endl;
					file.close();
					return Syntax;
				}
				float v = 0;
				for (int i = 0; i < 2; i++) {
					v = std::stof(set[1 + i]);
					char* buf = reinterpret_cast<char*>(&v);
					file.write(buf, sizeof(buf));// UV
				}
			} else {
				if (set.size() != 5) {
					if (set.size() > 5) {
						std::cout << "Syntax Error [" << ind << "] : Has << " << (set.size() - 5) << " additional parameter(s)" << std::endl;
					} else {
						std::cout << "Syntax Error [" << ind << "] : Missing " << (5 - set.size()) << " parameter(s)" << std::endl;
					}
					std::cout << " " << line << std::endl;
					file.close();
					return Syntax;
				}
				float v = 0;
				for (int i = 0; i < 4; i++) {
					v = std::stof(set[1 + i]);
					char* buf = reinterpret_cast<char*>(&v);
					file.write(buf, sizeof(buf));// Color
				}
			}
		} else if (set[0] == "t") {
			if (set.size() != 7) {
				if (set.size() > 7) {
					std::cout << "Syntax Error [" << ind << "] : Has << " << (set.size() - 7) << " additional parameter(s)" << std::endl;
				} else {
					std::cout << "Syntax Error [" << ind << "] : Missing " << (7 - set.size()) << " parameter(s)" << std::endl;
				}
			}
			char buf[2];
			unsigned short v = 0;
			for (int i = 0; i < 6; i++) {
				v = std::stoi(set[1 + i]);
				buf[0] = (char)(v);
				buf[1] = (char)(v >> 8);
				file.write(buf, sizeof(buf));// Index
			}
		}
	}
	std::cout << "Successfully converted " << ipath << " to " << opath << std::endl;
	file.close();
	return Success;
}
int ReadMesh(std::string path) {
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		std::cout << "Could not find " << path << std::endl;
		file.close();
		return 0;
	}
	int atByte = 0;
	file.seekg(0, file.end);
	int fileSize = file.tellg();
	file.seekg(0, file.beg);

	atByte += 7+4*6;
	if (atByte > fileSize) {
		std::cout << "Corrupt Error: At byte " << atByte << std::endl;
		return Corrupt;
	}

	char buf[2];
	unsigned short valu[3];
	file.read(reinterpret_cast<char*>(&valu[0]), 2*3);// Vertex Count
	unsigned short posC = valu[0];
	unsigned short colorC = valu[1];
	unsigned short triC = valu[2];
	std::cout << "Points: " << posC << std::endl;
	std::cout << "Colors: " << colorC << std::endl;
	std::cout << "Triangles: " << triC << std::endl;
	
	file.read(buf, 1);// Texture Name Length
	int texLen = (unsigned char)buf[0];

	int cStride = 4;
	std::string texName;
	if (texLen > 0) {
		cStride = 2;
		char* name = new char[texLen];
		atByte += texLen;
		if (atByte > fileSize) {
			std::cout << "Corrupt Error: At byte " << atByte << std::endl;
			return Corrupt;
		}
		file.read(name, texLen);// Texture Name
		for (int i = 0; i < texLen; i++) {
			texName += name[i];
		}
		std::cout << "Texture: " << texName << std::endl;
		delete name;
	} else {
		std::cout << "No Texture" << std::endl;
	}

	float* place = new float[6];
	file.read(reinterpret_cast<char*>(&place[0]),4*6);

	std::cout << "Position " << place[0] << " " << place[1] << " " << place[2] << std::endl;
	std::cout << "Rotation " << place[3] << " " << place[4] << " " << place[5] << std::endl;

	int uPosS = 3 * posC;
	int uColorS = colorC * cStride;
	int trisS = triC * 6;

	atByte += 4 * uPosS+4*uColorS+2*trisS;
	if (atByte > fileSize) {
		std::cout << "Corrupt Error: At byte " << atByte << std::endl;
		return Corrupt;
	}

	float* uPos = new float[uPosS];
	file.read(reinterpret_cast<char*>(&uPos[0]), 4 * uPosS); // Positions
	std::cout << "Unique Pos" << std::endl;

	float* uColor = new float[uColorS];
	file.read(reinterpret_cast<char*>(&uColor[0]), 4 * uColorS);// Colors

	std::cout << "Unique Color" << std::endl;

	unsigned short* tris = new unsigned short[trisS];
	file.read(reinterpret_cast<char*>(&tris[0]), 2 * trisS);// Triangles
	std::cout << "Triangles" << std::endl;

	/*for (int i = 0; i < 3*posC; i++) {
		std::cout << uPos[i] << " ";
		if ((i + 1) / (3) == (float)(i + 1) / (3))
			std::cout << std::endl;
	}*/
	/*for (int i = 0; i < colorC*cStride; i++) {
		std::cout << uColor[i] << " ";
		if ((i + 1) / (cStride) == (float)(i + 1) / (cStride))
			std::cout << std::endl;
	}*/
	/*for (int i = 0; i < 6 * triC; i++) {
		std::cout << tris[i] << " ";
		if ((i + 1) / (6) == (float)(i + 1) / (6))
			std::cout << std::endl;
	}*/
	
	std::vector<unsigned short> triN;
	std::vector<float> uNormal;
	
	for (int i = 0; i < triC; i++) {
		for (int j = 0; j < 3*3;j++) {
			if (tris[i * 6 + j/3*2] * 3+j-j/3*3 > uPosS) {
				std::cout << "Syntax Error: Triangle Index" << std::endl;
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
			if (uNormal[j * 3 + 0] == norm.x&&uNormal[j * 3 + 1] == norm.y&&uNormal[j * 3 + 2] == norm.z) {
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
	std::cout << "Calculated Normals " << std::endl;
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

	float* vout = new float[(indexing.size() / 3)*(3 + cStride + 3)];
	for (int i = 0; i < indexing.size() / 3; i++) {
		// Position
		for (int j = 0; j < 3; j++) {
			if (indexing[i * 3] * 3 + j > uPosS) {
				std::cout << "Corrupt Error: Position Index" << std::endl;
				return Corrupt;
			}
			vout[i*(3 + cStride + 3) + j] = uPos[indexing[i * 3] * 3 + j];
		}
		// Color
		for (int j = 0; j < cStride; j++) {
			if (indexing[i * 3 + 1] * cStride + j > uColorS) {
				std::cout << "Corrupt Error: Color Index" << std::endl;
				return Corrupt;
			}
			vout[i*(3 + cStride + 3) + 3 + j] = uColor[indexing[i * 3 + 1] * cStride + j];
		}
		
		// Normal
		for (int j = 0; j < 3; j++) {
			if (indexing[i * 3 + 2] * 3 + j > uNormal.size()) {
				std::cout << "Corrupt Error: Normal Index" << std::endl;
				return Corrupt;
			}
			vout[i*(3 + cStride + 3) + 3 + cStride + j] = uNormal[indexing[i * 3 + 2] * 3 + j];
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

	std::cout << "VertexBuffer " << (indexing.size() / 3) << "*" << (3 + cStride+3) << " IndexBuffer " << (triC) << "*3" << std::endl;

	// Used in actuall game
	/*
	if (mes != nullptr) {
		mes->tex = texName;
		mes->position = glm::vec3(place[0],place[1],place[2]);
		mes->rotation = glm::vec3(place[3],place[4],place[5]);
		mes->container.Setup(false, vout, (indexing.size() / 3)*(3 + cStride + 3), tout, triC * 3);
		mes->container.SetAttrib(0, 3, 3 + cStride + 3, 0);// Position
		mes->container.SetAttrib(1, cStride, 3 + cStride + 3, 3);// Color
		mes->container.SetAttrib(2, 3, 3 + cStride + 3, 3 + cStride);// Normal
	}*/

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
int ConvertAnim(std::string ipath) {
	std::string opath = ipath.substr(0, ipath.length() - 7) + "anim";
	int err;
	std::ifstream ifile(ipath);
	if (!ifile) {
		std::cout << "Could not find " << ipath << std::endl;
		ifile.close();
		return NotFound;
	}
	std::ofstream file(opath, std::ios::binary);
	if (!file) {
		std::cout << "Could not create/find " << opath << std::endl;
		file.close();
		return NotFound;
	}
	char setup = 0;
	bool tex = false;

	// Data
	unsigned short start=0;
	unsigned short end=0;
	float speed = 1;
	unsigned char loop = 0;
	unsigned char objects = 0;
	std::vector<std::string> objectNames;
	std::vector<unsigned short*> curves;
	std::vector<char> keysP;
	std::vector<unsigned short> keysF;
	std::vector<float> keysV;
	int oIndex;
	int cIndex;

	// Read
	int ind = 0; std::string line;
	while (getline(ifile, line)) {
		ind++;
		if (line[0] == '#' || line == "")
			continue;
		std::vector<std::string> set = SplitString(line, " ");
		if (set[0] == "start") {
			if (set.size() != 2) {
				std::cout << "Syntax Error at [" << ind << "] : Missing 1 parameter" << std::endl;
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			start = std::stoi(set[1]);
			setup++;
		} else if (set[0]=="end") {
			if (set.size() != 2) {
				std::cout << "Syntax Error at [" << ind << "] : Missing 1 parameter" << std::endl;
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			end = std::stoi(set[1]);

			setup++;
		} else if (set[0] == "speed") {
			if (set.size() != 2) {
				std::cout << "Syntax Error at [" << ind << "] : Missing 1 parameter" << std::endl;
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			float speed = std::stof(set[1]);

			setup++;
		} else if (set[0] == "loop") {
			if (set.size() != 2) {
				std::cout << "Syntax Error at [" << ind << "] : Missing 1 parameter" << std::endl;
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			loop = std::stoi(set[1]);

			setup++;
		} else if (setup != 4) {
			std::cout << "Syntax Error [" << ind << "] : Missing "<<(setup-4)<<" Info(s) Before" << std::endl;
			std::cout << " " << line << std::endl;
			file.close();
			return Syntax;
		} else if (set[0] == "object") {
			if (set.size() != 2) {
				if (set.size() > 2) {
					std::cout << "Syntax Error [" << ind << "] : Has << " << (set.size() - 2) << " additional parameter(s)" << std::endl;
				} else {
					std::cout << "Syntax Error [" << ind << "] : Missing " << (2 - set.size()) << " parameter(s)" << std::endl;
				}
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			oIndex = objectNames.size();
			objectNames.push_back(set[1]);
			curves.push_back(new unsigned short[18]{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});// FIX: MEMORY LEAK?
		} else if (set[0] == "keys") {
			if (set.size() != 2) {
				std::cout << "Syntax Error [" << ind << "] : Missing 1 parameter" << std::endl;
				std::cout << " " << line << std::endl;
				file.close();
				return Syntax;
			}
			if (set[1] == "loc0") {
				cIndex=0;
			}else if (set[1] == "loc1") {
				cIndex = 1;
			}else if (set[1] == "loc2") {
				cIndex = 2;
			}else if (set[1] == "rot0") {
				cIndex = 3;
			}else if (set[1] == "rot1") {
				cIndex = 4;
			}else if (set[1] == "rot2") {
				cIndex = 5;
			} else if (set[1] == "sca0") {
				cIndex = 6;
			} else if (set[1] == "sca1") {
				cIndex = 7;
			} else if (set[1] == "sca2") {
				cIndex = 8;
			}
			curves[oIndex][cIndex * 2] = keysP.size();
			curves[oIndex][cIndex * 2 + 1]=0;
		} else if (set[0] == "k") {
			if (set.size() != 4) {
				if (set.size() > 4) {
					std::cout << "Syntax Error [" << ind << "] : Has << " << (set.size() - 4) << " additional parameter(s)" << std::endl;
				} else {
					std::cout << "Syntax Error [" << ind << "] : Missing " << (4 - set.size()) << " parameter(s)" << std::endl;
				}
			}
			keysP.push_back(set[1][0]);
			keysF.push_back(std::stoi(set[2]));
			keysV.push_back(std::stof(set[3]));
			curves[oIndex][cIndex * 2+1]++;
		}
	}
	objects = objectNames.size();

	// Write
	char buf[2];
	buf[0] = (char)(start);
	buf[1] = (char)(start >> 8);
	file.write(buf, 2);// Start
	//std::cout <<"Start "<< start << std::endl;
	buf[0] = (char)(end);
	buf[1] = (char)(end >> 8);
	file.write(buf, 2);// End
	char* buff = reinterpret_cast<char*>(&speed);
	file.write(buff, sizeof(buff));// Speed
	buf[0] = (char)(loop);
	file.write(buf, 1);// Loop
	buf[0] = (char)(objects);
	file.write(buf, 1);// Objects

	for (int i = 0; i < objectNames.size();i++) {
		std::string name=objectNames[i];

		buf[0] = (unsigned short)name.length();
		file.write(buf, 1);// Text Length
		file.write(name.c_str(), name.length());

		unsigned short curveInt=0;
		for (int j = 0; j < 9; j++) {
			curveInt += (curves[i][j*2+1]>0) * pow(2, j);
		}

		buf[0] = (char)curveInt;
		buf[1] = (char)(curveInt >> 8);
		file.write(buf, 2);// Curves
		for (int j = 0; j < 9; j++) {
			unsigned short kIndex = curves[i][j * 2];
			unsigned short keys = curves[i][j * 2+1];
			if (keys>0) {
				buf[0] = (char)keys;
				buf[1] = (char)(keys >> 8);
				file.write(buf, 2);// Keys

				for (int k = 0; k < keys; k++) {
					buf[0] = keysP[kIndex+k];
					file.write(buf, 1);
					unsigned short frame = keysF[kIndex+k];
					buf[0] = (char)frame;
					buf[1] = (char)(frame >> 8);
					file.write(buf, 2);
					float v = keysV[kIndex + k];
					char* buff = reinterpret_cast<char*>(&v);
					file.write(buff, sizeof(buff));
				}
			}
		}
	}

	std::cout << "Successfully converted " << ipath << " to " << opath << std::endl;
	file.close();
	return Success;
}
int ReadAnim(std::string path) {
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		std::cout << "Could not find " << path << std::endl;
		file.close();
		return 0;
	}

	int atByte = 0;
	file.seekg(0, file.end);
	int fileSize = file.tellg();
	file.seekg(0, file.beg);

	atByte += 10;
	if (atByte > fileSize) {
		std::cout << "Corrupt Error: At byte " << atByte << std::endl;
		return Corrupt;
	}

	unsigned short valu[2];
	file.read(reinterpret_cast<char*>(&valu[0]), 2 * 2);
	unsigned short start = valu[0];
	std::cout << "Start: " << start << std::endl;
	unsigned short end = valu[1];
	std::cout << "End: " << end << std::endl;
	/*
	file.read(buf, 2);
	unsigned short start = 0x0000 | buf[1] << 8 | buf[0];
	std::cout << "Start: " << start << std::endl;

	file.read(buf, 2);
	unsigned short end = 0x0000 | buf[1] << 8 | buf[0];
	std::cout << "End: " << end << std::endl;
	*/
	float speed = 0;
	file.read(reinterpret_cast<char*>(&speed), 4);
	std::cout << "Speed: " << speed << std::endl;

	char buf[1];
	file.read(buf, 1);
	unsigned char loop = buf[0];
	std::cout << "Loop: " << (int)loop << std::endl;

	file.read(buf, 1);
	unsigned char objects = buf[0];
	std::cout << "Objects: " << (int)objects << std::endl;

	// Used in actual Game
	/*
	anim->frameStart = start;
	anim->frameEnd = end;
	anim->defaultSpeed = speed;
	anim->loop = loop;
	*/

	for (int i = 0; i < objects; i++) {
		atByte += 1;
		if (atByte > fileSize) {
			std::cout << "Corrupt Error: At byte " << atByte << std::endl;
			return Corrupt;
		}
		file.read(buf, 1);// Object Name Length
		unsigned char namelen = buf[0];
		char* cname = new char[namelen];
		atByte += namelen;
		if (atByte > fileSize) {
			std::cout << "Corrupt Error: At byte " << atByte << std::endl;
			return Corrupt;
		}
		file.read(cname, namelen);
		std::string name;
		for (int j = 0; j < namelen; j++) {
			name += cname[j];
		}
		atByte += 2;
		if (atByte > fileSize) {
			std::cout << "Corrupt Error: At byte " << atByte << std::endl;
			return Corrupt;
		}
		// TODO: IS BUG HERE WITH READING BYTES. Fix by using interpret cast and unsgined short[] array? See float reading for more info
		file.read(buf, 2);// Curves
		unsigned short curves = 0x0000 | buf[1] << 8 | buf[0];
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
		/*
		anim->curves[name] = FCurves();
		FCurves* fCurves = &anim->curves[name];
		*/
		for (int j = 0; j < 9; j++) {
			if (curveB[j]) {
				atByte += 2;
				if (atByte > fileSize) {
					std::cout << "Corrupt Error: At byte " << atByte << std::endl;
					return Corrupt;
				}
				file.read(buf, 2);// Keyframes
				unsigned short frames = 0x0000 | buf[1] << 8 | buf[0];
				std::cout << " Curve " << j << " " << (int)frames << std::endl;

				for (int k = 0; k < frames; k++) {
					atByte += 1+2+4;
					if (atByte > fileSize) {
						std::cout << "Corrupt Error: At byte " << atByte << std::endl;
						return Corrupt;
					}
					file.read(buf, 1);// Polation
					char polation = buf[0];
					file.read(buf, 2);// Frame
					unsigned short frame = 0x0000 | buf[1] << 8 | buf[0];
					float v = 0;
					file.read(reinterpret_cast<char*>(&v), 4);// Value
					std::cout << "  Key " << polation << " " << frame << " " << v << std::endl;
					/*
					fCurves->keyframes[j].keyframes.push_back(Keyframe(polation, frame, v));
					*/
				}
			}
		}
		delete cname;
	}

	// Cleanup
	file.close();
	return Success;
}