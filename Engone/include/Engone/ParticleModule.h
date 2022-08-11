#pragma once

#include "Engone/Window.h"

namespace engone {
	struct Particle {
		glm::vec3 pos;
		glm::vec3 vel;
	};
	
	// A class for rendering particles. Requires a shader specific for rendering particles with shader buffer.
	// "delta" uniform in shader is set before rendering particles.
	// 
	class ParticleGroup {
	public:
		ParticleGroup() = default;
		~ParticleGroup() { cleanup(); }

		void init(Window* window, Shader* shader);

		// Will update projection and bind shader
		void render(engone::RenderInfo& info);

		// will allocate particles for you to change.
		Particle* createParticles(int count, bool setZero = true);
		bool createCube(glm::vec3 position, glm::vec3 scale, uint32_t particleCount);

		// Always call this function when changing particle data. (unless you used createParticles).
		// The shader changes the data on the gpu side which means 
		// that it needs to be synchronized with the internal array.
		Particle* getParticles();
		uint32_t getCount() const { return m_count; };
		uint32_t getCapacity() const { return m_capacity; }

		Shader* getShader() const { return m_shader; }

		void clear();
		void cleanup();

		// will change the size of the internal particle array.
		// only do this when rendering because the shaderBuffer will set data and requires the correct active window
		bool resize(int count);
	private:
		Window* m_parent = nullptr;

		bool m_updateBuffer = false;
		Shader* m_shader = nullptr;
		Particle* m_particles = nullptr;
		int m_capacity = 0;
		int m_count = 0;

		ShaderBuffer m_shaderBuffer{};
	};

	//class ParticleModule {
	//public:
	//	ParticleModule() = default;
	//	~ParticleModule();

	//	// module will be initialized on specified window.
	//	void init(Window* window);

	//	void update(engone::UpdateInfo& ínfo);
	//	// call init function for this to work
	//	void render(engone::RenderInfo& info);

	//	int getParticleCount() const { return m_count; }

	//	// remove all particles
	//	void clear();

	//	// free memory
	//	void cleanup();

	//	static const int PARTICLE_BATCH = 1000000;
	//private:
	//	Window* m_parent = nullptr;

	//	bool m_updateBuffer = false;
	//	Particle* m_particles = nullptr;
	//	//ParticleInfo* m_particleInfos = nullptr;
	//	int m_capacity = 0;
	//	int m_count = 0;
	//	// count as in particles not bytes
	//	bool resize(int count);

	//	bool m_initiatedBuffers = false;
	//	//VertexBuffer m_particleVB{};
	//	//VertexArray m_particleVA{};
	//	ShaderBuffer m_particleSB{};
	//};
}