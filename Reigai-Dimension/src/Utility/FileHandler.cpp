#include "FileHandler.h"
#include <sys/stat.h>

int FHFileExist(std::string path) {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

int WriteTextFile(std::string path, std::vector<std::string> text) {
	std::ofstream file(path);
	path += ".txt";
	std::vector<std::string> out;
	if (!file) {
		//std::cout << "[WriteTextFile] FHFindError: "<< path << std::endl;
		return FHNotFound;
	}
	else {
		for (int i = 0; i < text.size(); i++) {
			if (i != 0)
				file << "\n";
			file << text[i];
		}
	}
	file.close();
	return FHSuccess;
}

std::vector<std::string> ReadFileList(std::string path,int* err){
	*err = FHSuccess;
	path += ".txt";
	std::ifstream file(path);

	std::vector<std::string> out;
	if (!file) {
		//std::cout << "[ReadFileList] FHFindError: " << path << std::endl;
		*err = FHNotFound;
		return out;
	}
	std::string line;
	while (getline(file, line))
	{
		out.push_back(line);
	}
	file.close();
	return out;
}
std::string ReadFile(std::string path,int* err){
	*err = FHSuccess;
	path += ".txt";
	std::ifstream file(path);
	if (!file) {
		//std::cout << "[ReadFile] FHFindError: " << path << std::endl;
		*err = FHNotFound;
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
int FHLoadMesh(MeshData* da, std::string path) {
	path += ".mesh";
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

	atByte += 7 + 4 * 6;
	if (atByte > fileSize) {
		std::cout << "Corrupt Error: At byte " << atByte << std::endl;
		return FHCorrupt;
	}

	char buf[2];
	unsigned short valu[3];
	file.read(reinterpret_cast<char*>(&valu[0]), 2 * 3);// Vertex Count
	unsigned short posC = valu[0];
	unsigned short colorC = valu[1];
	unsigned short triC = valu[2];
	//std::cout << "Points: " << posC << std::endl;
	//std::cout << "Colors: " << colorC << std::endl;
	//std::cout << "Triangles: " << triC << std::endl;

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
			return FHCorrupt;
		}
		file.read(name, texLen);// Texture Name
		for (int i = 0; i < texLen; i++) {
			texName += name[i];
		}
		//std::cout << "Texture: " << texName << std::endl;
		delete name;
	} else {
		//std::cout << "No Texture" << std::endl;
	}

	float* place = new float[6];
	file.read(reinterpret_cast<char*>(&place[0]), 4 * 6);

	//std::cout << "Position " << place[0] << " " << place[1] << " " << place[2] << std::endl;
	//std::cout << "Rotation " << place[3] << " " << place[4] << " " << place[5] << std::endl;

	int uPosS = 3 * posC;
	int uColorS = colorC * cStride;
	int trisS = triC * 6;

	atByte += 4 * uPosS + 4 * uColorS + 2 * trisS;
	if (atByte > fileSize) {
		std::cout << "Corrupt Error: At byte " << atByte << std::endl;
		return FHCorrupt;
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
		for (int j = 0; j < 3 * 3; j++) {
			if (tris[i * 6 + j / 3 * 2] * 3 + j - j / 3 * 3 > uPosS) {
				std::cout << "Syntax Error: Triangle Index" << std::endl;
				return FHCorrupt;
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

	float* vout = new float[(indexing.size() / 3)*(3 + cStride + 3)];
	for (int i = 0; i < indexing.size() / 3; i++) {
		// Position
		for (int j = 0; j < 3; j++) {
			if (indexing[i * 3] * 3 + j > uPosS) {
				std::cout << "Corrupt Error: Position Index" << std::endl;
				return FHCorrupt;
			}
			vout[i*(3 + cStride + 3) + j] = uPos[indexing[i * 3] * 3 + j];
		}
		// Color
		for (int j = 0; j < cStride; j++) {
			if (indexing[i * 3 + 1] * cStride + j > uColorS) {
				std::cout << "Corrupt Error: Color Index" << std::endl;
				return FHCorrupt;
			}
			vout[i*(3 + cStride + 3) + 3 + j] = uColor[indexing[i * 3 + 1] * cStride + j];
		}

		// Normal
		for (int j = 0; j < 3; j++) {
			if (indexing[i * 3 + 2] * 3 + j > uNormal.size()) {
				std::cout << "Corrupt Error: Normal Index" << std::endl;
				return FHCorrupt;
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

	//std::cout << "VertexBuffer " << (indexing.size() / 3) << "*" << (3 + cStride + 3) << " IndexBuffer " << (triC) << "*3" << std::endl;

	// Used in actuall game
	
	if (da != nullptr) {
		da->tex = texName;
		da->position = glm::vec3(place[0],place[1],place[2]);
		da->rotation = glm::vec3(place[3],place[4],place[5]);
		da->container.Setup(false, vout, (indexing.size() / 3)*(3 + cStride + 3), tout, triC * 3);
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
	return FHSuccess;
}
int FHLoadAnim(AnimData* da, std::string path){
	path += ".anim";
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
		return FHCorrupt;
	}

	char buf[2];
	file.read(buf, 2);
	unsigned short start = 0x0000 | buf[1] << 8 | buf[0];
	//std::cout << "Start: " << start << std::endl;

	file.read(buf, 2);
	unsigned short end = 0x0000 | buf[1] << 8 | buf[0];
	//std::cout << "End: " << end << std::endl;

	float speed = 0;
	file.read(reinterpret_cast<char*>(&speed), 4);
	//std::cout << "Speed: " << speed << std::endl;

	file.read(buf, 1);
	unsigned char loop = buf[0];
	//std::cout << "Loop: " << (int)loop << std::endl;

	file.read(buf, 1);
	unsigned char objects = buf[0];
	//std::cout << "Objects: " << (int)objects << std::endl;

	// Used in actual Game
	
	da->frameStart = start;
	da->frameEnd = end;
	da->defaultSpeed = speed;
	da->loop = loop;
	

	for (int i = 0; i < objects; i++) {
		atByte += 1;
		if (atByte > fileSize) {
			std::cout << "Corrupt Error: At byte " << atByte << std::endl;
			return FHCorrupt;
		}
		file.read(buf, 1);// Object Name Length
		unsigned char namelen = buf[0];
		char* cname = new char[namelen];
		atByte += namelen;
		if (atByte > fileSize) {
			std::cout << "Corrupt Error: At byte " << atByte << std::endl;
			return FHCorrupt;
		}
		file.read(cname, namelen);
		std::string name;
		for (int j = 0; j < namelen; j++) {
			name += cname[j];
		}
		atByte += 2;
		if (atByte > fileSize) {
			std::cout << "Corrupt Error: At byte " << atByte << std::endl;
			return FHCorrupt;
		}
		file.read(buf, 2);// Curves
		unsigned short curves = 0x0000 | buf[1] << 8 | buf[0];
		//std::cout << "Object " << name << " " << curves << std::endl;
		bool curveB[9]{ 0,0,0,0,0,0,0,0,0 };
		for (int j = 8; j >= 0; j--) {
			if (0 <= curves - pow(2, j)) {
				curves -= pow(2, j);
				curveB[j] = 1;
			} else {
				curveB[j] = 0;
			}
		}
		
		da->curves[name] = FCurves();
		FCurves* fCurves = &da->curves[name];
		
		for (int j = 0; j < 9; j++) {
			if (curveB[j]) {
				atByte += 2;
				if (atByte > fileSize) {
					std::cout << "Corrupt Error: At byte " << atByte << std::endl;
					return FHCorrupt;
				}
				file.read(buf, 2);// Keyframes
				unsigned short frames = 0x0000 | buf[1] << 8 | buf[0];
				//std::cout << " Curve " << j << " " << (int)frames << std::endl;

				for (int k = 0; k < frames; k++) {
					atByte += 1 + 2 + 4;
					if (atByte > fileSize) {
						std::cout << "Corrupt Error: At byte " << atByte << std::endl;
						return FHCorrupt;
					}
					file.read(buf, 1);// Polation
					char polation = buf[0];
					file.read(buf, 2);// Frame
					unsigned short frame = 0x0000 | buf[1] << 8 | buf[0];
					float v = 0;
					file.read(reinterpret_cast<char*>(&v), 4);// Value
					//std::cout << "  Key " << polation << " " << frame << " " << v << std::endl;
					
					fCurves->keyframes[j].keyframes.push_back(Keyframe(polation, frame, v));
					
				}
			}
		}
		delete cname;
	}

	// Cleanup
	file.close();
	return FHSuccess;
}
int FHLoadColl(CollData* co, std::string path) { // TODO: More efficient by removing unneccecery for loops
	//CollData* da=new CollData();
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

	atByte += 2 * 3;
	if (atByte > fileSize) {
		std::cout << "Corrupt Error: At byte " << atByte << std::endl;
		return FHCorrupt;
	}

	char buf[2];
	std::uint16_t valu[3];
	file.read(reinterpret_cast<char*>(&valu[0]), 2 * 3);// Vertex Count
	std::uint16_t vC = valu[0];
	std::uint16_t qC = valu[1];
	std::uint16_t tC = valu[2];

	//std::cout << "Points: " << vC << std::endl;
	//std::cout << "Quads: " << qC << std::endl;
	//std::cout << "Triangles: " << tC << std::endl;

	int vS = vC * 3;
	int qS = qC * 4;
	int tS = tC * 3;

	atByte += 4 * vS + 2 * qS + 2 * tS;
	if (atByte > fileSize) {
		std::cout << "Corrupt Error: At byte " << atByte << std::endl;
		return FHCorrupt;
	}

	float* uV = new float[vS];
	file.read(reinterpret_cast<char*>(&uV[0]), 4 * vS); // Positions

	std::uint16_t* uQ = new std::uint16_t[qS];
	file.read(reinterpret_cast<char*>(&uQ[0]), 2 * qS);// Colors

	std::uint16_t* uT = new std::uint16_t[tS];
	file.read(reinterpret_cast<char*>(&uT[0]), 2 * tS);// Triangles

	for (int i = 0; i < vC; i++) {
		co->points.push_back(glm::vec3(uV[i * 3], uV[i * 3 + 1], uV[i * 3 + 2]));
		//std::cout << "V " << uV[i * 3] << " " << uV[i * 3 + 1] << " " << uV[i * 3 + 2] << std::endl;
	}
	for (int i = 0; i < qC; i++) {
		//std::cout << "Q";
		for (int j = 0; j < 4; j++) {
			co->quad.push_back(uQ[i * 4 + j]);
			//std::cout << " " << uQ[i * 4 + j];
		}
		//std::cout << std::endl;
	}
	for (int i = 0; i < tC; i++) {
		//std::cout << "T";
		for (int j = 0; j < 3; j++) {
			co->quad.push_back(uT[i]);
			//std::cout << " " << uQ[i * 3 + j];
		}
		//std::cout << std::endl;
	}

	// Cleanup
	delete uV;
	delete uQ;
	delete uT;
	file.close();
	return FHSuccess;
}