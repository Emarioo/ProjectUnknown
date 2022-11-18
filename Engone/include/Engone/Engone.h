#pragma once

#include "Engone/RenderModule.h"
#include "Engone/EngoneFlag.h"
#include "Engone/Application.h"
#include "Engone/Window.h"

#include "Engone/SoundModule.h"
#include "Engone/Utilities/Utilities.h"
#include "Engone/Utilities/MemAllocator.h"
#include "Engone/Networking/NetworkModule.h"

#include "Engone/Rendering/FrameBuffer.h"
#include "Engone/Rendering/Light.h"

#include "Engone/Rendering/Camera.h"

#include "Engone/Utilities/rp3d.h"
#include "Engone/EngineObject.h"
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
		//void addObject(EngineObject* object);

		//Playground& getPlayground() { return playground; };

		//void addParticleGroup(ParticleGroupT* group);

		// the run time of stats
		inline double getEngineTime() const { return m_runtimeStats.getRunTime(); }

		void addLight(Light* l);
		void removeLight(Light* l);

		inline EngoneFlags getFlags() const {
			return m_flags;
		}
		inline void setFlags(EngoneFlags flags) {
			m_flags = flags;
		}

		RuntimeStats& getStats() { return m_runtimeStats; }
		
		void bindLights(Shader* shader, glm::vec3 objectPos);

		static TrackerId trackerId;
	private:
		RuntimeStats m_runtimeStats;

		EngoneFlags m_flags;

		// delta uniform is set in engine loop.
		std::vector<Light*> m_lights;

		bool m_loadedDefault=false;

		FrameBuffer frameBuffer;
		VertexBuffer quadBuffer;
		VertexArray quadArray;

		std::vector<Application*> m_applications{};
		std::vector<uint16_t> m_appSizes{}; // used for tracker
		std::vector<TrackerId> m_appIds{}; // used for tracker

		void update(LoopInfo& info);
		// only renders objects at the moment
		void render(LoopInfo& info);
		void renderObjects(LoopInfo& info);
	};
}