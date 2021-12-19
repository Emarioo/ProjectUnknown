#include "gonpch.h"

#include "AssetHandler.h"

#include "../Utility/Utilities.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "DebugHandler.h"

#include "GL/glew.h"

namespace engone {

	std::string toString(AssetError e){
		switch (e) {
		case MissingFile: return "Missing File";
		case MissingData: return "Missing Data";
		case BadSyntax: return "Bad Syntax";
		case CorruptedData: return "Corrupted Data";
		}
	}
	std::string toString(AssetType type)
	{
		switch (type) {
		case AssetType::None:
			return "None";
		case AssetType::Texture:
			return "Texture";
		case AssetType::Font:
			return "Font";
		case AssetType::Shader:
			return "Shader";
		case AssetType::Material:
			return "Material";
		case AssetType::Mesh:
			return "Mesh";
		case AssetType::Animation:
			return "Animation";
		case AssetType::Armature:
			return "Armature";
		case AssetType::Model:
			return "Model";
		case AssetType::Collider:
			return "Collider";
		}
	}
	namespace log
	{
		logger operator<<(logger log, AssetType type)
		{
			log << toString(type);
			return log;
		}
	}
	void Texture::Load(const std::string& path)
	{
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
	void Texture::Init(int w, int h, void* data)
	{
		width = w;
		height = h;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	void Texture::SubData(int x, int y, int w, int h, void* data)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	void Texture::Bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, id);
	}
	int Texture::GetWidth()
	{
		return width;
	}
	int Texture::GetHeight()
	{
		return height;
	}
	void Shader::Load(const std::string& path)
	{
		std::ifstream file(path);
		if (!file) {
			error = MissingFile;
			Logging({ "AssetManager","Shader",toString(error) + ": " + path }, LogStatus::Error);
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

		id = CreateShader(ss[0].str(), ss[1].str());
	}
	void Shader::Init(const std::string& source)
	{
		std::string vertex, fragment;

		int vertPos = source.find("#shader vertex\n");
		int fragPos = source.find("#shader fragment\n");

		section[0] = 1;
		section[1] = 1;
		section[2] = 0;

		for (int i = 0; i < source.length(); i++) {
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
			std::cout << "Is this shader source correct?\n" << source << "\n";
		}

		id = CreateShader(vertex, fragment);
	}
	unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		unsigned int program = glCreateProgram();
		unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}
	unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
	{
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			error = BadSyntax;
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			std::cout << "Compile error with " << filePath << " (" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << ")\n";

			std::string number;
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
			std::cout << "\n";

			glDeleteShader(id);
			return 0;
		}

		return id;
	}
	void Shader::Bind()
	{
		glUseProgram(id);
	}
	void Shader::SetFloat(const std::string& name, float f)
	{
		glUniform1f(GetUniformLocation(name), f);
	}
	void Shader::SetVec2(const std::string& name, glm::vec2 v)
	{
		glUniform2f(GetUniformLocation(name), v.x, v.y);
	}
	void Shader::SetIVec2(const std::string& name, glm::ivec2 v)
	{
		glUniform2i(GetUniformLocation(name), v.x, v.y);
	}
	void Shader::SetVec3(const std::string& name, glm::vec3 v)
	{
		glUniform3f(GetUniformLocation(name), v.x, v.y, v.z);
	}
	void Shader::SetIVec3(const std::string& name, glm::ivec3 v)
	{
		glUniform3i(GetUniformLocation(name), v.x, v.y, v.z);
	}
	void Shader::SetVec4(const std::string& name, float f0, float f1, float f2, float f3)
	{
		glUniform4f(GetUniformLocation(name), f0, f1, f2, f3);
	}
	void Shader::SetInt(const std::string& name, int v)
	{
		glUniform1i(GetUniformLocation(name), v);
	}
	void Shader::SetMatrix(const std::string& name, glm::mat4 mat)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
	}
	unsigned int Shader::GetUniformLocation(const std::string& name)
	{
		if (uniLocations.find(name) != uniLocations.end()) {
			return uniLocations[name];
		}
		unsigned int loc = glGetUniformLocation(id, name.c_str());
		uniLocations[name] = loc;
		return loc;
	}

	void Font::Load(const std::string& path)
	{
		std::vector<std::string> list;
		
		FileReport err = ReadTextFile(path + ".txt", list);

		if (err == FileReport::Success) {
			if (list.size() == 2) {
				charWid[0] = std::stoi(list.at(0));
				int num = std::stoi(list.at(1));
				for (int i = 1; i < 256; i++) {
					charWid[i] = num;
				}
			}
			else {
				int i = 0;
				for (std::string s : list) {
					std::vector<std::string> list2 = SplitString(s, ",");
					for (std::string s2 : list2) {
						charWid[i] = std::stoi(s2);
						i++;
					}
				}
			}
			texture.Load(path+".png");
			if (!texture) {
				error = texture.error;
			}
		}
		else {
			error = MissingFile;
		}
		if (error) {
			Logging({ "AssetManager","Font",toString(error) + ": " + path }, LogStatus::Error);
		}
			
	}
	void MaterialAsset::Load(const std::string& path)
	{
		//std::cout << "load " << path << "\n";
		FileReader file(path);
		/*if (!file) {
			error = file.error;
			//std::cout <<" mae error "<<file.error<<"\n";
			return;
		}*/
		try {
			std::string diffuse_mapName;
			std::string root = GetRootPath();
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
			Logging({ "AssetManager","Material",toString(err) + ": " + path }, LogStatus::Error);
		}
		file.close();
		// add diffuse_map as a texture asset
	}
	Keyframe::Keyframe(PolationType polation, unsigned short frame, float value)
		: polation(polation), frame(frame), value(value)
	{
	}
	FCurve::FCurve() {}
	Keyframe& FCurve::Get(int index)
	{
		return frames.at(index);
	}
	void FCurve::Add(Keyframe keyframe)
	{
		frames.push_back(keyframe);
	}
	FCurve& Channels::Get(ChannelType channel)
	{
		return fcurves[channel];
	}
	/*
	"channel" is what type of value the keyframes will be changing. Example PosX.
	Create a "FCurve" object and see the "Add" function for more details.
	*/
	void Channels::Add(ChannelType channel, FCurve fcurve)
	{
		fcurves[channel] = fcurve;
	}
	void Channels::GetValues(int frame, float blend, glm::vec3& pos, glm::vec3& euler, glm::vec3& scale, glm::mat4& quater, short* usedChannels)
	{
		float build[9]{ 0,0,0,0,0,0,1,1,1 };
		glm::quat q0 = glm::quat(1, 0, 0, 0);
		glm::quat q1 = glm::quat(1, 0, 0, 0);
		//bug::out < "curve "< fcurves.size() < bug::end;
		float slerpT = 0;
		for (std::pair<ChannelType, FCurve> curve : fcurves) {
			ChannelType type = curve.first;
			Keyframe* a = nullptr;
			Keyframe* b = nullptr;
			for (Keyframe& k : curve.second.frames) {
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
					q0[curve.first - QuaX] = a->value;
					*usedChannels = *usedChannels | (1 << curve.first);
					continue;
				}
				else {
					build[curve.first] = a->value;
					*usedChannels = *usedChannels | (1 << curve.first);
					continue;
				}
			}
			//std::cout << a->frame << " " << b->frame << std::endl;
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
				q0[curve.first - QuaX] = a->value;
				q1[curve.first - QuaX] = b->value;
				*usedChannels = *usedChannels | (1 << curve.first);
			}
			else {
				build[curve.first] = a->value * (1 - lerp) + b->value * lerp;
				*usedChannels = *usedChannels | (1 << curve.first);
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


		//std::cout << slerpT << std::endl;

		//bug::outs < "b " <build[1] < "\n";

		if (q0 == q1 || slerpT == 0) {
			quater *= glm::mat4_cast(glm::slerp(glm::quat(1, 0, 0, 0), q0, blend));
		}
		else if (slerpT == 1) {
			quater *= glm::mat4_cast(glm::slerp(glm::quat(1, 0, 0, 0), q1, blend));
		}
		else // Expensive. Is it possible to optimize?
			quater *= glm::mat4_cast(glm::slerp(glm::quat(1, 0, 0, 0), glm::slerp(q0, q1, slerpT), blend));
	}
	Channels& AnimationAsset::Get(unsigned short i)
	{
		return objects[i];
	}
	/*
	objectIndex is the index of the bone. Also known as the vertex group in blender.
	Create a "Channels" object and see the "Add" function for more details.
	*/
	void AnimationAsset::AddObjectChannels(int objectIndex, Channels channels)
	{
		objects[objectIndex] = channels;
	}
	/*
	Use this to create an animation by code.
	See the "AddObjectChannels" function for more details.
	*/
	void AnimationAsset::Modify(unsigned short startFrame, unsigned short endFrame)
	{
		frameStart = startFrame;
		frameEnd = endFrame;
	}
	/*
	Use this to create an animation by code.
	"speed" is 24 by default. Speed can also be changed in the animator.
	See the "AddObjectChannels" function for more details.
	*/
	void AnimationAsset::Modify(unsigned short startFrame, unsigned short endFrame, float speed)
	{
		frameStart = startFrame;
		frameEnd = endFrame;
		defaultSpeed = speed;
	}
	void AnimationAsset::Load(const std::string& path)
	{
		FileReader file(path);
		/*if (!file) {
			error = file.error;
			return;
		}*/
		try {
			file.read(&frameStart);
			file.read(&frameEnd);
			file.read(&defaultSpeed);
			
			uint8_t objectCount;
			file.read(&objectCount);

			for (int i = 0; i < objectCount; i++) {
				uint16_t index,curves;
				
				file.read(&index);
				file.read(&curves);

				bool curveB[13]{ 0,0,0,0,0,0,0,0,0,0,0,0,0 };
				for (int j = 12; j >= 0; j--) {
					if (0 <= curves - pow(2, j)) {
						curves -= pow(2, j);
						curveB[j] = 1;
					}
					else {
						curveB[j] = 0;
					}
				}

				objects[index] = Channels();
				Channels* channels = &objects[index];

				const std::string curve_order[]{ "PX","PY","PZ","RX","RY","RZ","SX","SY","SZ","QX","QY","QZ","QW" };
				for (ChannelType cha = PosX; cha < 13; cha = (ChannelType)(cha + 1)) {
					if (curveB[cha]) {
						uint16_t keys;
						file.read(&keys);

						channels->fcurves[cha] = FCurve();
						FCurve* fcurve = &channels->fcurves[cha];

						for (int k = 0; k < keys; k++) {
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
			Logging({ "AssetManager","Animation",toString(err) + ": " + path }, LogStatus::Error);
		}
		file.close();
	}
	void MeshAsset::Load(const std::string& path)
	{
		//std::cout << "Try load "<<path << "\n";
		FileReader file(path);
		/*if (!file) {
			error = file.error;
			return;
		}*/
		try {
			uint16_t pointCount;
			uint16_t textureCount;
			uint8_t materialCount;
			file.read(&meshType);
			file.read(&pointCount);
			file.read(&textureCount);
			file.read(&materialCount);
			//std::cout << "uhu\n";
			std::string root = GetRootPath();
			for (int i = 0; i < materialCount; i++) {
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

			int uPointSize = 3 * pointCount;
			float* uPoint = new float[uPointSize];
			file.read(uPoint, uPointSize);

			int uTextureSize = textureCount * 3;
			float* uTexture = new float[uTextureSize];

			file.read(uTexture, uTextureSize);

			// Weight
			int uWeightS = weightCount * 3;
			int* uWeightI = new int[uWeightS];
			float* uWeightF = new float[uWeightS];
			if (meshType==MeshType::Boned) {
				char index[3];
				float floats[3];
				for (int i = 0; i < weightCount; i++) {
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

			int tStride = 6;
			if (meshType==MeshType::Boned)
				tStride = 9;
			int trisS = triangleCount * tStride;
			uint16_t* tris = new uint16_t[trisS];
			//std::cout << "head: "<<file.readHead << "\n";

			//file.file.read(reinterpret_cast<char*>(tris), trisS*2);
			file.read(tris, trisS);
			
			//std::cout << "err? " << file.error << "\n";
			//std::cout << file.readHead << " "<<trisS<< "\n";

			//std::cout << "stride " << tStride << "\n";
			for (int i = 0; i < trisS;i++) {
				 //for (int j = 0; j < tStride;j++) {
					//std::cout << tris[i]<<" ";
				//}
				//std::cout << "\n";
			}

			std::vector<uint16_t> indexNormal;
			std::vector<float> uNormal;
			for (int i = 0; i < triangleCount; i++) {
				for (int j = 0; j < 3; j++) {
					if (tris[i * tStride + j * tStride / 3] * 3 + 2 >= uPointSize) {
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

			std::vector<unsigned short> uniqueVertex;// [ posIndex,colorIndex,normalIndex,weightIndex, ...]
			unsigned int* triangleOut = new unsigned int[triangleCount * 3];

			int uvStride = 1 + (tStride) / 3;
			for (int i = 0; i < triangleCount; i++) {
				for (int t = 0; t < 3; t++) {
					bool same = false;
					for (int v = 0; v < uniqueVertex.size() / (uvStride); v++) {
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

			int vStride = 3 + 3 + 3;
			if (meshType == MeshType::Boned)
				vStride += 6;
			float* vertexOut = new float[(uniqueVertex.size() / uvStride) * vStride];
			for (int i = 0; i < uniqueVertex.size() / uvStride; i++) {
				// Position
				for (int j = 0; j < 3; j++) {
					if (uniqueVertex[i * uvStride] * 3 + j > uPointSize) {
						//bug::out < bug::RED < "Corruption at '" < path < "' : Position Index\n";
						throw AssetError::CorruptedData;
					}
					vertexOut[i * vStride + j] = uPoint[uniqueVertex[i * uvStride] * 3 + j];
				}
				// Normal
				for (int j = 0; j < 3; j++) {
					if (uniqueVertex[i * uvStride + 1] * 3 + j > uNormal.size()) {
						//bug::out < bug::RED < "Corruption at '" < path < "' : Normal Index\n";
						throw AssetError::CorruptedData;
					}
					vertexOut[i * vStride + 3 + j] = uNormal[uniqueVertex[i * uvStride + 1] * 3 + j];
				}
				// UV
				for (int j = 0; j < 3; j++) {
					if (uniqueVertex[i * uvStride + 2] * 3 + j > uTextureSize) {
						//bug::out < bug::RED < "Corruption at '" < path < "' : Color Index\n";
						//bug::out < (uniqueVertex[i * uvStride + 2] * 3 + j) < " > " < uTextureSize < bug::end;
						throw AssetError::CorruptedData;
					}
					else
						vertexOut[i * vStride + 3 + 3 + j] = uTexture[uniqueVertex[i * uvStride + 2] * 3 + j];
				}
				if (meshType == MeshType::Boned) {
					// Bone Index
					for (int j = 0; j < 3; j++) {
						if (uniqueVertex[i * uvStride + 3] * 3 + j > uWeightS) {
							//bug::out < bug::RED < "Corruption at '" < path < "' : Bone Index\n";
							throw AssetError::CorruptedData;
						}
						vertexOut[i * vStride + 3 + 3 + 3 + j] = uWeightI[uniqueVertex[i * uvStride + 3] * 3 + j];
					}
					// Weight
					for (int j = 0; j < 3; j++) {
						if (uniqueVertex[i * uvStride + 3] * 3 + j > uWeightS) {
							//bug::out < bug::RED < "Corruption at '" < path < "' : Weight Index\n";
							throw AssetError::CorruptedData;
						}
						vertexOut[i * vStride + 3 + 3 + 3 + 3 + j] = uWeightF[uniqueVertex[i * uvStride + 3] * 3 + j];
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

			buffer.Init(false, vertexOut, (uniqueVertex.size() / uvStride) * vStride, triangleOut, triangleCount * 3);
			buffer.SetAttrib(0, 3, vStride, 0);// Position
			buffer.SetAttrib(1, 3, vStride, 3);// Normal
			buffer.SetAttrib(2, 3, vStride, 3 + 3);// Color
			if (meshType == MeshType::Boned) {
				buffer.SetAttrib(3, 3, vStride, 3 + 3 + 3);// Bone Index
				buffer.SetAttrib(4, 3, vStride, 3 + 3 + 3 + 3);// Weight
			}

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
			Logging({ "AssetManager","Mesh",toString(err) + ": " + path }, LogStatus::Error);
		}
		file.close();
	}
	void ColliderAsset::Load(const std::string& path)
	{
		FileReader file(path);
		/*if (!file) {
			error = file.error;
			return;
		}*/
		try {

		}
		catch (AssetError err) {
			error = err;
			Logging({ "AssetManager","Collider",toString(err) + ": " + path }, LogStatus::Error);
		}
		file.close();
	}
	void ArmatureAsset::Load(const std::string& path)
	{
		FileReader file(path);
		/*if (!file) {
			error = file.error;
			return;
		}*/
		try {
			uint8_t boneCount;
			file.read(&boneCount);

			//log::out << path << "\n";
			//log::out << boneCount << "\n";
	
			// Acquire and Load Data
			for (int i = 0; i < boneCount; i++) {
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
		file.close();
	}
	void ModelAsset::Load(const std::string& path)
	{
		FileReader file(path);
		/*if (!file) {
			error = file.error;
			Logging({ "AssetManager","Model","Missing: " + path }, LogStatus::Error);
			return;
		}*/
		try {
			std::string root = GetRootPath();

			uint16_t instanceCount;
			file.read(&instanceCount);
			for (int i = 0; i < instanceCount; i++) {
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
			for (int i = 0; i < animationCount; i++) {
				std::string name;
				file.read(&name);
				animations.push_back(GetAsset<AnimationAsset>(root + name));
			}
		}
		catch (AssetError err) {
			error = err;
			Logging({ "AssetManager","Model",toString(err) + ": " + path }, LogStatus::Error);
		}
		file.close();
	}
	std::unordered_map<std::string, MaterialAsset*> engone_materials;
	std::unordered_map<std::string, AnimationAsset*> engone_animations;
	std::unordered_map<std::string, MeshAsset*> engone_meshes;
	std::unordered_map<std::string, ColliderAsset*> engone_colliders;
	std::unordered_map<std::string, ArmatureAsset*> engone_armatures;
	std::unordered_map<std::string, ModelAsset*> engone_models;
	std::unordered_map<std::string, Texture*> engone_textures;
	std::unordered_map<std::string, Font*> engone_fonts;
	std::unordered_map<std::string, Shader*> engone_shaders;
}