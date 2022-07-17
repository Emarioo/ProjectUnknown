#pragma once

#include "Engone/EngoneFlag.h"
#include "Engone/RenderModule.h"
#include "Engone/Application.h"
#include "Engone/Window.h"

#include "Engone/SoundModule.h"
#include "Engone/Utilities/Utilities.h"
#include "Engone/Utilities/MemAllocator.h"
#include "Engone/NetworkModule.h"

#include "Engone/Rendering/FrameBuffer.h"
#include "Engone/Rendering/Light.h"

#include "Engone/Rendering/Camera.h"

#include "reactphysics3d/reactphysics3d.h"
#include "Engone/GameObject.h"

namespace engone {
	struct EngineStats {
		double startTime=0;// what the epoch time was when engine started

		uint64_t totalFrames=0;
		uint64_t totalUpdates=0;

		uint32_t skippedUpdates=0;

		void print() {
			std::string time = FormatTime(GetSystemTime() - startTime, false,FormatTimeUS|FormatTimeMS|FormatTimeS|FormatTimeM|FormatTimeH);
			log::out << "Running Time: "<<time<<"\n";
			log::out << "Total frames/updates: "<<totalFrames<<" / "<<totalUpdates<<"\n";
			log::out << "Skipped updates: "<<skippedUpdates<<" (potentially missed isKeyPressed)\n";
		}
	};
	class Engone {
	public:
		Engone(EngoneFlag flags=EngoneFlagNone);
		~Engone();

		// Will create an application in the engine.
		template<class T>
		T* createApplication() {
			T* app = new T(this);
			m_applications.push_back(app);
			return app;
		}
		template<class T, typename A>
		T* createApplication(const A& arg) {
			T* app = new T(this,arg);
			m_applications.push_back(app);
			return app;
		}
		// I wouldn't recommend adding or removing apps from the vector
		// Instead use createApplication or Application::stop if you want to add or remove applications.
		std::vector<Application*>& getApplications() { return m_applications; }

		/*
		Start the engine loop which will run applications until their windows are closed.
		*/
		void start();
		// don't add nullptr
		void addObject(GameObject* object);

		inline double getEngineTime() const { return GetSystemTime()-m_engineStats.startTime; }

		rp3d::PhysicsCommon* m_pCommon=nullptr;
		rp3d::PhysicsWorld* m_pWorld=nullptr;

		void addLight(Light* l);
		void removeLight(Light* l);

		const EngineStats& getStats() const { return m_engineStats; }

	private:
		EngineStats m_engineStats;

		int buf1[5];
		std::vector<GameObject*> m_objects;
		int buf2[5];
		std::vector<Light*> m_lights;
		bool m_loadedDefault=false;

		std::vector<Application*> m_applications;
		EngoneFlag m_flags;

		VertexBuffer instanceBuffer;

		// Render bit

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