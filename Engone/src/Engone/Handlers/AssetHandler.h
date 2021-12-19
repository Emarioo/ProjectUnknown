#pragma once

#include "../Handlers/FileHandler.h"
#include "../Rendering/Buffer.h"

#include "Engone/Utility/Utilities.h"

namespace engone {
	enum AssetError
	{
		None = 0,
		MissingFile,
		MissingData,
		BadSyntax,
		CorruptedData
	};
	std::string toString(AssetError t);

	enum class AssetType : char
	{
		None = 0,
		Texture,
		Font,
		Shader,
		Material,
		Mesh,
		Animation,
		Armature,
		Model,
		Collider
	};
	std::string toString(AssetError t);
	namespace log
	{
		logger operator<<(logger log, AssetType type);
	}
	
	class FileReader
	{
	public:
		//FileReader() = default;
		FileReader(const std::string& path, bool binaryForm = false) : binaryForm(binaryForm), path(path)
		{
			file.open(path,std::ios::binary);
			if (file) {
				file.seekg(0, file.end);
				fileSize = file.tellg();
				file.seekg(file.beg);
			}
			else {
				//std::cout << " how strangee" <<"not found " << path << "\n";
				error = MissingFile;
				close();
			}
		}

		bool binaryForm = false;
		int readHead = 0;
		size_t fileSize = 0;
		std::ifstream file;
		std::string path;
		AssetError error;

		bool operator!()
		{
			return error != None;
		}
		operator bool()
		{
			return error == None;
		}

		void close()
		{
			if(file.is_open())
				file.close();
		}
		/*
		T has to be convertable to int or float. Can also be char short and most of them. Maybe not long or double
		*/
		template <typename T>
		void read(T* var, uint32_t size = 1)
		{
			if (error != None)
				return;
			if (binaryForm) {
				if (readHead - 1 + size * sizeof(T) > fileSize) {
					error = MissingData;
					throw MissingData;
				}

				file.read(reinterpret_cast<char*>(var), size*sizeof(T));
				readHead += size * sizeof(T);
				//std::cout << "read "<<size << " "<<sizeof(T)<<" "<< readHead<<" "<< "\n";
			}
			else {
				try {
					std::string line;
					int index = 0;

					while (index < size) {
						std::getline(file, line);
						if (line[0] == '#')
							continue;
						if (line.back() == '\r')
							line.erase(line.end() - 1);

						std::vector<std::string> numbers = SplitString(line, " ");

						for (int i = 0; i < numbers.size(); i++) {
							//std::cout << "num"<<numbers[i] << "\n";
							if (numbers[i].length() == 0)
								continue;
							if (numbers[i].find('.') == -1) {
								var[index++] = static_cast<T>(std::stoi(numbers[i]));
							}
							else {
								float f = std::stof(numbers[i]);
								
								var[index++] = static_cast<T>(f);
							}
						}
					}
				}catch(std::invalid_argument e){
					throw CorruptedData;
				}
			}
		}
		void read(glm::vec3* var, uint32_t size = 1)
		{
			if (error != None)
				return;
			if (binaryForm) {
				if (readHead - 1 + size * sizeof(glm::vec3) > fileSize) {
					error = MissingData;
					throw MissingData;
				}

				file.read(reinterpret_cast<char*>(var), size * sizeof(glm::vec3));
				readHead += size * sizeof(glm::vec3);
				//std::cout << "read "<<size << " "<<sizeof(T)<<" "<< readHead<<" "<< "\n";
			}
			else {
				try {
					std::string line;
					int index = 0;

					while (index < size*3) {
						std::getline(file, line);
						if (line[0] == '#')
							continue;
						if (line.back() == '\r')
							line.erase(line.end() - 1);

						std::vector<std::string> numbers = SplitString(line, " ");

						for (int i = 0; i < numbers.size(); i++) {
							//std::cout << "num"<<numbers[i] << "\n";
							if (numbers[i].length() == 0)
								continue;
							if (numbers[i].find('.') == -1) {
								//var[index/3][index%3] = std::stoi(numbers[i]);
							}
							else {
								/*
								glm::vec3& temp = var[index / 3];
								temp[index%3] = std::stof(numbers[i]);
								std::cout << temp[index % 3] << "\n";
								*/
								var[index / 3][index%3]= std::stof(numbers[i]);
							}
							index++;
						}
					}
				}
				catch (std::invalid_argument e) {
					throw CorruptedData;
				}
			}
		}
		void read(glm::mat4* var, uint32_t size = 1)
		{
			if (error != None)
				return;
			if (binaryForm) {
				if (readHead - 1 + size * sizeof(glm::mat4) > fileSize) {
					error = MissingData;
					throw MissingData;
				}

				file.read(reinterpret_cast<char*>(var), size * sizeof(glm::mat4));
				readHead += size * sizeof(glm::mat4);
				//std::cout << "read "<<size << " "<<sizeof(T)<<" "<< readHead<<" "<< "\n";
			}
			else {
				try {
					std::string line;
					int index = 0;

					while (index < size * 16) {
						std::getline(file, line);
						if (line[0] == '#')
							continue;
						if (line.back() == '\r')
							line.erase(line.end() - 1);

						std::vector<std::string> numbers = SplitString(line, " ");

						for (int i = 0; i < numbers.size(); i++) {
							//std::cout << "num"<<numbers[i] << "\n";
							if (numbers[i].length() == 0)
								continue;
							if (numbers[i].find('.') == -1) {
								//var[index/3][index%3] = std::stoi(numbers[i]);
							}
							else {
								/*
								glm::vec3& temp = var[index / 3];
								temp[index%3] = std::stof(numbers[i]);
								std::cout << temp[index % 3] << "\n";
								*/
								var[index / 16][(index/4) % 4][index%4] = std::stof(numbers[i]);
							}
							index++;
						}
					}
				}
				catch (std::invalid_argument e) {
					throw CorruptedData;
				}
			}
		}
		/*
		256 characters is the current max size.
		*/
		void read(std::string* var)
		{
			if (error == MissingFile)
				return;
			if (binaryForm) {
				if (readHead - 1 + 1 > fileSize) {
					error = MissingData;
					throw MissingData;
				}
				uint8_t length;
				file.read(reinterpret_cast<char*>(&length), 1);
				readHead++;

				if (length == 0)
					return;
				char* tempString = new char[length+1];
				tempString[length] = '\0';

				if (readHead - 1 + length > fileSize) {
					error = MissingData;
					throw MissingData;
				}
				file.read(reinterpret_cast<char*>(tempString), length);
				readHead += length;

				*var = tempString;

				//std::cout << *var << "\n";

				delete[] tempString;
			}
			else {
				std::string line;
				int index = 0;

				while (index < 1) {
					std::getline(file, line);
					if (line[0] == '#')
						continue;
					if (line.back() == '\r')
						line.erase(line.end() - 1);
					
					*var = line;
					index++;
				}
			}
		}
	};

	class Asset
	{
	public:
		static const AssetType TYPE = AssetType::None; // General type
		AssetType type = AssetType::None; // my type
		Asset() = default;
		Asset(const std::string& path) : filePath("assets/" + path) { }

		std::string baseName;
		std::string filePath;
		std::string GetRootPath(){
			int first = filePath.find_first_of("/");
			int last = filePath.find_last_of("/");

			if (first == last) {// directly in assets
				return "";
			}
			else {
				return filePath.substr(7, last-6);
			}
		}
		void SetBaseName(const std::string& name)
		{
			int last = name.find_last_of("/");
			if(last!=-1)
				baseName = name.substr(last+1);
			else
				baseName = name;
		}

		AssetError error=None;

		bool operator!()
		{
			return error != None;
		}
		operator bool()
		{
			return error == None;
		}

		template <class T>
		T* cast() { return reinterpret_cast<T*>(this); }

		virtual void Load(const std::string& path) { std::cout << "virtual func of asset\n"; };
	};
	class Texture : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Texture;
		Texture() { type == AssetType::Texture; };
		Texture(const std::string& path) : Asset(path + ".png") { type = AssetType::Texture; Load(filePath); };
		virtual void Load(const std::string& path) override;
		void Init(int w, int h, void* data);

		void SubData(int x, int y, int w, int h, void* data);
		void Bind(unsigned int slot=0);

		int GetWidth();
		int GetHeight();
	
	private:
		unsigned int id;
		unsigned char* buffer = nullptr;
		int width = 0, height, BPP = 0;
	};
	class Shader: public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Shader;
		Shader() { type = AssetType::Shader; }
		Shader(const std::string& path) : Asset(path + ".glsl") { Load(filePath); type = AssetType::Shader; }
		Shader(const std::string& source, bool isSource) { Init(source); type = AssetType::Shader; }
		virtual void Load(const std::string& path) override;
		void Init(const std::string& source);

		void Bind();
		void SetInt(const std::string& name, int i);
		void SetFloat(const std::string& name, float f);
		void SetVec2(const std::string& name, glm::vec2);
		void SetIVec2(const std::string& name, glm::ivec2);
		void SetVec3(const std::string& name, glm::vec3);
		void SetIVec3(const std::string& name, glm::ivec3);
		void SetVec4(const std::string& name, float f0, float f1, float f2, float f3);
		void SetMatrix(const std::string& name, glm::mat4 v);

	private:
		unsigned int id;
		unsigned int CreateShader(const std::string& vert, const std::string& frag);
		unsigned int CompileShader(const unsigned int, const std::string& src);

		unsigned int GetUniformLocation(const std::string& name);
		std::unordered_map<std::string, unsigned int> uniLocations;
		int section[3]{ 1,1,1 };
	};
	class Font: public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Font;
		Font() { type = AssetType::Font; };
		Font(const std::string& path) : Asset(path) { type = AssetType::Font; Load(filePath); };
		virtual void Load(const std::string& path) override;

		Texture texture;
		int charWid[256];

		int imgSize = 1024;
		int charSize = imgSize / 16;

	};
	class MaterialAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Material;

		MaterialAsset() { type = AssetType::Material; };
		MaterialAsset(const std::string& path) : Asset(path + ".material") { type = AssetType::Material; Load(filePath); };
		virtual void Load(const std::string& path) override;

		Texture* diffuse_map=nullptr;
		glm::vec3 diffuse_color = { 1,1,1 };
		glm::vec3 specular = { .5f,.5,.5f };
		float shininess = .5;

	};
	enum PolationType : unsigned char
	{
		Constant, Linear, Bezier
	};
	class Keyframe
	{
	public:
		PolationType polation;
		unsigned short frame;
		float value;
		/*
		"polation" is the type of polation between keyframes.
		*/
		Keyframe(PolationType polation, unsigned short frame, float value);
	};
	class FCurve
	{
	public:
		FCurve();
		Keyframe& Get(int index);
		void Add(Keyframe keyframe);

		std::vector<Keyframe> frames;
	};
	enum ChannelType : unsigned char
	{
		PosX = 0, PosY, PosZ,
		RotX, RotY, RotZ, // Not used. Deprecated
		ScaX, ScaY, ScaZ,
		QuaX, QuaY, QuaZ, QuaW
	};
	class Channels
	{
	public:
		Channels() {}
		FCurve& Get(ChannelType channel);
		/*
		"channel" is what type of value the keyframes will be changing. Example PosX.
		Create a "FCurve" object and see the "Add" function for more details.
		*/
		void Add(ChannelType channel, FCurve fcurve);
		/*
		Add values to the references given in the argument
		*/
		void GetValues(int frame, float blend, glm::vec3& pos, glm::vec3& euler, glm::vec3& scale, glm::mat4& quater, short* usedChannel);
		std::unordered_map<ChannelType, FCurve> fcurves;
	};
	class AnimationAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Animation;
		AnimationAsset() { type = AssetType::Animation; };
		AnimationAsset(const std::string& path) : Asset(path+".animation") { type = AssetType::Animation; Load(filePath); };
		virtual void Load(const std::string& path) override;

		Channels& Get(unsigned short i);
		/*
		objectIndex is the index of the bone. Also known as the vertex group in blender.
		Create a "Channels" object and see the "Add" function for more details.
		*/
		void AddObjectChannels(int objectIndex, Channels channels);
		/*
		Use this to create an animation by code.
		See the "AddObjectChannels" function for more details.
		*/
		void Modify(unsigned short startFrame, unsigned short endFrame);
		/*
		Use this to create an animation by code.
		"speed" is 24 by default. Speed can also be changed in the animator.
		See the "AddObjectChannels" function for more details.
		*/
		void Modify(unsigned short startFrame, unsigned short endFrame, float speed);
		/*
		
		*/
		std::unordered_map<unsigned short, Channels> objects;

		uint16_t frameStart = 0;
		uint16_t frameEnd = 0;
		float defaultSpeed = 24;// frames per second. 24 is default from blender.
	};
	class MeshAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Mesh;
		MeshAsset() { type = AssetType::Mesh; };
		MeshAsset(const std::string& path) : Asset(path+".mesh") { type = AssetType::Mesh; Load(filePath); };
		virtual void Load(const std::string& path) override;

		enum class MeshType : char
		{
			Normal=0,
			Boned
		};

		MeshType meshType = MeshType::Normal;
		std::vector<MaterialAsset*> materials;
		std::vector<AnimationAsset*> animations;
		TriangleBuffer buffer;
	};
	class ColliderAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Collider;
		ColliderAsset() { type = AssetType::Collider; };
		ColliderAsset(const std::string& path) : Asset(path+".collider") { type = AssetType::Collider; Load(filePath); };
		virtual void Load(const std::string& path) override;

		std::vector<AnimationAsset*> animations;
		float furthestPoint = 0;
		std::vector<std::uint16_t> quad; // 4 = quad
		std::vector<std::uint16_t> tri; // 3 = tri - not supported at the moment
		std::vector<glm::vec3> points;
	};
	class Bone
	{
	public:
		uint16_t parent = 0;

		glm::mat4 localMat = glm::mat4(1);
		glm::mat4 invModel = glm::mat4(1);

		Bone() = default;

	};
	class ArmatureAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Armature;
		ArmatureAsset() { type = AssetType::Armature; };
		ArmatureAsset(const std::string& path) : Asset(path + ".armature") { type = AssetType::Armature; Load(filePath); };
		virtual void Load(const std::string& path) override;

		std::vector<Bone> bones;

	};
	class AssetInstance
	{
	public:
		AssetInstance() = default;

		std::string name;
		int16_t parent = -1;

		glm::mat4 localMat = glm::mat4(1);
		glm::mat4 invModel = glm::mat4(1);

		Asset* asset;
	};
	class ModelAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Model;
		ModelAsset() { type = AssetType::Model; };
		ModelAsset(const std::string& path) : Asset(path+".model") { type = AssetType::Model; Load(filePath); };
		virtual void Load(const std::string& path) override;
		
		/*
		List of mesh, collider and armature transforms.
		transforms[meshes.size()+colliders.size()+1] to get the second armature transform
		*/
		//std::vector<glm::mat4> transforms;
		std::vector<AssetInstance> instances;
		std::vector<AnimationAsset*> animations;

		/*
		// Instances of the assets
		std::vector<MeshAsset*> meshes;
		std::vector<ColliderAsset*> colliders;
		std::vector<ArmatureAsset*> armatures;
		*/

	};
	extern std::unordered_map<std::string, MaterialAsset*> engone_materials;
	extern std::unordered_map<std::string, AnimationAsset*> engone_animations;
	extern std::unordered_map<std::string, MeshAsset*> engone_meshes;
	extern std::unordered_map<std::string, ColliderAsset*> engone_colliders;
	extern std::unordered_map<std::string, ArmatureAsset*> engone_armatures;
	extern std::unordered_map<std::string, ModelAsset*> engone_models;
	extern std::unordered_map<std::string, Texture*> engone_textures;
	extern std::unordered_map<std::string, Font*> engone_fonts;
	extern std::unordered_map<std::string, Shader*> engone_shaders;
	template <class T>
	T* AddAsset(const std::string& name, T* asset)
	{
		if (asset->error!=None) {
			log::out << log::RED << log::TIME << toString(asset->error) << ": " << asset->filePath << "\n" << log::SILVER;
		}
		//std::cout << name <<" "<<(int)T::type<< "\n";
		if (T::TYPE == AssetType::Texture)
			engone_textures[name] = asset->cast<Texture>();
		else if (T::TYPE == AssetType::Font)
			engone_fonts[name] = asset->cast<Font>();
		else if (T::TYPE == AssetType::Shader)
			engone_shaders[name] = asset->cast<Shader>();
		else if (T::TYPE == AssetType::Material) {
			engone_materials[name] = asset->cast<MaterialAsset>();
			//std::cout << "added\n";
		}
		else if (T::TYPE == AssetType::Animation)
			engone_animations[name] = asset->cast<AnimationAsset>();
		else if (T::TYPE == AssetType::Mesh)
			engone_meshes[name] = asset->cast<MeshAsset>();
		else if (T::TYPE == AssetType::Collider)
			engone_colliders[name] = asset->cast<ColliderAsset>();
		else if (T::TYPE == AssetType::Armature)
			engone_armatures[name] = asset->cast<ArmatureAsset>();
		else if (T::TYPE == AssetType::Model)
			engone_models[name] = asset->cast<ModelAsset>();
		return asset;
	}
	/*
	@path is based in the assets directory
	*/
	template <class T>
	T* AddAsset(const std::string& name,const std::string& path)
	{
		T* t = new T(path);
		t->SetBaseName(name);
		return AddAsset<T>(name, t);
	}
	/*
	@path is based in the assets directory
	*/
	template <class T>
	T* AddAsset(const std::string& path)
	{
		return AddAsset<T>(path, path);
	}
	/*
	@return can be nullptr if the asset is missing. Remember to handle it.
	*/
	template <class T>
	T* GetAsset(const std::string& name)
	{
		//std::cout << name << " " << (int)T::type<<" "<<(int)AssetType::Shader << "\n";
		if (T::TYPE == AssetType::Texture) {
			if (engone_textures.count(name) > 0)
				return engone_textures[name]->cast<T>();
			else {
				return AddAsset<Texture>(name)->cast<T>();
			}
		}
		else if (T::TYPE == AssetType::Font) {
			if (engone_fonts.count(name) > 0)
				return engone_fonts[name]->cast<T>();
			else {
				return AddAsset<Font>(name)->cast<T>();
			}
		}
		else if (T::TYPE == AssetType::Shader) {
			if (engone_shaders.count(name) > 0)
				return engone_shaders[name]->cast<T>();
			else {
				return AddAsset<Shader>(name)->cast<T>();
			}
		}
		else if (T::TYPE == AssetType::Material) {
			if (engone_materials.count(name) > 0)
				return engone_materials[name]->cast<T>();
			else {
				return AddAsset<MaterialAsset>(name)->cast<T>();
			}
		}
		else if (T::TYPE == AssetType::Animation) {
			if (engone_animations.count(name) > 0)
				return engone_animations[name]->cast<T>();
			else {
				return AddAsset<AnimationAsset>(name)->cast<T>();
			}
		}
		else if (T::TYPE == AssetType::Mesh) {
			if (engone_meshes.count(name) > 0)
				return engone_meshes[name]->cast<T>();
			else {
				return AddAsset<MeshAsset>(name)->cast<T>();
			}
		}
		else if (T::TYPE == AssetType::Collider) {
			if (engone_colliders.count(name) > 0)
				return engone_colliders[name]->cast<T>();
			else {
				return AddAsset<ColliderAsset>(name)->cast<T>();
			}
		}
		else if (T::TYPE == AssetType::Armature) {
			if (engone_armatures.count(name) > 0)
				return engone_armatures[name]->cast<T>();
			else {
				return AddAsset<ArmatureAsset>(name)->cast<T>();
			}
		}
		else if (T::TYPE == AssetType::Model) {
			if (engone_models.count(name) > 0)
				return engone_models[name]->cast<T>();
			else {
				return AddAsset<ModelAsset>(name)->cast<T>();
			}
		}
		return nullptr;
	}
	template <class T>
	void RemoveAsset(const std::string& name)
	{
		if (T::TYPE == AssetType::Texture)
			if (engone_textures.count(name) > 0) { // needs to be copied to the other ones
				delete engone_textures[name];
				engone_textures.erase(name);
			}
		else if (T::TYPE == AssetType::Font)
			engone_fonts.erase(name);
		else if (T::TYPE == AssetType::Shader)
			engone_shaders.erase(name);
		else if (T::TYPE == AssetType::Material)
			engone_materials.erase(name);
		else if (T::TYPE == AssetType::Animation)
			engone_animations.erase(name);
		else if (T::TYPE == AssetType::Mesh)
			engone_meshes.erase(name);
		else if (T::TYPE == AssetType::Collider)
			engone_colliders.erase(name);
		else if (T::TYPE == AssetType::Armature)
			engone_armatures.erase(name);
		else if (T::TYPE == AssetType::Model)
			engone_models.erase(name);
		
	}
}