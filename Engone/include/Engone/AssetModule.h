#pragma once

#include "Engone/Rendering/Buffer.h"
#include "Engone/Utilities/Utilities.h"
#include "Engone/Logger.h"

namespace engone {

	// used in ModelAsset, not great but it works
	class Animator;

	enum AssetType : uint8_t {
		AssetNone = 0,
		AssetTexture,
		AssetFont,
		AssetShader,
		AssetMaterial,
		AssetMesh,
		AssetAnimation,
		AssetArmature,
		AssetModel,
		AssetCollider,

		AssetTypeCount, // count+1
	};
	std::string toString(AssetType t);
	namespace log {
		logger operator<<(logger log, AssetType type);
	}
	
	class Assets;
	class Asset
	{
	public:
		static const AssetType TYPE = AssetNone; // General type
		AssetType type = AssetNone; // my type
		Asset(AssetType type) : type(type) {};
		Asset(AssetType type,const std::string& path) : type(type), filePath(path) { }

		std::string baseName;
		std::string filePath;
		std::string getRootPath(){
			uint32_t first = filePath.find_first_of("/");
			uint32_t last = filePath.find_last_of("/");

			if (first == last) {// directly in assets
				return "";
			}
			else {
				return filePath.substr(7, last-6);
			}
		}
		void setBaseName(const std::string& name)
		{
			uint32_t last = name.find_last_of("/");
			if(last!=-1)
				baseName = name.substr(last+1);
			else
				baseName = name;
		}

		FileError error=FileErrorNone;

		bool operator!() {
			return error != FileErrorNone;
		}
		operator bool() {
			return error == FileErrorNone;
		}

		template <class T>
		T* cast() { return reinterpret_cast<T*>(this); }
		// assets is used to aquire more assets.
		virtual void load(const std::string& path, Assets* assets) { std::cout << "virtual func of asset\n"; };
	};
	class Texture : public Asset
	{
	public:
		static const AssetType TYPE = AssetTexture;
		Texture() : Asset(TYPE)  { };
		Texture(const std::string& path, Assets* assets=nullptr) : Asset(TYPE,path + ".png") { load(filePath,assets); };
		Texture(const char* inBuffer, uint32_t size, Assets* assets=nullptr) : Asset(TYPE,"") { load(inBuffer,size,assets); };
		void load(const char* inBuffer, uint32_t size, Assets* assets);
		void load(const std::string& path, Assets* assets) override;
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
		static const AssetType TYPE = AssetShader;
		Shader() : Asset(TYPE) { }
		Shader(const std::string& source, Assets* assets=nullptr) : Asset(TYPE) { init(source); }
		//Shader(const std::string& path) : Asset(TYPE,path + ".glsl") { load(filePath); }
		
		//void load(const std::string& path, Assets* assets) override;
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
		static const AssetType TYPE = AssetFont;
		Font() : Asset(TYPE) {  };
		Font(const std::string& path, Assets* assets = nullptr) : Asset(TYPE,path) { load(filePath,assets); };
		Font(const char* bufferImg,uint32_t size, const char* bufferTxt, uint32_t size2, Assets* assets = nullptr) : Asset(TYPE,"") { load(bufferImg,size,bufferTxt,size2,assets); };
		void load(const std::string& path, Assets* assets) override;
		void load(const char* bufferImg,uint32_t size,const char* bufferTxt, uint32_t size2, Assets* assets);

		float getWidth(const std::string& str, float height);

		Texture texture;
		int charWid[256];

		int imgSize = 1024;
		int charSize = imgSize / 16;

	};
	class MaterialAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetMaterial;

		MaterialAsset() : Asset(TYPE) { };
		MaterialAsset(const std::string& path, Assets* assets = nullptr) : Asset(TYPE,path + ".material") { load(filePath,assets); };
		void load(const std::string& path, Assets* assets) override;

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
		static const AssetType TYPE = AssetAnimation;
		AnimationAsset() : Asset(TYPE) {  };
		AnimationAsset(const std::string& path, Assets* assets = nullptr) : Asset(TYPE,path+".animation") { load(filePath,assets); };
		void load(const std::string& path, Assets* assets) override;

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
		static const AssetType TYPE = AssetMesh;
		MeshAsset() : Asset(TYPE) {  };
		MeshAsset(const std::string& path, Assets* assets = nullptr) : Asset(TYPE,path+".mesh") { load(filePath,assets); };
		void load(const std::string& path, Assets* assets) override;

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
		static const AssetType TYPE = AssetCollider;
		ColliderAsset() : Asset(TYPE) {  };
		ColliderAsset(const std::string& path, Assets* assets = nullptr) : Asset(TYPE,path+".collider") { load(filePath,assets); };
		void load(const std::string& path, Assets* assets) override;

		enum class Type : char {
			Cube=0,
			Sphere=1,
			HeightMap=2
		};
		// Height map
		std::vector<glm::vec3> points;
		uint32_t gridWidth=0,gridHeight=0;

		Type colliderType = Type::Cube;
		union {
			struct {// cube
				glm::vec3 size = { 0,0,0 };
			} cube;
			struct {// sphere
				float radius=0;
			} sphere;
			struct {
				uint32_t gridWidth = 0, gridHeight = 0;
				float minHeight=0, maxHeight=0;
				glm::vec3 scale;
				std::vector<float> heights;
			} heightMap;
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
		static const AssetType TYPE = AssetArmature;
		ArmatureAsset() : Asset(TYPE) { };
		ArmatureAsset(const std::string& path, Assets* assets = nullptr) : Asset(TYPE,path + ".armature") { load(filePath,assets); };
		void load(const std::string& path, Assets* assets) override;

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
		static const AssetType TYPE = AssetModel;
		ModelAsset() : Asset(TYPE) {  };
		ModelAsset(const std::string& path, Assets* assets = nullptr) : Asset(TYPE, path + ".model") { load(filePath,assets); };
		void load(const std::string& path, Assets* assets) override;

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
	class Assets {
	public:
		Assets() = default;

		enum Flag : uint8_t {
			FlagNone = 0,
			FlagReplace = 1,
		};

		template<class T>
		T* set(const std::string& name, T* asset, Flag flags = FlagNone) {
			if (!asset) return asset;
			if (asset->error != FileErrorNone) {
				log::out << log::RED << " " << toString(asset->error) << ": " << asset->filePath << "\n";
			}
			engone_assets[T::TYPE - 1][name] = asset;
			return asset;
		}
		template<class T>
		T* get(const std::string& name, Flag flags = FlagNone) {
			auto& list = engone_assets[T::TYPE - 1];
			auto find = list.find(name);
			if (find != list.end()) {
				return find->second->cast<T>();
			} else {
				log::out << log::RED << " asset is nullptr\n";
				return nullptr;
			}
		}
		template<class T>
		T* set(const std::string& name, const std::string& path, Flag flags = FlagNone) {
			std::string _path = modifyPath(path);
			T* t;
			if (!(flags & FlagReplace)) { // If we don't want to replace asset, see if asset with name already exists and return it if so.
				auto& list = engone_assets[T::TYPE - 1];
				auto find = list.find(name);
				if (find != list.end()) {
					return list[name]->cast<T>();
				}
			}
			//log::out << "memory leak if an asset with the same name exists and we replace it\n";
			t = new T(_path, this);
			t->setBaseName(name);
			set(name, t);
			return t;
		}
		template<class T>
		T* set(const std::string& path, Flag flags = FlagNone) {
			return set<T>(path, path, flags);
		}
		template<class T>
		void remove(const std::string& name, Flag flags = FlagNone) {
			auto& list = engone_assets[(uint8_t)T::TYPE - 1];
			auto find = list.find(name);
			if (find != list.end()) {
				delete find->second;
				list.erase(name);
			}
		}
		std::string modifyPath(const std::string& str) {
			if (baseDir.empty()) return str;
			return baseDir + str;
		}
		const std::string& getBaseDir() const { return baseDir; };
		void setBaseDir(std::string str) { baseDir = str; };
	private:
		std::unordered_map<std::string, Asset*> engone_assets[AssetTypeCount - 1];
		std::string baseDir = "assets/";
	};

	//extern std::unordered_map<std::string, Asset*> engone_assets[ASSET_TYPES];

	//template <class T>
	//T* AddAsset(const std::string& name, T* asset) {
	//	//if (name.empty()) return;
	//	if (asset->error != None) {
	//		log::out << log::RED << log::TIME << " " << toString(asset->error) << ": " << asset->filePath << "\n" << log::SILVER;
	//	}
	//	engone_assets[(char)T::TYPE - 1][name] = asset;
	//	return asset;
	//}
	///*
	//@path is based in the assets directory
	//*/
	//template <class T>
	//T* AddAsset(const std::string& name, const std::string& path) {
	//	//if (name.empty()||path.empty()) return;
	//	T* t = new T(path);
	//	t->setBaseName(name);
	//	return AddAsset<T>(name, t);
	//}
	///*
	//@path is based in the assets directory
	//*/
	//template <class T>
	//T* AddAsset(const std::string& path) {
	//	//log::out << path << "\n";
	//	//if (path.empty()) return;
	//	return AddAsset<T>(path, path);
	//}
	///*
	//If asset isn't loaded, the function will try to load the asset with "name" as path.
	//Function will not return nullptr
	//*/
	//template <class T>
	//T* GetAsset(const std::string& name) {
	//	//log::out << name<<"\n";
	//	//if (name.empty()) return;
	//	auto& list = engone_assets[(char)T::TYPE - 1];
	//	if (list.count(name)) {
	//		return list[name]->cast<T>();
	//	} else {
	//		return AddAsset<T>(name);
	//	}
	//}
	//template <class T>
	//void RemoveAsset(const std::string& name) {
	//	//if (name.empty()) return;

	//	auto& list = engone_assets[(char)T::TYPE - 1];
	//	if (list.count(name)) {
	//		delete list[name];
	//		list.erase(name);
	//	}
	//}
}