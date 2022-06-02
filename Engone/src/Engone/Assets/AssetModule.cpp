
#include "Engone/AssetModule.h"
#include "Engone/Components/Component.h"

#include "Engone/Utility/Utilities.h"

#define STB_IMAGE_IMPLEMENTATION
#include <vendor/stb_image/stb_image.h>
#define GLEW_STATIC
#include <GL/glew.h>

namespace engone {

	std::string toString(AssetError e){
		switch (e) {
		case MissingFile: return "Missing File";
		case CorruptedData: return "Corrupted Data";
		}
		return "";
	}
	std::string toString(AssetType type) {
		switch (type) {
		case AssetType::None: return "None";
		case AssetType::Texture: return "Texture";
		case AssetType::Font: return "Font";
		case AssetType::Shader: return "Shader";
		case AssetType::Material: return "Material";
		case AssetType::Mesh: return "Mesh";
		case AssetType::Animation: return "Animation";
		case AssetType::Armature: return "Armature";
		case AssetType::Model: return "Model";
		case AssetType::Collider: return "Collider";
		}
		return "";
	}
	namespace log {
		logger operator<<(logger log, AssetType type) {
			return log << toString(type);
		}
		logger operator<<(logger log, AssetError err) {
			return log << toString(err);
		}
	}
	void Texture::load(const std::string& path) {
		//Logging({ "AssetManager","Texture","Path: " + path }, LogStatus::Info);

		//std::cout << "Texture " << path << "\n";
		if (!FindFile(path)) {
			error = MissingFile;
			//Logging({ "AssetManager","Texture",toString(error) + ": " + path }, LogStatus::Error);
			//log::out << log::RED << log::TIME<< toString(error) << ": " << path << "\n"<<log::SILVER;
			return;
		}
		//if (path.length() > 4) {
			stbi_set_flip_vertically_on_load(1);
			buffer = stbi_load(path.c_str(), &width, &height, &BPP, 4);

			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			glBindTexture(GL_TEXTURE_2D, 0);

			//log::out << log::SILVER <<log::TIME << " Loaded " << path << "\n";

			if (buffer)
				stbi_image_free(buffer);
		//}
	}
	void Texture::init(int w, int h, void* data)
	{
		width = w;
		height = h;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	void Texture::subData(int x, int y, int w, int h, void* data)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	void Texture::bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, id);
	}
	int Texture::getWidth()
	{
		return width;
	}
	int Texture::getHeight()
	{
		return height;
	}
	void Shader::load(const std::string& path) {
		log::out << log::RED<<"Shader.load is deleted" << "\n";
#ifdef gone
		std::ifstream file(path);
		if (!file) {
			error = MissingFile;
			//Logging({ "AssetManager","Shader",toString(error) + ": " + path }, LogStatus::Error);
			return;
		}

		enum class ShaderType
		{
			NONE = -1, VERTEX = 0, FRAGMENT = 1
		};

		std::string line;
		std::stringstream ss[2];
		ShaderType type = ShaderType::NONE;
		while (getline(file, line)) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos) {
					type = ShaderType::FRAGMENT;
				}
			}
			else {
				ss[(int)type] << line << "\n";
				if (type == ShaderType::VERTEX)
					section[1]++;
				else
					section[2]++;
			}
		}
		//log::out << id << "\n";

		id = createShader(ss[0].str(), ss[1].str());
#endif
	}
	void Shader::init(const std::string& source)
	{
		std::string& vertex=vs, &fragment = fs;

		size_t vertPos = source.find("#shader vertex\n");
		size_t fragPos = source.find("#shader fragment\n");
		
		section[0] = 1;
		section[1] = 1;
		section[2] = 0;

		for (size_t i = 0; i < source.length(); ++i) {
			if (source[i] == '\n') {
				if (i < vertPos)
					section[0]++;
				if (i < fragPos)
					section[1]++;
				section[2]++;
			}
		}

		if (vertPos != -1 && fragPos != -1) {
			vertex = source.substr(vertPos + 15, fragPos - vertPos - 15);
			fragment = source.substr(fragPos + 17);
		}

		if (vertex.size() == 0 || fragment.size() == 0) {
			std::cout << "Is this shader source correct?:\n" << source << "\n";
		}


		//fs = fragment;
		//fs = vertex;

		id = createShader(vertex, fragment);

		//log::out << id<< " other \n";
	}
	unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		unsigned int program = glCreateProgram();
		unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
		unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}
	unsigned int Shader::compileShader(unsigned int type, const std::string& source)
	{
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			error = CorruptedData;
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			//char* message = (char*)alloca(length * sizeof(char));
			std::string message(length, ' ');

			glGetShaderInfoLog(id, length, &length, message.data());
			//std::cout << "Compile error with " << filePath << " (" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << ")\n";

			std::cout << message << "\n";

			//int same = 0;
			//std::string stuff = "ERROR: 0:";
			//for (int i = 0; i < 0;i++) {
			//	/*if (stuff[same] == message[i])
			//		same++;
			//	else
			//		same = 0;

			//	if (same == stuff.length()) {
			//		same = 0;
			//		// read line number
			//		std::string line = "";
			//		while (message[i] != ' ') {
			//			line += message[i];
			//		}
			//		std::cout << "["<<line<<"]";
			//	} else {
			//	}
			//	*/
			//		std::cout << message[i];
			//}

			/*int at = -1;
			while (at=message.find("ERROR: 0:")) {
				std::cout << std::string_view(message.c_str(),at) << "\n";

			}*/

			/*std::string number;
			for (int i = 0; i < length; i++) {
				if (i == 0 || message[i] == '\n' && i != length - 1) {
					if (message[i] == '\n') {
						i++;
						std::cout << '\n';
					}
					i += 2;

					while (message[i] != ')') {
						number += message[i];
						i++;
					}
					if (type == GL_VERTEX_SHADER)
						std::cout << "VS " << (std::stoi(number) + section[0]);
					else
						std::cout << "FS " << (std::stoi(number) + section[1]);
					number = "";
					i++;
				}

				std::cout << message[i];
			}
			std::cout << "\n";*/

			glDeleteShader(id);

			return 0;
		}

		return id;
	}
	void Shader::bind()
	{
		//glLinkProgram(id);
		//glValidateProgram(id);
		glUseProgram(id);

	}
	void Shader::setFloat(const std::string& name, float f)
	{
		glUniform1f(getUniformLocation(name), f);
	}
	void Shader::setVec2(const std::string& name, glm::vec2 v)
	{
		glUniform2f(getUniformLocation(name), v.x, v.y);
	}
	void Shader::setIVec2(const std::string& name, glm::ivec2 v)
	{
		glUniform2i(getUniformLocation(name), v.x, v.y);
	}
	void Shader::setVec3(const std::string& name, glm::vec3 v)
	{
		glUniform3f(getUniformLocation(name), v.x, v.y, v.z);
	}
	void Shader::setIVec3(const std::string& name, glm::ivec3 v)
	{
		glUniform3i(getUniformLocation(name), v.x, v.y, v.z);
	}
	void Shader::setVec4(const std::string& name, float f0, float f1, float f2, float f3)
	{
		glUniform4f(getUniformLocation(name), f0, f1, f2, f3);
	}
	void Shader::setInt(const std::string& name, int v)
	{
		glUniform1i(getUniformLocation(name), v);
	}
	void Shader::setMat4(const std::string& name, glm::mat4 mat)
	{
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
	}
	unsigned int Shader::getUniformLocation(const std::string& name)
	{
		if (uniLocations.find(name) != uniLocations.end()) {
			return uniLocations[name];
		}
		//int num = glGetError();
		//const GLubyte* okay = glewGetErrorString(num);
		//std::cout << num << "\n";
		unsigned int loc = glGetUniformLocation(id, name.c_str());
		//int num = glGetError();
		//const GLubyte* okay = glewGetErrorString(num);
		//std::cout << num << "\n";
		uniLocations[name] = loc;
		return loc;
	}
	void Font::load(const std::string& path)
	{
		std::vector<std::string> list;
		
		FileReader file(path,false);
		try {
			if (!file) return;
			std::string str;
			file.readAll(&str);
			std::vector<std::string> list = SplitString(str,"\n");

			if (list.size() == 2) {
				charWid[0] = std::stoi(list.at(0));
				int num = std::stoi(list.at(1));
				for (size_t i = 1; i < 256; ++i) {
					charWid[i] = num;
				}
			} else {
				size_t i = 0;
				for (std::string s : list) {
					std::vector<std::string> list2 = SplitString(s, ",");
					for (std::string s2 : list2) {
						charWid[i] = std::stoi(s2);
						i++;
					}
				}
			}
			texture.load(path + ".png");
			if (!texture) {
				error = texture.error;
			}
		} catch (AssetError err) {
			error = err;
		}
	}
	float Font::getWidth(const std::string& str, float height) {
		float out=0;
		for (size_t i = 0; i < str.length(); ++i) {
			unsigned char chr = str[i];
			
			out +=  height*(charWid[chr] / (float)charSize);
		}
		return out;
	}
	void MaterialAsset::load(const std::string& path)
	{
		// free/reset data? no need though

		FileReader file(path);
		try {
			std::string diffuse_mapName;
			std::string root = getRootPath();
			file.read(&diffuse_mapName);
			//std::cout << "Path: "<< root << diffuse_mapName << "\n";
			if(diffuse_mapName.length()!=0)
				diffuse_map = GetAsset<Texture>(root+diffuse_mapName);

			//std::cout << "Diffuse "<<diffuse_map << "\n";

			file.read(&diffuse_color);
			file.read(&specular);
			file.read(&shininess);

			//std::cout << diffuse_color[0] << " " << diffuse_color[1] << " " << diffuse_color[2] << "\n";
		}
		catch (AssetError err) {
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
		else // Expensive. Is it possible to optimize?
			quater *= glm::mat4_cast(glm::normalize(glm::slerp(glm::quat(1, 0, 0, 0), glm::slerp(q0, q1, slerpT), blend)));
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
		// clear data
		// clear data in side channels?
		objects.clear();

		FileReader file(path);
		try {
			file.read(&frameStart);
			file.read(&frameEnd);
			file.read(&defaultSpeed);
			
			uint8_t objectCount;
			file.read(&objectCount);

			//log::out << "obs " << objectCount << "\n";

			for (size_t i = 0; i < objectCount; ++i) {
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

						for (size_t k = 0; k < keys; ++k) {
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
		catch (AssetError err) {
			error = err;
			//Logging({ "AssetManager","Animation",toString(err) + ": " + path }, LogStatus::Error);
		}
		//file.close();
	}
	void MeshAsset::load(const std::string& path)
	{
		// clear data
		materials.clear();
		//buffer.Uninit();

		FileReader file(path);		
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
			for (size_t i = 0; i < materialCount && i< MeshAsset::maxMaterials; ++i) {
				std::string materialName;
				file.read(&materialName);

				//std::cout << "Matloc: " << root<<materialName<< "\n";
				MaterialAsset* asset = GetAsset<MaterialAsset>(root + materialName);
				
				if(asset)
					materials.push_back(asset);
				else{
					//std::cout << "Damn error\n";
				}
				
				//std::cout << materials.back()->error << " err\n";
			}
			if (materialCount == 0) {
				MaterialAsset* asset = GetAsset<MaterialAsset>("defaultMaterial");
				materials.push_back(asset);
			}
			//std::cout << "uh2u\n";
			uint16_t weightCount = 0,triangleCount;
			if(meshType==MeshType::Boned){
				file.read(&weightCount);
			}
			file.read(&triangleCount);
			
			//std::cout << "Points " << pointCount << " Textures " << textureCount <<" Triangles: "<<triangleCount<<" Weights "<<weightCount<<" Mats " << (int)materialCount << "\n";

			size_t uPointSize = 3 * pointCount;
			float* uPoint = new float[uPointSize];
			file.read(uPoint, uPointSize);

			size_t uTextureSize = textureCount * 3;
			float* uTexture = new float[uTextureSize];

			file.read(uTexture, uTextureSize);

			// Weight
			size_t uWeightS = weightCount * 3;
			int* uWeightI = new int[uWeightS];
			float* uWeightF = new float[uWeightS];
			if (meshType==MeshType::Boned) {
				char index[3];
				float floats[3];
				for (size_t i = 0; i < weightCount; ++i) {
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

			size_t tStride = 6;
			if (meshType==MeshType::Boned)
				tStride = 9;
			size_t trisS = triangleCount * tStride;
			uint16_t* tris = new uint16_t[trisS];
			//std::cout << "head: "<<file.readHead << "\n";

			//file.file.read(reinterpret_cast<char*>(tris), trisS*2);
			file.read(tris, trisS);
			
			//std::cout << "err? " << file.error << "\n";
			//std::cout << file.readHead << " "<<trisS<< "\n";

			//std::cout << "stride " << tStride << "\n";
			for (size_t i = 0; i < trisS;++i) {
				 //for (int j = 0; j < tStride;j++) {
					//std::cout << tris[i]<<" ";
				//}
				//std::cout << "\n";
			}

			std::vector<uint16_t> indexNormal;
			std::vector<float> uNormal;
			for (size_t i = 0; i < triangleCount; ++i) {
				for (size_t j = 0; j < 3; ++j) {
					if (tris[i * tStride + j * tStride / 3] * 3u + 2u >= uPointSize) {
						//std::cout << "Corruption at '" << i <<" "<< (i * tStride)<<" "<<(j * tStride / 3) <<" "<< tris[i*tStride+j*tStride/3] << "' : Triangle Index\n";
						throw AssetError::CorruptedData;
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
				for (size_t j = 0; j < uNormal.size() / 3; ++j) {
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
			size_t* triangleOut = new size_t[triangleCount * 3];

			size_t uvStride = 1 + (tStride) / 3;
			for (size_t i = 0; i < triangleCount; ++i) {
				for (size_t t = 0; t < 3; ++t) {
					bool same = false;
					for (size_t v = 0; v < uniqueVertex.size() / (uvStride); ++v) {
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

			size_t vStride = 3 + 3 + 3;
			if (meshType == MeshType::Boned)
				vStride += 6;
			float* vertexOut = new float[(uniqueVertex.size() / uvStride) * vStride];
			for (size_t i = 0; i < uniqueVertex.size() / uvStride; i++) {
				// Position
				for (size_t j = 0; j < 3; ++j) {
					if (uniqueVertex[i * uvStride] * 3 + j > uPointSize) {
						//bug::out < bug::RED < "Corruption at '" < path < "' : Position Index\n";
						throw AssetError::CorruptedData;
					}
					vertexOut[i * vStride + j] = uPoint[uniqueVertex[i * uvStride] * 3 + j];
				}
				// Normal
				for (size_t j = 0; j < 3; ++j) {
					if (uniqueVertex[i * uvStride + 1] * 3 + j > uNormal.size()) {
						//bug::out < bug::RED < "Corruption at '" < path < "' : Normal Index\n";
						throw AssetError::CorruptedData;
					}
					vertexOut[i * vStride + 3 + j] = uNormal[uniqueVertex[i * uvStride + 1] * 3 + j];
				}
				// UV
				for (size_t j = 0; j < 3; ++j) {
					if (uniqueVertex[i * uvStride + 2] * 3 + j > uTextureSize) {
						//bug::out < bug::RED < "Corruption at '" < path < "' : Color Index\n";
						//bug::out < (uniqueVertex[i * uvStride + 2] * 3 + j) < " > " < uTextureSize < bug::end;
						throw AssetError::CorruptedData;
					}
					else
						vertexOut[i * vStride + 3 + 3 + j] = (float)uTexture[uniqueVertex[i * uvStride + 2] * 3 + j];
				}
				if (meshType == MeshType::Boned) {
					// Bone Index
					for (size_t j = 0; j < 3; ++j) {
						if (uniqueVertex[i * uvStride + 3] * 3 + j > uWeightS) {
							//bug::out < bug::RED < "Corruption at '" < path < "' : Bone Index\n";
							throw AssetError::CorruptedData;
						}
						vertexOut[i * vStride + 3 + 3 + 3 + j] = (float)uWeightI[uniqueVertex[i * uvStride + 3] * 3 + j];
					}
					// Weight
					for (size_t j = 0; j < 3; ++j) {
						if (uniqueVertex[i * uvStride + 3] * 3 + j > uWeightS) {
							//bug::out < bug::RED < "Corruption at '" < path < "' : Weight Index\n";
							throw AssetError::CorruptedData;
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
			
			vertexBuffer.setData((uniqueVertex.size() / uvStride)* vStride, vertexOut);
			indexBuffer.setData(triangleCount * 3, triangleOut);
			
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
			delete[] uPoint;
			delete[] uTexture;
			delete[] uWeightI;
			delete[] uWeightF;
			delete[] tris;
			delete[] vertexOut;
			delete[] triangleOut;
		}
		catch (AssetError err) {/*
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
	void ColliderAsset::load(const std::string& path)
	{
		if (colliderType == Type::Sphere) {

		}else if (colliderType == Type::Cube) {

		}else if (colliderType == Type::Mesh) {
			points.clear();
			tris.clear();
		}
		
		//ints.push_back(2);
		//ints.push_back(6);

		FileReader file(path);
		try {
			//uint8_t movable;
			//file.read(&movable);

			file.read(&colliderType);

			//log::out << (int)colliderType << " type\n";

			switch (colliderType) {
			case Type::Sphere:
				file.read(&sphere.radius);
				file.read(&sphere.position);
				furthest = sphere.radius;
				//log::out << cube.scale << " radius \n";
				break;
			case Type::Cube:
				file.read(&cube.scale);
				file.read(&cube.position);
				//log::out << cube.scale<<" scale \n";
				furthest = glm::length(cube.scale);
				break;
			case Type::Mesh:
				
				uint16_t triCount,pointCount;
				file.read(&pointCount);
				file.read(&triCount);
				file.read(&furthest);

				/*if (triCount > 100000 || pointCount > 100000||furthest<0){
					log::out << "something may be corrupted\n";
					throw CorruptedData;
				}*/

				points.resize(pointCount);
				tris.resize(triCount*3);
				for (size_t i = 0; i < points.size();++i) {
					file.read(&points[i]);
				}
				for (size_t i = 0; i < tris.size(); ++i) {
					file.read(&tris[i]);
				}

				break;
			}
		}
		catch (AssetError err) {
			error = err;
			//Logging({ "AssetManager","Collider",toString(err) + ": " + path }, LogStatus::Error);
		}
		//file.close();
	}
	void ArmatureAsset::load(const std::string& path)
	{
		bones.clear();

		FileReader file(path);
		try {
			uint8_t boneCount;
			file.read(&boneCount);

			//log::out << path << "\n";
			//log::out << boneCount << "\n";
	
			// Acquire and Load Data
			for (size_t i = 0; i < boneCount; ++i) {
				Bone b;
				file.read(&b.parent);
				//log::out << b.parent << "\n";
				//log::out << i<<" matrix" << "\n";
				file.read(&b.localMat);
				file.read(&b.invModel);

				bones.push_back(b);
			}
		}
		catch (AssetError err) {
			error = err;
			//Logging({ "AssetManager","Armature",toString(err) + ": " + path }, LogStatus::Error);
		}
		//file.close();
	}
	void ModelAsset::load(const std::string& path)
	{
		instances.clear();
		animations.clear();

		FileReader file(path);
		try {
			std::string root = getRootPath();

			uint16_t instanceCount;
			file.read(&instanceCount);
			for (size_t i = 0; i < instanceCount; ++i) {
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
					instance.asset = GetAsset<MeshAsset>(root + name);
					break;
				case 1:
					instance.asset = GetAsset<ArmatureAsset>(root + name);
					break;
				case 2:
					instance.asset = GetAsset<ColliderAsset>(root + name);
					break;
				}

				file.read(&instance.parent);
				file.read(&instance.localMat);
				//file.read(&instance.invModel);
			}
			
			uint16_t animationCount;
			file.read(&animationCount);
			for (size_t i = 0; i < animationCount; ++i) {
				std::string name;
				file.read(&name);
				animations.push_back(GetAsset<AnimationAsset>(root + name));
			}
		}
		catch (AssetError err) {
			error = err;
			//Logging({ "AssetManager","Model",toString(err) + ": " + path }, LogStatus::Error);
		}
		//file.close();
	}
	void ModelAsset::getParentTransforms(Animator* animator, std::vector<glm::mat4>& mats) {
		mats.resize(instances.size());

		std::vector<glm::mat4> modelT(instances.size());
		//log::out << "go "<< "\n";
		for (size_t i = 0; i < instances.size(); ++i) {
			AssetInstance& instance = instances[i];
			glm::mat4 loc = instances[i].localMat;
			glm::mat4 inv = instances[i].invModel;

			glm::vec3 pos = { 0,0,0 };
			glm::vec3 euler = { 0,0,0 };
			glm::vec3 scale = { 1,1,1 };
			glm::mat4 quater = glm::mat4(1);

			short usedChannels = 0;

			for (size_t k = 0; k < Animator::maxAnimations; ++k) {
				if (animator->enabledAnimations[i].asset) {
					AnimationProperty& prop = animator->enabledAnimations[k];
					for (size_t j = 0; j < animations.size(); ++j) {
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
	}
	void ModelAsset::getArmatureTransforms(Animator* animator, std::vector<glm::mat4>& mats, glm::mat4& instanceMat, AssetInstance* instance, ArmatureAsset* armature) {
		mats.resize(armature->bones.size());
		if (armature != nullptr) {
			std::vector<glm::mat4> modelT(armature->bones.size());
			
			for (size_t i = 0; i < armature->bones.size(); ++i) {
				Bone& bone = armature->bones[i];
				glm::mat4 loc = bone.localMat;
				glm::mat4 inv = bone.invModel;
				glm::vec3 pos = { 0,0,0 };
				glm::vec3 euler = { 0,0,0 };
				glm::vec3 scale = { 1,1,1 };
				glm::mat4 quater = glm::mat4(1);

				short usedChannels = 0;

				for (size_t k = 0; k < Animator::maxAnimations; ++k) {
					if (animator->enabledAnimations[k].asset) {

						AnimationProperty& prop = animator->enabledAnimations[k];
						for (size_t j = 0; j < animations.size(); ++j) {
							AnimationAsset* animation = animations[j];
							
							if (prop.asset == animation &&
								std::strcmp(prop.instanceName, instance->name.c_str()) == 0) {

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
		}
	}

	std::unordered_map<std::string, Asset*> engone_assets[ASSET_TYPES];
}