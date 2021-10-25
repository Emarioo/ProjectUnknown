#include "gonpch.h"

#include "AssetManager.h"
#include "FileHandler.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "GL/glew.h"

namespace engone {

	void TextureAsset::Load(const std::string& path)
	{
		if (!FindFile("assets/" + path)) {
			error = MissingFile;
			return;
		}
		if (path.length() > 4) {
			stbi_set_flip_vertically_on_load(1);
			buffer = stbi_load(("assets/"+path).c_str(), &width, &height, &BPP, 4);

			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			glBindTexture(GL_TEXTURE_2D, 0);

			if (buffer)
				stbi_image_free(buffer);
		}
	}
	void TextureAsset::Bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void ShaderAsset::Load(const std::string& path)
	{
		std::ifstream file(path);
		if (!file) {
			error = MissingFile;
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
	void ShaderAsset::Init(const std::string& source)
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
	unsigned int ShaderAsset::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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
	unsigned int ShaderAsset::CompileShader(unsigned int type, const std::string& source)
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
	void ShaderAsset::Bind()
	{
		glUseProgram(id);
	}
	void ShaderAsset::SetFloat(const std::string& name, float f)
	{
		glUniform1f(GetUniformLocation(name), f);
	}
	void ShaderAsset::SetVec2(const std::string& name, glm::vec2 v)
	{
		glUniform2f(GetUniformLocation(name), v.x, v.y);
	}
	void ShaderAsset::SetIVec2(const std::string& name, glm::ivec2 v)
	{
		glUniform2i(GetUniformLocation(name), v.x, v.y);
	}
	void ShaderAsset::SetVec3(const std::string& name, glm::vec3 v)
	{
		glUniform3f(GetUniformLocation(name), v.x, v.y, v.z);
	}
	void ShaderAsset::SetIVec3(const std::string& name, glm::ivec3 v)
	{
		glUniform3i(GetUniformLocation(name), v.x, v.y, v.z);
	}
	void ShaderAsset::SetVec4(const std::string& name, float f0, float f1, float f2, float f3)
	{
		glUniform4f(GetUniformLocation(name), f0, f1, f2, f3);
	}
	void ShaderAsset::SetInt(const std::string& name, int v)
	{
		glUniform1i(GetUniformLocation(name), v);
	}
	void ShaderAsset::SetMatrix(const std::string& name, glm::mat4 mat)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
	}
	unsigned int ShaderAsset::GetUniformLocation(const std::string& name)
	{
		if (uniLocations.find(name) != uniLocations.end()) {
			return uniLocations[name];
		}
		unsigned int loc = glGetUniformLocation(id, name.c_str());
		uniLocations[name] = loc;
		return loc;
	}
	void MaterialAsset::Load(const std::string& path)
	{
		FileReader file(path);
		if (!file) {
			error = file.error;
			return;
		}
		try {
			file.read(&diffuse_map);
			std::cout << diffuse_map << "\n";

			file.read<glm::vec3>(&diffuse_color);
			file.read<glm::vec3>(&specular);
			file.read<float>(&shininess);
		}
		catch (AssetError err) {
			error = err;
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
	void Channels::GetValues(int frame, float blend, glm::vec3& pos, glm::vec3& euler, glm::vec3& scale, glm::mat4& quater)
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
					continue;
				}
				else {
					build[curve.first] = a->value;
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

			}
			else {
				build[curve.first] = a->value * (1 - lerp) + b->value * lerp;
			}
		}

		pos.x += build[0] * blend;
		pos.y += build[1] * blend;
		pos.z += build[2] * blend;

		euler.x += build[3] * blend;
		euler.y += build[4] * blend;
		euler.z += build[5] * blend;

		scale.x *= build[6] * blend;
		scale.y *= build[7] * blend;
		scale.z *= build[8] * blend;

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
		if (!file) {
			error = file.error;
			return;
		}
		try {
			
		}
		catch (AssetError err) {
			error = err;
		}
		file.close();
	}
	void MeshAsset::Load(const std::string& path)
	{
		FileReader file(path);
		if (!file) {
			error = file.error;
			return;
		}
		try {
			uint16_t pointCount;
			uint16_t textureCount;
			uint8_t materialCount;
			file.read<MeshType>(&meshType);
			file.read<uint16_t>(&pointCount);
			file.read<uint16_t>(&textureCount);
			file.read<uint8_t>(&materialCount);

			for (int i = 0; i < materialCount; i++) {
				std::string materialName;
				file.read(&materialName);
				std::cout << "Matloc: " << file.path.substr(0, file.path.find_last_of("/")) << "\n";
				materials.push_back(GetAsset<Material>(
					file.path.substr(0, file.path.find_last_of("/")) + materialName
				));
			}



		}
		catch (AssetError err) {
			error = err;
		}
		file.close();
	}
	void ColliderAsset::Load(const std::string& path)
	{
		FileReader file(path);
		if (!file) {
			error = file.error;
			return;
		}
		try {

		}
		catch (AssetError err) {
			error = err;
		}
		file.close();
	}
	void ArmatureAsset::Load(const std::string& path)
	{
		FileReader file(path);
		if (!file) {
			error = file.error;
			return;
		}
		try {

		}
		catch (AssetError err) {
			error = err;
		}
		file.close();
	}
	void ModelAsset::Load(const std::string& path)
	{
		FileReader file(path);
		if (!file) {
			error = file.error;
			return;
		}
		try {

		}
		catch (AssetError err) {
			error = err;
		}
		file.close();
	}
	static std::unordered_map<std::string, MaterialAsset*> materials;
	static std::unordered_map<std::string, AnimationAsset*> animations;
	static std::unordered_map<std::string, MeshAsset*> meshes;
	static std::unordered_map<std::string, ColliderAsset*> colliders;
	static std::unordered_map<std::string, ArmatureAsset*> armatures;
	static std::unordered_map<std::string, ModelAsset*> models;
	static std::unordered_map<std::string, TextureAsset*> textures;
	static std::unordered_map<std::string, FontAsset*> fonts;
	static std::unordered_map<std::string, ShaderAsset*> shaders;
	template <class T>
	AssetError AddAsset(const std::string& path)
	{
		T* asset;
		if (T.type == AssetType::Material) {
			asset = new MaterialAsset(file);
			materials[path] = asset;
		}
		else if(T.type==AssetType::Animation) {
			asset = new AnimationAsset(file);
			animations[path] = asset;
		}
		return asset->error;
	}
	template <class T>
	AssetError AddAsset(const std::string& name, T* asset)
	{
		if (T.type == AssetType::Material) {
			materials[path] = asset;
		}
		else if (T.type == AssetType::Animation) {
			animations[path] = asset;
		}
		return asset->error;
	}
	template <class T>
	T* GetAsset(const std::string& path)
	{
		if (T.type == AssetType::Material) {
			if (materials.count(path) > 0) {
				return materials[path];
			}
		}
		else if (T.type == AssetType::Animation) {
			if (animations.count(path) > 0) {
				return animations[path];
			}
		}
		return nullptr;
	}
	template <class T>
	void RemoveAsset(const std::string& name)
	{
		assets.erase(name);
	}

	static std::unordered_map<std::string, Material> materials;
	void AddMaterialAsset(const std::string& file) {
		if (materials.count(file) == 0) {
			materials[file] = Material();
			LoadMaterial(&materials[file], file);
		}
	}
	Material* GetMaterialAsset(const std::string& name) {
		if (materials.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Material '" < name < "'\n";
		} else if (!materials[name].hasError)
			return &materials[name];
		return nullptr;
	}
	void DeleteMaterialAsset(const std::string& name) {
		materials.erase(name);
	}
	static std::unordered_map<std::string, Mesh> meshes;
	void AddMeshAsset(const std::string& file) {
		if (meshes.count(file) == 0) {
			meshes[file] = Mesh();
			LoadMesh(&meshes[file], file);
		}
	}
	Mesh* GetMeshAsset(const std::string& name) {
		if (meshes.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Mesh '" < name < "'\n";
		} else if (!meshes[name].hasError)
			return &meshes[name];
		return nullptr;
	}
	void DeleteMeshAsset(const std::string& name) {
		meshes.erase(name);
	}
	static std::unordered_map<std::string, Animation> animations;
	void AddAnimationAsset(const std::string& file) {
		if (animations.count(file) == 0) {
			animations[file] = Animation();
			LoadAnimation(&animations[file], file);
		}
	}
	Animation* GetAnimationAsset(const std::string& name) {
		if (animations.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Animation '" < name < "'\n";
		} else
			return &animations[name];
	}
	void DeleteAnimationAsset(const std::string& name) {
		animations.erase(name);
	}
	static std::unordered_map<std::string, Armature> armatures;
	void AddArmatureAsset(const std::string& file) {
		if (armatures.count(file) == 0) {
			armatures[file] = Armature();
			LoadArmature(&armatures[file], file);
		}
	}
	Armature* GetArmatureAsset(const std::string& name) {
		if (armatures.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Armature '" < name < "'\n";
		} else if (!armatures[name].hasError)
			return &armatures[name];
		return nullptr;
	}
	void DeleteArmatureAsset(const std::string& name) {
		armatures.erase(name);
	}
	static std::unordered_map<std::string, Collider> colliders;
	void AddColliderAsset(const std::string& file) {
		if (colliders.count(file) == 0) {
			colliders[file] = Collider();
			LoadCollider(&colliders[file], file);
		}
	}
	Collider* GetColliderAsset(const std::string& name) {
		if (colliders.count(name) == 0) {
			AddColliderAsset(name);
			//bug::out < bug::RED < "Cannot find Collider '" < name < "'\n";
		}
		if (!colliders[name].hasError)
			return &colliders[name];
		return nullptr;
	}
	void DeleteColliderAsset(const std::string& name) {
		colliders.erase(name);
	}
	static std::unordered_map<std::string, Model> models;
	void AddModelAsset(const std::string& file) {
		if (models.count(file) == 0) {
			models[file] = Model();
			LoadModel(&models[file], file);
		}
	}
	Model* GetModelAsset(const std::string& name) {
		if (models.count(name) == 0) {
			AddModelAsset(name);
		}
		if (!models[name].hasError)
			return &models[name];
		return nullptr;
	}
	void DeleteModelAsset(const std::string& name) {
		models.erase(name);
	}
	/*
	std::unordered_map<std::string, Texture> textures;
	void AddTextureAsset(const std::string& name) {// Change this system so it is simular to every other system. FileExist?
		std::string path = "assets/textures/" + name + ".png";
		if (FindFile(path)) {
			textures[name] = Texture(path);
		} else {
			bug::out < bug::RED < "Cannot find Texture '" < path < bug::end;
		}
	}
	Texture* GetTextureAsset(const std::string& name) {
		if (textures.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Texture '" < name < "'\n";
		} else
			return &textures[name];
		return nullptr;
	}
	void DeleteTextureAsset(const std::string& name) {
		textures.erase(name);
	}
	*/
	static std::unordered_map<std::string, Font*> fonts;
	static std::unordered_map<std::string, Texture*> textures;
	static std::unordered_map<std::string, Shader*> shaders;
	Font* GetFont(const std::string& name)
	{
		return fonts[name];
	}
	void AddFont(const std::string& name, Font* font)
	{
		fonts[name] = font;
	}
	Shader* GetShader(const std::string& name)
	{
		return shaders[name];
	}
	void AddShader(const std::string& name, Shader* shader)
	{
		shaders[name] = shader;
	}
	Texture* GetTexture(const std::string& name)
	{
		return textures[name];
	}
	void AddTexture(const std::string& name, Texture* texture)
	{
		textures[name] = texture;
	}
}