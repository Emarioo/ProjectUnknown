#pragma once

#include "Engone/Rendering/Buffer.h"
#include "Engone/Utilities/Utilities.h"
#include "Engone/Logger.h"

namespace engone {

	// used in ModelAsset, not great but it works
	class Animator;

	// FEATURE: Replace with trackerId?
	enum AssetType : uint8_t {
		AssetTexture = 0,
		AssetFont,
		AssetShader,
		AssetMaterial,
		AssetMesh,
		AssetAnimation,
		AssetArmature,
		AssetCollider,
		AssetModel,

		AssetTypeCount,
	};
	std::string toString(AssetType t);
	namespace log {
		logger operator<<(logger log, AssetType type);
	}

	class Assets;
	// Default constructors do not have tracking, they may not be
	class Asset {
	public:
		static const AssetType TYPE = AssetTypeCount; // General type
		AssetType type = AssetTypeCount; // my type

		virtual ~Asset() {}

		std::string getRootPath() const {
			uint32_t first = filePath.find_first_of("/");
			uint32_t last = filePath.find_last_of("/");

			if (first == last) {// directly in assets
				return "";
			} else {
				return filePath.substr(7, last - 6);
			}
		}
		void setBaseName(const std::string& name) {
			uint32_t last = name.find_last_of("/");
			if (last != -1)
				baseName = name.substr(last + 1);
			else
				baseName = name;
		}
		const std::string& getBaseName() const { return baseName; }

		FileError error = FileErrorNone;
		// not valid if an error occured while loading.
		// asset is valid if it hasn't been loaded yet.
		bool valid() const { return error == FileErrorNone; }

		template <class T>
		T* cast() { return reinterpret_cast<T*>(this); }
		// assets is used to aquire more assets.
		virtual void load(const std::string& path) { std::cout << "virtual load of asset\n"; };

		//// if you are tracking the object
		//bool setTracked()

		Assets* m_parent = nullptr;
	protected:

		// used when you attach an Asset to Assets with set(string,T* asset)
		// this way it knows if it's tracked or not.
		bool m_tracked = false;

		std::string baseName;
		std::string filePath;

		Asset(AssetType type) : type(type) {};
		//Asset(AssetType type, Assets* assets=nullptr) : type(type), m_parent(assets) {};
		//Asset(AssetType type, const std::string& path, Assets* assets=nullptr) : type(type), filePath(path), m_parent(assets) { }

		friend class Assets;
	};
	class Texture : public Asset {
	public:
		static const AssetType TYPE = AssetTexture;
		Texture() : Asset(TYPE)  { };
		Texture(const std::string& path) : Asset(TYPE) { load(path); };
		//Texture(const char* inBuffer, uint32_t size, Assets* assets = nullptr) : Asset(TYPE, "") { load(inBuffer, size, assets); };
		~Texture() { }
		void load(const void* inBuffer, uint32_t size);
		void load(const std::string& path) override;

		void init(int w, int h, void* data);

		void subData(int x, int y, int w, int h, void* data);
		void bind(unsigned int slot = 0);

		int getWidth();
		int getHeight();

		static TrackerId trackerId;
	private:
		unsigned int id = 0;
		unsigned char* buffer = nullptr;
		int width = 0, height = 0, BPP = 0;
	};
	// FEATURE: load shader as resource? like fonts but with text. Naybe not though because resources may be specific to visual studio which means you won't be able to compile the elsewhere.
	class Shader : public Asset {
	public:
		static const AssetType TYPE = AssetShader;
		Shader() : Asset(TYPE) { }
		
		Shader(const char* source) : Asset(TYPE) { init(source); }
		//Shader(const std::string& path) : Asset(TYPE,path + ".glsl") { load(filePath); }
		~Shader() { }

		//void load(const std::string& path, Assets* assets) override;
		void init(const char* source);

		struct ShaderSource {
			const char* start = nullptr; // this is not null terminated
			int length = 0;
			int line = 0;// which line on the original

			bool operator==(ShaderSource src) {
				return start == src.start&&length==src.length;
			}
		};
		ShaderSource vs, fs;
		const char* original = nullptr;

		void bind();
		void setInt(const std::string& name, int i);
		void setFloat(const std::string& name, float f);
		void setVec2(const std::string& name, glm::vec2);
		void setIVec2(const std::string& name, glm::ivec2);
		void setVec3(const std::string& name, glm::vec3);
		void setIVec3(const std::string& name, glm::ivec3);
		void setVec4(const std::string& name, float f0, float f1, float f2, float f3);
		void setMat4(const std::string& name, glm::mat4 v);

		static TrackerId trackerId;
	private:
		uint32_t id = 0;
		uint32_t createShader(ShaderSource vertexSrc, ShaderSource fragmentSrc);
		uint32_t compileShader(uint32_t type, ShaderSource source);

		unsigned int getUniformLocation(const std::string& name);
		std::unordered_map<std::string, unsigned int> uniLocations;
	};
	class Font: public Asset
	{
	public:
		static const AssetType TYPE = AssetFont;
		Font() : Asset(TYPE) {  };
		//Font(const std::string& path, Assets* assets = nullptr) : Asset(TYPE,path) { load(filePath,assets); };
		Font(const void* bufferImg,uint32_t size, const void* bufferTxt, uint32_t size2) : Asset(TYPE) { load(bufferImg,size,bufferTxt,size2); };
		~Font() { }
		
		void load(const std::string& path) override;
		void load(const void* bufferImg,uint32_t size,const void* bufferTxt, uint32_t size2);

		float getWidth(const std::string& str, float height);

		Texture texture;
		int charWid[256];

		int imgSize = 1024;
		int charSize = imgSize / 16;

		static TrackerId trackerId;
	private:
	};
	class MaterialAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetMaterial;

		MaterialAsset() : Asset(TYPE) { };
		MaterialAsset(const std::string& path) : Asset(TYPE) { load(path); };
		void load(const std::string& path) override;

		~MaterialAsset() {
			if (diffuse_map) {
				if (!diffuse_map->m_parent) {
					GetTracker().untrack(diffuse_map);
					delete diffuse_map;
					diffuse_map = nullptr;
				}
			}
		}
		Texture* diffuse_map=nullptr;
		glm::vec3 diffuse_color = { 1,1,1 };
		glm::vec3 specular = { .5f,.5,.5f };
		float shininess = .5;

		void bind(Shader* shader, int index);

		static TrackerId trackerId;
	private:
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
		AnimationAsset(const std::string& path) : Asset(TYPE) { load(path); };
		~AnimationAsset() {}
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

		static TrackerId trackerId;
	private:
	};
	class MeshAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetMesh;
		MeshAsset() : Asset(TYPE) {  };
		MeshAsset(const std::string& path) : Asset(TYPE) { load(path); };
		~MeshAsset() {
			for (int i = 0; i < materials.size(); i++) {
				MaterialAsset* mat = materials[i];
				if (!mat->m_parent) {
					GetTracker().untrack(mat);
					delete mat;
				}
			}
			materials.clear();
		}
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

		static TrackerId trackerId;
	private:

	};
	class ColliderAsset : public Asset
	{
	public:
		static const AssetType TYPE = AssetCollider;
		ColliderAsset() : Asset(TYPE) {  };
		ColliderAsset(const std::string& path) : Asset(TYPE) { load(path); };
		~ColliderAsset() {}
		void load(const std::string& path) override;

		enum Type : uint8_t {
			SphereType=0,
			CubeType=1,
			CapsuleType=2,
			MapType=3,
		};
		// Height map
		std::vector<glm::vec3> points;
		uint32_t gridWidth=0,gridHeight=0;

		Type colliderType = Type::SphereType;
		union {
			struct {// sphere
				float radius=0;
			} sphere;
			struct {// cube
				glm::vec3 size = { 0,0,0 };
			} cube;
			struct {
				float radius=0, height = 0;
			} capsule;
			struct {
				uint32_t gridColumns = 0, gridRows = 0;
				float minHeight=0, maxHeight=0;
				glm::vec3 scale;
				std::vector<float> heights;
			} map;
		};

		static TrackerId trackerId;
	private:
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
		ArmatureAsset(const std::string& path) : Asset(TYPE) { load(path); };
		~ArmatureAsset() {}
		void load(const std::string& path) override;

		std::vector<Bone> bones;

		static TrackerId trackerId;
	private:
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

		static TrackerId trackerId;
	private:
	};
	class ModelAsset : public Asset {
	public:
		static const AssetType TYPE = AssetModel;
		ModelAsset() : Asset(TYPE) {  };
		ModelAsset(const std::string& path) : Asset(TYPE) { load(path); };
		~ModelAsset() {
			for (int i = 0; i < animations.size(); i++) {
				AnimationAsset* anim = animations[i];
				if (!anim->m_parent) {
					GetTracker().untrack(anim);
					delete anim;
				}
			}
			animations.clear();
		}
		void load(const std::string& path) override;

		/*
		List of mesh, collider and armature transforms.
		transforms[meshes.size()+colliders.size()+1] to get the second armature transform
		*/
		//std::vector<glm::mat4> transforms;
		std::vector<AssetInstance> instances;
		std::vector<AnimationAsset*> animations;
		
		// Will give a list of combined parent matrices to instances, do mats[i] * instance.localMat to get whole transform
		std::vector<glm::mat4> getParentTransforms(Animator* animator);

		// will find the first instance with asset type T
		//template<typename T>
		//AssetInstance* findInstance() {
		//	for (int i = 0; i < instances.size(); i++) {
		//		auto& inst = instances[i];
		//		if (inst.asset->type == T::TYPE) {
		//			return inst.asset;
		//		}
		//	}
		//	return nullptr;
		//}

		/*
		@instance: The armature instance. Not the mesh instance
		*/
		std::vector<glm::mat4> getArmatureTransforms(Animator* animator, glm::mat4& instanceMat, AssetInstance* instance, ArmatureAsset* asset,std::vector<glm::mat4>* boneMats=nullptr);

		static TrackerId trackerId;
	private:
	};
	// FEATURE: Consider adding functions like in engone where an application is created with createApplication.
	//		In assets you could say, createAsset as a template and supply info.
	class Assets {
	public:
		Assets() = default;
		~Assets();

		// ISSUE: remove flags since they don't seem to be needed?
		enum Flag : uint8_t {
			FlagNone = 0,
			FlagReplace = 1,
		};

		// created asset will be owned by assets.
		// you should not delete the returned pointer.
		//template<class T>
		//T* create(const std::string& name, const std::string& path) {
		//	T* t = new T();
		//	t->m_parent = this;
		//	t->load(path);
		//	m_assets[T::TYPE][name] = t;
		//}

		// Will "give" asset to this class. Assets will become the owner and destroy asset when the time has come.
		// Do not delete asset yourself.
		template<class T>
		T* set(const std::string& name, T* asset, Flag flags = FlagNone) {
			if (!asset) return asset;
			if (asset->error != FileErrorNone) {
				log::out << log::RED << " " << toString(asset->error) << ": " << asset->filePath << "\n";
			}
			GetTracker().track(asset); // tracker will not track duplicates so it is safe to call track twice on an asset.
			asset->m_parent = this;
			m_assets[T::TYPE][name] = asset;
			return asset;
		}
		template<class T>
		T* get(const std::string& name, Flag flags = FlagNone) {
			auto& list = m_assets[T::TYPE];
			auto find = list.find(name);
			if (find != list.end()) {
				return find->second->cast<T>();
			} else {
				log::out << log::RED << "Assets::get - asset is nullptr\n";
				return nullptr;
			}
		}
		template<class T>
		T* set(const std::string& name, const std::string& path, Flag flags = FlagNone) {
			std::string _path = modifyPath<T>(path);
			if (!(flags & FlagReplace)) { // If we don't want to replace asset, see if asset with name already exists and return it if so.
				auto& list = m_assets[T::TYPE];
				auto find = list.find(name);
				if (find != list.end()) {
					return list[name]->cast<T>();
				}
			}
			// ISSUE: log::out << "memory leak if an asset with the same name exists and we replace it\n";
			T* t = new T();
			if (t) {
				t->m_parent = this;
				t->load(_path);
				GetTracker().track(t);
				t->setBaseName(name);
				set(name, t);
				return t;
			} else return nullptr;
		}
		template<class T>
		T* set(const std::string& path, Flag flags = FlagNone) {
			return set<T>(path, path, flags);
		}
		template<class T>
		void remove(const std::string& name, Flag flags = FlagNone) {
			auto& list = m_assets[T::TYPE];
			auto find = list.find(name);
			if (find != list.end()) {
				GetTracker().untrack((T*)find->second);
				delete find->second;
				list.erase(name);
			}
		}
		template<typename T>
		std::string modifyPath(const std::string& str) {
			std::string out;

			if (!m_baseDir.empty()) out += m_baseDir;
			
			if (T::TYPE == ModelAsset::TYPE)
				out += "models/";

			out+= str;
			return out;
		}
		const std::string& getBaseDir() const { return m_baseDir; };
		void setBaseDir(std::string str) { m_baseDir = str; };
		std::unordered_map<std::string, Asset*> m_assets[AssetTypeCount];
	private:
		template<class T>
		void removeAssets() {
			std::unordered_map<std::string, Asset*>& list = m_assets[T::TYPE];
			for (auto [key, val] : list) {
				GetTracker().untrack((T*)val);
				delete val;
			}
		}

		std::string m_baseDir = "assets/";
	};
}