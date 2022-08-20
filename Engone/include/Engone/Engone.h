#pragma once

#include "Engone/RenderModule.h"
#include "Engone/EngoneFlag.h"
#include "Engone/Application.h"
#include "Engone/Window.h"

#include "Engone/SoundModule.h"
#include "Engone/Utilities/Utilities.h"
#include "Engone/Utilities/MemAllocator.h"
#include "Engone/NetworkModule.h"

#include "Engone/Rendering/FrameBuffer.h"
#include "Engone/Rendering/Light.h"

#include "Engone/Rendering/Camera.h"

#include "Engone/Utilities/rp3d.h"
#include "Engone/GameObject.h"
#include "Engone/Utilities/RuntimeStats.h"
#include "Engone/ParticleModule.h"

namespace engone {
	enum EngoneFlag : uint32_t {
		EngoneNone = 0,
		EngoneFixedLoop = 1, // update and frame will be called each game loop iteration. FPS will be used and UPS will be ignored.
	};
	typedef uint32_t EngoneFlags;
	class Engone {
	public:
		Engone();
		~Engone();

		// Will create an application in the engine.
		// The created app will be tracked which means it requires TrackerId. I would recommend looking at an Application example.
		template<class T>
		T* createApplication() {
			T* app = new T(this);
			GetTracker().track(app);
			m_applications.push_back(app);
			m_appSizes.push_back(sizeof(T));
			m_appIds.push_back(T::trackerId);
			return app;
		}
		// See other overloaded function.
		template<class T, typename A>
		T* createApplication(const A& arg) {
			T* app = new T(this, arg);
			GetTracker().track(app);

			m_applications.push_back(app);
			m_appSizes.push_back(sizeof(T));
			m_appIds.push_back(T::trackerId);
			return app;
		}
		// I wouldn't recommend adding or removing apps from the vector.
		// Instead use createApplication or Application::stop if you want to add or remove an app.
		std::vector<Application*>& getApplications() { return m_applications; }

		/*
		Start the engine loop which will run applications until their windows are closed.
		*/
		void start();
		// don't add nullptr, object is assumed to be valid.
		void addObject(GameObject* object);

		void addParticleGroup(ParticleGroupT* group);

		// the run time of stats
		inline double getEngineTime() const { return m_runtimeStats.getRunTime(); }
#ifdef ENGONE_PHYSICS
		rp3d::PhysicsCommon* m_pCommon=nullptr;
		rp3d::PhysicsWorld* m_pWorld=nullptr;
#endif
		void addLight(Light* l);
		void removeLight(Light* l);

		inline EngoneFlags getFlags() const {
			return m_flags;
		}
		inline void setFlags(EngoneFlags flags) {
			m_flags = flags;
		}

		RuntimeStats& getStats() { return m_runtimeStats; }

		static TrackerId trackerId;
	private:
		RuntimeStats m_runtimeStats;

		EngoneFlags m_flags;

		// delta uniform is set in engine loop.
		std::vector<ParticleGroupT*> m_particleGroups;
		std::vector<GameObject*> m_objects;
		std::vector<Light*> m_lights;
		bool m_loadedDefault=false;

		FrameBuffer frameBuffer;
		VertexBuffer quadBuffer;
		VertexArray quadArray;

		std::vector<Application*> m_applications{};
		std::vector<uint16_t> m_appSizes{}; // used for tracker
		std::vector<TrackerId> m_appIds{}; // used for tracker

		void update(UpdateInfo& info);
		// only renders objects at the moment
		void render(RenderInfo& info);
		void renderObjects(RenderInfo& info);

		void bindLights(Shader* shader, glm::vec3 objectPos);

	};

	//void Initialize(EngoneOption hints=EngoneOption::None);

	/*
	Start the loop running the applications.
	When all windows of all applications are closed the loop will end.
	*/
	//void Start();
	//void AddApplication(Application* app);

	//FrameBuffer& GetDepthBuffer();

	//Camera* GetCamera();

	/*
	Update objects and camera (viewMatrix)
	 also dimensions but they should be moved somewhere else.
	*/
	//void UpdateObjects(float delta);

	/*
	Render objects, hitboxes with the appropriate shaders.
	*/
	//void RenderObjects(float lag);
	/*
	Render objects without doing anything but calling the object's draw function.
	*/
	//void RenderRawObjects(Shader* shader, float lag);

	/*
	Render objects, ui, debug tool
	*/
	//void RenderEngine(float lag);
	/*
	Update objects, ui, debug tool
	*/
	//void UpdateEngine(float delta);

	//struct Delayed {
	//	float time;
	//	std::function<void()> func;
	//};

	//void AddTimer(float time, std::function<void()> func);
	//void UpdateTimers(float delta);

	// Light
	//void AddLight(Light* l);
	//void RemoveLight(Light* l);

	/*
	Binds light to current shader
	 If one of the four closest light are already bound then don't rebind them [Not added]
	*/
	//void BindLights(Shader* shader, glm::vec3 objectPos);
	//std::vector<Light*>& GetLights();
}