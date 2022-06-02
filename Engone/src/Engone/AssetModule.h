#pragma once

#include "Engone/Rendering/Buffer.h"
#include "Engone/Utility/Utilities.h"
#include "Engone/Logger.h"

namespace engone {

	// used in ModelAsset, not great but it works
	class Animator;

	enum AssetError {
		None = 0,
		MissingFile,
		CorruptedData
	};
	std::string toString(AssetError t);

#define ASSET_TYPES 9
	enum class AssetType : char {
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
	namespace log {
		logger operator<<(logger log, AssetType type);
		logger operator<<(logger log, AssetError err);
	}
	
	class FileWriter{
	public:
		FileWriter(const std::string& path, bool binaryForm = false) : binaryForm(binaryForm), path(path) {
			file.open(path,std::ios::binary);
			if (file) {

			} else {
				error = MissingFile;
				close();
			}
		}

		bool binaryForm = false;
		int writeHead = 0;
		std::ofstream file;
		std::string path;
		AssetError error=AssetError::None;

		std::vector<std::string> readNumbers;

		bool operator!() {
			return error != None;
		}
		operator bool() {
			return error == None;
		}
		void close() {
			if (file.is_open())
				file.close();
		}
		/*
		T has to be convertable to a string std::to_string(T)
		*/
		template <typename T>
		void write(T* var, uint32_t size = 1) {
			if (error != None || var==nullptr || size==0)
				return;
			if (binaryForm) {
				file.write(reinterpret_cast<char*>(var), size * (size_t)sizeof(T));
				writeHead += size * sizeof(T);
			} else {
				std::string out;
				for (uint32_t i = 0; i < size;i++) {
					if(i==0)
						out += std::to_string(var[i]);
					else
						out += " "+ std::to_string(var[i]);
				}
				file.write(out.c_str(),out.length());
				file.write("\n", 1);
			}
		}
		void write(glm::vec3* var, uint32_t size = 1) {
			write((float*)var, size * 3);
		}
		void write(glm::mat4* var, uint32_t size = 1) {
			write((float*)var, size * 16);
		}
		/*
		256 characters is the current max size.
		*/
		void write(std::string* var) {
			if (error == MissingFile)
				return;
			if (binaryForm) {
				file.write(var->c_str(),var->length());
				writeHead += var->length();
			} else {
				file.write(var->c_str(),var->length());
				file.write("\n", 1);
			}
		}
		void writeComment(const std::string& str) {
			if (error == MissingFile)
				return;
			if (!binaryForm) {
				file.write("# ", 2);
				file.write(str.c_str(), str.length());
				file.write("\n",1);
			}
		}
	};
	class FileReader {
	public:
		//FileReader() = default;
		FileReader(const std::string& path, bool binaryForm = false) : binaryForm(binaryForm), path(path) {
			file.open(path, std::ios::binary);
			if (file) {
				file.seekg(0, file.end);
				fileSize = (std::size_t)file.tellg();
				file.seekg(file.beg);
			} else {
				//std::cout << " how strangee" <<"not found " << path << "\n";
				error = MissingFile;
				close();
			}
		}
		~FileReader() {
			close();
		}

		bool binaryForm = false;
		uint32_t readHead = 0;
		uint32_t fileSize = 0;
		std::ifstream file;
		std::string path;
		AssetError error=AssetError::None;

		std::vector<std::string> readNumbers;

		bool operator!() {
			return error != None;
		}
		operator bool() {
			return error == None;
		}

		void close() {
			if (file.is_open())
				file.close();
		}
		/*
		T has to be convertable to int or float. Can also be char short and most of them. Maybe not long or double
		*/
		template <typename T>
		void read(T* var, uint32_t size = 1) {
			if (error != None || var == nullptr || size==0)
				return;
			if (binaryForm) {
				if (readHead - 1 + size * sizeof(T) > fileSize) {
					error = CorruptedData;
					throw CorruptedData;
				}

				file.read(reinterpret_cast<char*>(var), size * (size_t)sizeof(T));
				readHead += size * sizeof(T);
			} else {
				try {
					std::string line;
					uint32_t index = 0;

					while (index < size) {
						if (readNumbers.size() == 0) {

							std::getline(file, line);

							if (file.eof())
								throw CorruptedData;
							if (line.empty())
								continue;
							if (line[0] == '#')
								continue;
							if (line.back() == '\r')
								line.erase(line.end() - 1);

							readNumbers = SplitString(line, " ");
						}
						uint32_t toRead = readNumbers.size();
						if (size < readNumbers.size())
							toRead = size;

						for (uint32_t i = 0; i < toRead; i++) {
							std::string& str = readNumbers[0];
							if (str.length() == 0) {
								readNumbers.erase(readNumbers.begin());
								continue;
							}
							if (str.find('.') == -1) {
								try {
									var[index] = static_cast<T>(std::stoi(str));
								} catch (std::out_of_range e) {
									var[index] = static_cast<T>(std::stoull(str));
								}
								index++;
							} else {
								var[index++] = static_cast<T>(std::stof(str));
							}
							readNumbers.erase(readNumbers.begin());
						}
					}
				} catch (std::invalid_argument e) {
					throw CorruptedData;
				}
			}
		}
		void read(glm::vec3* var, uint32_t size = 1) {
			read((float*)var, size * 3);
		}
		void read(glm::mat4* var, uint32_t size = 1) {
			read((float*)var, size * 16);
		}
		/*
		256 characters is the current max size.
		*/
		void read(std::string* var) {
			if (error != None || var == nullptr)
				return;
			if (binaryForm) {
				if (readHead - 1u + 1u > fileSize) {
					error = CorruptedData;
					throw CorruptedData;
				}
				uint8_t length;
				file.read(reinterpret_cast<char*>(&length), 1);
				readHead++;

				if (length == 0u)
					return;

				if (readHead - 1u + length > fileSize) {
					error = CorruptedData;
					throw CorruptedData;
				}

				*var = std::string(length, ' ');

				file.read(reinterpret_cast<char*>(var->data()), length);
				readHead += length;
			} else {
				std::string line;
				uint32_t index = 0;

				while (index < 1u) {
					if (file.eof()) {
						throw CorruptedData;
					}
					std::getline(file, line);

					//std::cout << line[0] << "\n";
					if (line[0] == '#') {
						continue;
					}
					if (!line.empty()) {
						if (line.back() == '\r')
							line.erase(line.end() - 1);
					}

					*var = line;
					index++;
				}
			}
		}
		void readAll(std::string* var) {
			if (error != None || var == nullptr)
				return;
			if (binaryForm) {
				log::out <<log::RED <<"readAll with binary is not implemented" << "\n";
				throw CorruptedData;
				//if (readHead - 1 + 1 > fileSize) {
				//	error = CorruptedData;
				//	throw CorruptedData;
				//}
				//uint8_t length;
				//file.read(reinterpret_cast<char*>(&length), 1);
				//readHead++;

				//if (length == 0)
				//	return;

				//if (readHead - 1 + length > fileSize) {
				//	error = CorruptedData;
				//	throw CorruptedData;
				//}

				//*var = std::string(length, ' ');

				//file.read(reinterpret_cast<char*>(var->data()), length);
				//readHead += length;
			} else {
				std::string line;
				while (true) {
					if (file.eof()) {
						break;
					}
					std::getline(file, line);

					if (line[0] == '#') {
						continue;
					}
					if (!line.empty()) {
						if (line.back() == '\r')
							line.erase(line.end() - 1);
					}
					*var += line+"\n";
				}
			}
		}
		std::vector<std::string> readLines() {
			if (error != None)
				return std::vector<std::string>();
			if (binaryForm) {
				log::out << log::RED << "readLines with binary is not implemented" << "\n";
				throw CorruptedData;
				//if (readHead - 1 + 1 > fileSize) {
				//	error = CorruptedData;
				//	throw CorruptedData;
				//}
				//uint8_t length;
				//file.read(reinterpret_cast<char*>(&length), 1);
				//readHead++;

				//if (length == 0)
				//	return;

				//if (readHead - 1 + length > fileSize) {
				//	error = CorruptedData;
				//	throw CorruptedData;
				//}

				//*var = std::string(length, ' ');

				//file.read(reinterpret_cast<char*>(var->data()), length);
				//readHead += length;
			} else {
				std::vector<std::string> list;
				std::string line;
				while (true) {
					if (file.eof()) {
						break;
					}
					std::getline(file, line);

					if (line[0] == '#') {
						continue;
					}
					if (!line.empty()) {
						if (line.back() == '\r')
							line.erase(line.end() - 1);
					}
					list.push_back(line + "\n");
				}
				return list;
			}
		}

	};

	class Asset
	{
	public:
		static const AssetType TYPE = AssetType::None; // General type
		AssetType type = AssetType::None; // my type
		Asset() = default;
		Asset(AssetType type) : type(type) {};
		Asset(AssetType type,const std::string& path) : type(type), filePath("assets/" + path) { }

		std::string baseName;
		std::string filePath;
		std::string getRootPath(){
			size_t first = filePath.find_first_of("/");
			size_t last = filePath.find_last_of("/");

			if (first == last) {// directly in assets
				return "";
			}
			else {
				return filePath.substr(7, last-6);
			}
		}
		void setBaseName(const std::string& name)
		{
			size_t last = name.find_last_of("/");
			if(last!=-1)
				baseName = name.substr(last+1);
			else
				baseName = name;
		}

		AssetError error=None;

		bool operator!() {
			return error != None;
		}
		operator bool() {
			return error == None;
		}

		template <class T>
		T* cast() { return reinterpret_cast<T*>(this); }

		virtual void load(const std::string& path) { std::cout << "virtual func of asset\n"; };
	};
	class Texture : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Texture;
		Texture() : Asset(TYPE)  { };
		Texture(const std::string& path) : Asset(TYPE,path + ".png") { load(filePath); };
		void load(const std::string& path) override;
		void init(int w, int h, void* data);

		void subData(int x, int y, int w, int h, void* data);
		void bind(unsigned int slot=0);

		int getWidth();
		int getHeight();
	
	private:
		unsigned int id=0;
		unsigned char* buffer = nullptr;
		int width = 0, height=0, BPP = 0;
	};
	class Shader: public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Shader;
		Shader() : Asset(TYPE) { }
		Shader(const char* source) { init(source); }
		Shader(const std::string& path) : Asset(TYPE,path + ".glsl") { load(filePath); }
		
		void load(const std::string& path) override;
		void init(const std::string& source);

		std::string vs,fs;

		void bind();
		void setInt(const std::string& name, int i);
		void setFloat(const std::string& name, float f);
		void setVec2(const std::string& name, glm::vec2);
		void setIVec2(const std::string& name, glm::ivec2);
		void setVec3(const std::string& name, glm::vec3);
		void setIVec3(const std::string& name, glm::ivec3);
		void setVec4(const std::string& name, float f0, float f1, float f2, float f3);
		void setMat4(const std::string& name, glm::mat4 v);

	private:
		unsigned int id=0;
		unsigned int createShader(const std::string& vert, const std::string& frag);
		unsigned int compileShader(const unsigned int, const std::string& src);

		unsigned int getUniformLocation(const std::string& name);
		std::unordered_map<std::string, unsigned int> uniLocations;
		int section[3]{ 1,1,1 };
	};
	class Font: public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Font;
		Font() : Asset(TYPE) {  };
		Font(const std::string& path) : Asset(TYPE,path) { load(filePath); };
		void load(const std::string& path) override;

		float getWidth(const std::string& str, float height);

		Texture texture;
		int charWid[256];

		int imgSize = 1024;
		int charSize = imgSize / 16;

	};
	class MaterialAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Material;

		MaterialAsset() : Asset(TYPE) { };
		MaterialAsset(const std::string& path) : Asset(TYPE,path + ".material") { load(filePath); };
		void load(const std::string& path) override;

		Texture* diffuse_map=nullptr;
		glm::vec3 diffuse_color = { 1,1,1 };
		glm::vec3 specular = { .5f,.5,.5f };
		float shininess = .5;

		void bind(Shader* shader, int index);

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
		Keyframe& get(int index);
		void add(Keyframe keyframe);

		std::vector<Keyframe> frames;
	};
	enum ChannelType : unsigned char
	{
		PosX = 0, PosY, PosZ,
		RotX, RotY, RotZ, // Not used. Deprecated
		ScaX, ScaY, ScaZ,
		QuaX, QuaY, QuaZ, QuaW,
	};
	class Channels
	{
	public:
		Channels() {}
		FCurve& get(ChannelType channel);
		/*
		"channel" is what type of value the keyframes will be changing. Example PosX.
		Create a "FCurve" object and see the "Add" function for more details.
		*/
		void add(ChannelType channel, FCurve fcurve);
		/*
		Add values to the references given in the argument
		*/
		void getValues(int frame, float blend, glm::vec3& pos, glm::vec3& euler, glm::vec3& scale, glm::mat4& quater, short* usedChannel);
		std::unordered_map<ChannelType, FCurve> fcurves;
	};
	class AnimationAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Animation;
		AnimationAsset() : Asset(TYPE) {  };
		AnimationAsset(const std::string& path) : Asset(TYPE,path+".animation") { load(filePath); };
		void load(const std::string& path) override;

		Channels& get(unsigned short i);
		/*
		objectIndex is the index of the bone. Also known as the vertex group in blender.
		Create a "Channels" object and see the "Add" function for more details.
		*/
		void addObjectChannels(int objectIndex, Channels channels);
		/*
		Use this to create an animation by code.
		See the "AddObjectChannels" function for more details.
		*/
		void modify(unsigned short startFrame, unsigned short endFrame);
		/*
		Use this to create an animation by code.
		"speed" is 24 by default. Speed can also be changed in the animator.
		See the "AddObjectChannels" function for more details.
		*/

		void modify(unsigned short startFrame, unsigned short endFrame, float speed);

		std::unordered_map<unsigned short, Channels> objects;

		uint16_t frameStart = 0;
		uint16_t frameEnd = 0;
		float defaultSpeed = 24;// frames per second. 24 is default from blender.
	};
	class MeshAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Mesh;
		MeshAsset() : Asset(TYPE) {  };
		MeshAsset(const std::string& path) : Asset(TYPE,path+".mesh") { load(filePath); };
		void load(const std::string& path) override;

		enum class MeshType : char
		{
			Normal=0,
			Boned
		};

		MeshType meshType = MeshType::Normal;
		static const int maxMaterials=4;
		std::vector<MaterialAsset*> materials;
		VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;
		VertexArray vertexArray;
		//TriangleBuffer buffer;
	};
	class ColliderAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetType::Collider;
		ColliderAsset() : Asset(TYPE) {  };
		ColliderAsset(const std::string& path) : Asset(TYPE,path+".collider") { load(filePath); };
		void load(const std::string& path) override;

		enum class Type : char {
			Sphere,
			Cube,
			Mesh
		};

		//-- Mesh
		float furthest=0;
		std::vector<glm::vec3> points;
		std::vector<std::uint16_t> tris;

		Type colliderType = Type::Sphere;
		union {
			struct {// cube
				glm::vec3 position = { 0,0,0 }, scale = { 0,0,0 }, rotation = { 0,0,0 };
			} cube{};
			struct {// sphere
				glm::vec3 position = { 0,0,0 };
				float radius=0;
			} sphere;
		};
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
		ArmatureAsset() : Asset(TYPE) { };
		ArmatureAsset(const std::string& path) : Asset(TYPE,path + ".armature") { load(filePath); };
		void load(const std::string& path) override;

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
	class ModelAsset : public Asset {
	public:
		static const AssetType TYPE = AssetType::Model;
		ModelAsset() : Asset(TYPE) {  };
		ModelAsset(const std::string& path) : Asset(TYPE, path + ".model") { load(filePath); };
		void load(const std::string& path) override;

		/*
		List of mesh, collider and armature transforms.
		transforms[meshes.size()+colliders.size()+1] to get the second armature transform
		*/
		//std::vector<glm::mat4> transforms;
		std::vector<AssetInstance> instances;
		std::vector<AnimationAsset*> animations;
		
		// Will give a list of combined parent matrices to instances, do mats[i] * instance.localMat to get whole transform
		void getParentTransforms(Animator* animator, std::vector<glm::mat4>& mats);

		/*
		@instance: The armature instance. Not the mesh instance
		*/
		void getArmatureTransforms(Animator* animator, std::vector<glm::mat4>& mats, glm::mat4& instanceMat, AssetInstance* instance, ArmatureAsset* asset);

	};

	extern std::unordered_map<std::string, Asset*> engone_assets[ASSET_TYPES];

	template <class T>
	T* AddAsset(const std::string& name, T* asset) {
		//if (name.empty()) return;
		if (asset->error != None) {
			log::out << log::RED << log::TIME << " " << toString(asset->error) << ": " << asset->filePath << "\n" << log::SILVER;
		}
		engone_assets[(char)T::TYPE - 1][name] = asset;
		return asset;
	}
	/*
	@path is based in the assets directory
	*/
	template <class T>
	T* AddAsset(const std::string& name, const std::string& path) {
		//if (name.empty()||path.empty()) return;
		T* t = new T(path);
		t->setBaseName(name);
		return AddAsset<T>(name, t);
	}
	/*
	@path is based in the assets directory
	*/
	template <class T>
	T* AddAsset(const std::string& path) {
		//log::out << path << "\n";
		//if (path.empty()) return;
		return AddAsset<T>(path, path);
	}
	/*
	If asset isn't loaded, the function will try to load the asset with "name" as path.
	Function will not return nullptr
	*/
	template <class T>
	T* GetAsset(const std::string& name) {
		//log::out << name<<"\n";
		//if (name.empty()) return;
		auto& list = engone_assets[(char)T::TYPE - 1];
		if (list.count(name)) {
			return list[name]->cast<T>();
		} else {
			return AddAsset<T>(name);
		}
	}
	template <class T>
	void RemoveAsset(const std::string& name) {
		//if (name.empty()) return;

		auto& list = engone_assets[(char)T::TYPE - 1];
		if (list.count(name)) {
			delete list[name];
			list.erase(name);
		}
	}
}