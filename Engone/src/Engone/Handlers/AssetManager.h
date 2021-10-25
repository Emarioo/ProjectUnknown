#pragma once

#include "../Handlers/FileHandler.h"
#include "../Rendering/Texture.h"
#include "../Rendering/Font.h"
#include "../Rendering/Shader.h"
#include "../DebugTool/DebugHandler.h"

#include "../Rendering/Buffer.h"

namespace engone {
	enum AssetError
	{
		None = 0,
		MissingFile,
		MissingData,
		BadSyntax,
		CorruptedData
	};
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
	class FileReader
	{
	public:
		FileReader() = default;
		FileReader(const std::string& path, bool binary = true) : binary(binary), path(path)
		{
			file.open(path);
			if (file) {
				file.seekg(0, file.end);
				fileSize = file.tellg();
				file.seekg(file.beg);
			}
			else {
				error = MissingFile;
				close();
				throw MissingFile;
			}
		}

		bool binary = false;
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

		template <class T>
		void read(T* var, uint32_t size = 1)
		{
			if (binary) {
				if (readHead - 1 + size * sizeof(T) > fileSize) {
					error = MissingData;
					throw MissingData;
				}

				file.read(reinterpret_cast<char*>(var), size);
				readHead += size * sizeof(T);
			}
			else {
				//std::string line;
				//getline(file, line)
			}
		}
		/*
		256 characters is the current max size.
		*/
		void read(std::string* var)
		{
			if (binary) {
				if (readHead - 1 + 1 > fileSize) {
					error = MissingData;
					throw MissingData;
				}
				size_t length;
				file.read(reinterpret_cast<char*>(length), 1);
				readHead++;

				if (readHead - 1 + length > fileSize) {
					error = MissingData;
					throw MissingData;
				}
				file.read(reinterpret_cast<char*>(var), length);
				readHead += length;
			}
			else {
				//std::string line;
				//getline(file, line);
			}
		}
	};

	class Asset
	{
	public:
		static const AssetType type = AssetType::None;
		Asset() = default;
		Asset(const std::string& path) : filePath(path) { Load(path); }

		std::string filePath;
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

		virtual void Load(const std::string& path) {};
	};
	class TextureAsset : public Asset
	{
	public:
		static const AssetType type = AssetType::Texture;
		TextureAsset() = default;
		TextureAsset(const std::string& path) : Asset(path) {};
		virtual void Load(const std::string& path) override;

		void Bind(unsigned int slot=0);

	private:
		unsigned int id;
		unsigned char* buffer = nullptr;
		int width = 0, height, BPP = 0;
	};
	class ShaderAsset : public Asset
	{
	public:
		static const AssetType type = AssetType::Shader;
		ShaderAsset() = default;
		ShaderAsset(const std::string& path) : Asset(path) {};
		ShaderAsset(const std::string& source, bool isSource) { Init(source); };
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
	class FontAsset : public Asset
	{
	public:
		static const AssetType type = AssetType::Font;
		FontAsset() = default;
		FontAsset(const std::string& path) : Asset(path) {};
		virtual void Load(const std::string& path) override;

		

	};
	class MaterialAsset : public Asset
	{
	public:
		static const AssetType type = AssetType::Material;
		MaterialAsset() = default;
		MaterialAsset(const std::string& path) : Asset(path) {};
		virtual void Load(const std::string& path) override;

		std::string diffuse_map;
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
		void GetValues(int frame, float blend, glm::vec3& pos, glm::vec3& euler, glm::vec3& scale, glm::mat4& quater);
		std::unordered_map<ChannelType, FCurve> fcurves;
	};
	class AnimationAsset : public Asset
	{
		static const AssetType type = AssetType::Animation;
		AnimationAsset() = default;
		AnimationAsset(const std::string& path) : Asset(path) {};
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
		Change to vector for better performance. This is done by ordering the objects.
		Shouldn't be to hard.
		*/
		std::unordered_map<unsigned short, Channels> objects;

		uint16_t frameStart = 0;
		uint16_t frameEnd = 0;
		float defaultSpeed = 24;// frames per second. 24 is default from blender.

	};
	class MeshAsset : public Asset
	{
	public:
		static const AssetType type = AssetType::Mesh;
		MeshAsset() = default;
		MeshAsset(const std::string& path) : Asset(path) {};
		virtual void Load(const std::string& path) override;

		enum class MeshType : char
		{
			Normal=0,
			Boned
		};

		MeshType meshType = MeshType::Normal;
		std::vector<Material*> materials;
		std::vector<Animation*> animations;
		TriangleBuffer buffer;
	};
	class ColliderAsset : public Asset
	{
	public:
		static const AssetType type = AssetType::Collider;
		ColliderAsset() = default;
		ColliderAsset(const std::string& path) : Asset(path) {};
		virtual void Load(const std::string& path) override;

		std::vector<Animation*> animations;
		float furthestPoint = 0;
		std::vector<std::uint16_t> quad; // 4 = quad
		std::vector<std::uint16_t> tri; // 3 = tri - not supported at the moment
		std::vector<glm::vec3> points;
	};
	class ArmatureAsset : public Asset
	{
	public:
		static const AssetType type = AssetType::Armature;
		ArmatureAsset() = default;
		ArmatureAsset(const std::string& path) : Asset(path) {};
		virtual void Load(const std::string& path) override;

		std::vector<Mesh*> meshes;
		std::vector<Animation*> animations;

	};
	class ModelAsset : public Asset
	{
	public:
		static const AssetType type = AssetType::Model;
		ModelAsset() = default;
		ModelAsset(const std::string& path) : Asset(path) {};
		virtual void Load(const std::string& path) override;

		std::vector<MeshAsset*> meshes;
		std::vector<ColliderAsset*> colliders;
		std::vector<ArmatureAsset*> armatures;

	};
	/*
	@path is based in the assets directory
	*/
	template <class T>
	AssetError AddAsset(const std::string& path);
	template <class T>
	AssetError AddAsset(const std::string& name, T* asset);
	/*
	@return can be nullptr if the asset is missing. Remember to handle it.
	*/
	template <class T>
	T* GetAsset(const std::string& path);
	template <class T>
	void RemoveAsset(const std::string& path);
	/*
	If file isn't found an error will occur and an empty object will
	be created. Set the object's hasError to false to modify
	*/
	void AddAnimationAsset(const std::string& file);
	/*
	returns nullptr if name doesn't exist
	*/
	Animation* GetAnimationAsset(const std::string& name);
	void DeleteAnimationAsset(const std::string& name);

	void AddArmatureAsset(const std::string& file);
	/*
	returns nullptr if name doesn't exist
	*/
	Armature* GetArmatureAsset(const std::string& name);

	void DeleteArmatureAsset(const std::string& name);

	void AddMaterialAsset(const std::string& file);
	Material* GetMaterialAsset(const std::string& name);
	void DeleteMaterialAsset(const std::string& name);

	void AddMeshAsset(const std::string& file);
	Mesh* GetMeshAsset(const std::string& name);
	void DeleteMeshAsset(const std::string& name);

	void AddColliderAsset(const std::string& file);
	/*
	Add empty collider. Used for objects with unique collisions
	*/
	//CollData* AddColl(const std::string& name);
	/*
	Will return nullptr if collider doesn't exist
	*/
	Collider* GetColliderAsset(const std::string& name);
	void DeleteColliderAsset(const std::string& name);

	void AddModelAsset(const std::string& file);
	/*
	Modified to try adding model asset to the game unless it already is loaded.
	*/
	Model* GetModelAsset(const std::string& name);
	void DeleteModelAsset(const std::string& name);
	/*
	void AddTextureAsset(const std::string& name);
	Texture* GetTextureAsset(const std::string& name);
	void DeleteTextureAsset(const std::string& name);
	*/
	Font* GetFont(const std::string& name);
	void AddFont(const std::string& name, Font* font);
	Shader* GetShader(const std::string& name);
	void AddShader(const std::string& name, Shader* shader);
	Texture* GetTexture(const std::string& name);
	void AddTexture(const std::string& name, Texture* texture);
}