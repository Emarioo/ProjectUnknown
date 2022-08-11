
#include "Engone/ParticleModule.h"

namespace engone {

//	static const char* particleGLSL = {
//#include "Engone/Shaders/particle.glsl"
//	};

	void ParticleGroup::init(Window* window, Shader* shader) {
		if (m_parent) {
			log::out << log::YELLOW << "ParticleModule::init - cannot initialize twice\n";
			return;
		}
		m_parent = window;
		if (!window) {
			log::out << log::RED << "ParticleModule::init - window was nullptr\n";
			return;
		}
		m_shader = shader;
		if (!shader) {
			log::out << log::RED << "ParticleModule::init - shader was nullptr\n";
			return;
		}
	}
	void ParticleGroup::render(engone::RenderInfo& info) {
		if (m_count == 0 || !m_parent || !m_shader) return;
		if (!m_shaderBuffer.initialized()) return;

		//EnableDepth();
		EnableBlend();
		Assets* assets = m_parent->getAssets();
		Renderer* renderer = m_parent->getRenderer();

		m_shader->bind();
		renderer->updateProjection(m_shader);
		m_shader->setFloat("delta",1/60.f);

		//uint32_t batch = m_shaderBuffer.getCount();

		//-- The commented code is for batch rendering
		//bool shouldSet = m_count > batch || m_updateBuffer;
		//// will be false if one batch draw is enough and particles hasn't been updated. Good for performance.
		//uint32_t remaining = m_count;
		//while (remaining > 0) {
		//	int drawPoints = std::min(remaining, batch);
		//	if (shouldSet) {
		//		m_shaderBuffer.setData(drawPoints * sizeof(Particle), m_particles + m_count - remaining);
		//	}
		//	m_shaderBuffer.drawPoints(m_count, 0);
		//	remaining -= drawPoints;
		//}
		if (m_count != 0) {
			if (m_updateBuffer) {
				m_shaderBuffer.setData(m_count * sizeof(Particle), m_particles);
			}
			m_shaderBuffer.drawPoints(m_count, 0);
		}

		m_updateBuffer = false;

		ui::TextBox partCount = { "Particles: " + std::to_string(m_count),10,50,40,assets->get<Font>("consolas"),{1,1,1,1} };
		ui::Draw(partCount);
	}
	Particle* ParticleGroup::createParticles(int count, bool setZero) {
		if (m_capacity - m_count < count) {
			bool yes = resize(m_capacity + count); // resize won't grow
			if (!yes) return nullptr;
		}
		m_updateBuffer = true;

		Particle* out = m_particles + m_count;

		if (setZero) {
			memset(m_particles, 0, count * sizeof(Particle));
		}
		m_count += count;
		return out;
	}
	bool ParticleGroup::createCube(glm::vec3 position, glm::vec3 scale, uint32_t particleCount) {
		Particle* parts = createParticles(particleCount);
		if (parts) {
			for (int i = 0; i < particleCount; i++) {
				float x = GetRandom();
				float y = GetRandom();
				float z = GetRandom();
				//float x = 0;
				//float y = 0;
				//float z = 0;
				parts[i].pos = { position.x + scale.x * (x - 0.5), position.y + scale.y * (y - 0.5), position.z + scale.z * (z - 0.5) };
				parts[i].vel = { 0, 0, 0 };
			}
			return true;
		}
		return false;
	}
	Particle* ParticleGroup::getParticles() {
		m_shaderBuffer.getData(m_capacity * sizeof(Particle), m_particles);
		m_updateBuffer = true;
		return m_particles;
	}
	void ParticleGroup::clear() {
		m_count = 0;
	}
	void ParticleGroup::cleanup() {
		resize(0);
	}
	bool ParticleGroup::resize(int count) {
		m_updateBuffer = true;
		if (count == 0) {
			if (m_particles)
				free(m_particles);

			m_shaderBuffer.cleanup();
			int capacity = 0;
			int count = 0;

			m_particles = nullptr;
		}
		else {
			if (!m_particles) {
				Particle* parts = (Particle*)alloc::_malloc(count * sizeof(Particle));
				if (!parts) {
					log::out << log::RED << "ParticleModule::resize - failed!\n";
					return false;
				}
				m_shaderBuffer.setData(count*sizeof(Particle),nullptr);

				m_particles = parts;
				m_capacity = count;
				m_count = 0;
			}
			else {
				Particle* parts = (Particle*)alloc::_realloc(m_particles, m_capacity * sizeof(Particle), count * sizeof(Particle));
				if (!parts) {
					log::out << log::RED << "ParticleModule::resize - failed!\n";
					return false;
				}
				m_shaderBuffer.setData(count * sizeof(Particle), nullptr);
				m_particles = parts;
				m_capacity = count;

				if (count < m_count)
					m_count = count;
			}
		}
		return true;
	}

	//ParticleModule::~ParticleModule() {
	//	cleanup();
	//}
	//static void createSphere() {
	//	ParticleData data = createParticles(ParticleModule::PARTICLE_BATCH);

	//	if (data.count != 0) {
	//		glm::vec3 bounds = { 5,5,5 };
	//		glm::vec3 center = { 0,0,0 };

	//		int row = pow(data.count, 1. / 3);
	//		for (int i = 0; i < data.count; i++) {
	//			//float x = fmod(i,row)/(float)row;
	//			//float y = fmod(i/row,row)/(float)row;
	//			//float z = (i/row/row)/(float)row;
	//			//float x = (float)rand()/RAND_MAX;
	//			//float y = (float)rand() / RAND_MAX;
	//			//float z = (float)rand() / RAND_MAX;
	//			float x = GetRandom();
	//			float y = GetRandom();
	//			float z = GetRandom();

	//			data.vert[i].pos = { center.x - bounds.x * x, center.y - bounds.y * y, center.z - bounds.z * z };
	//			//data.vert[i].pos = { center.x-bounds.x *GetRandom(), center.y - bounds.y * GetRandom(), center.z - bounds.z * GetRandom()};
	//			data.vert[i].vel = { 0, 0, 0 };
	//			//data.vert[i].color = data.vert[i].pos;
	//			//data.info[i].life = i*0.00001f;
	//			//glm::vec3 ran = { GetRandom(), GetRandom(), GetRandom() };
	//			//data.info[i].velocity = data.vert[i].pos/160.f+ran*0.1f;
	//			//data.vert[i].vel = data.vert[i].pos/160.f+ran*0.1f;
	//			//glm::vec3 diff = data.vert[i].pos - center;
	//			//float mul = 5-glm::dot(diff, diff)/(3*3.f);
	//			//data.info[i].velocity *= mul;
	//			//data.info[i].velocity *= mul;
	//		}
	//	}
	//}
	//static void createCube(ParticleCollection collection) {
	//	ParticleData data = createParticles(PARTICLE_BATCH);
	//	if (data.count != 0) {
	//		glm::vec3 bounds = { 5,5,5 };
	//		glm::vec3 center = {0,0,0};
	//				
	//		int row = pow(data.count, 1. / 3);
	//		for (int i = 0; i < data.count; i++) {
	//			//float x = fmod(i,row)/(float)row;
	//			//float y = fmod(i/row,row)/(float)row;
	//			//float z = (i/row/row)/(float)row;
	//			//float x = (float)rand()/RAND_MAX;
	//			//float y = (float)rand() / RAND_MAX;
	//			//float z = (float)rand() / RAND_MAX;
	//			float x = GetRandom();
	//			float y = GetRandom();
	//			float z = GetRandom();
	//	
	//			data.vert[i].pos = { center.x-bounds.x *x, center.y - bounds.y * y, center.z - bounds.z * z};
	//			//data.vert[i].pos = { center.x-bounds.x *GetRandom(), center.y - bounds.y * GetRandom(), center.z - bounds.z * GetRandom()};
	//			data.vert[i].vel = { 0, 0, 0 };
	//			//data.vert[i].color = data.vert[i].pos;
	//			//data.info[i].life = i*0.00001f;
	//			//glm::vec3 ran = { GetRandom(), GetRandom(), GetRandom() };
	//			//data.info[i].velocity = data.vert[i].pos/160.f+ran*0.1f;
	//			//data.vert[i].vel = data.vert[i].pos/160.f+ran*0.1f;
	//			//glm::vec3 diff = data.vert[i].pos - center;
	//			//float mul = 5-glm::dot(diff, diff)/(3*3.f);
	//			//data.info[i].velocity *= mul;
	//			//data.info[i].velocity *= mul;
	//		}
	//	}
	//}
	//void ParticleModule::init(Window* window) {
	//	if (m_parent) {
	//		log::out << log::RED << "ParticleModule::init - cannot initialize twice\n";
	//		return;
	//	}
	//	m_parent = window;
	//	if (!window) {
	//		log::out << log::RED << "ParticleModule::init - window was nullptr\n";
	//		return;
	//	}
	//	Assets* assets = window->getAssets();
	//	assets->set<Shader>("particle", new Shader(particleGLSL));

	//	bool yes = resize(PARTICLE_BATCH);
	//	if (!yes) return;

	//	ParticleData data = createParticles(PARTICLE_BATCH);

	//	if (data.count != 0) {
	//		glm::vec3 bounds = { 5,5,5 };
	//		glm::vec3 center = {0,0,0};
	//		
	//		int row = pow(data.count, 1. / 3);
	//		for (int i = 0; i < data.count; i++) {
	//			//float x = fmod(i,row)/(float)row;
	//			//float y = fmod(i/row,row)/(float)row;
	//			//float z = (i/row/row)/(float)row;
	//			//float x = (float)rand()/RAND_MAX;
	//			//float y = (float)rand() / RAND_MAX;
	//			//float z = (float)rand() / RAND_MAX;
	//			float x = GetRandom();
	//			float y = GetRandom();
	//			float z = GetRandom();

	//			data.vert[i].pos = { center.x-bounds.x *x, center.y - bounds.y * y, center.z - bounds.z * z};
	//			//data.vert[i].pos = { center.x-bounds.x *GetRandom(), center.y - bounds.y * GetRandom(), center.z - bounds.z * GetRandom()};
	//			data.vert[i].vel = { 0, 0, 0 };
	//			//data.vert[i].color = data.vert[i].pos;
	//			//data.info[i].life = i*0.00001f;
	//			//glm::vec3 ran = { GetRandom(), GetRandom(), GetRandom() };
	//			//data.info[i].velocity = data.vert[i].pos/160.f+ran*0.1f;
	//			//data.vert[i].vel = data.vert[i].pos/160.f+ran*0.1f;
	//			//glm::vec3 diff = data.vert[i].pos - center;
	//			//float mul = 5-glm::dot(diff, diff)/(3*3.f);
	//			//data.info[i].velocity *= mul;
	//			//data.info[i].velocity *= mul;
	//		}
	//	}
	//}
	//ParticleData ParticleModule::createParticles(int count, bool setZero) {
	//	if (m_capacity-m_count < count) {
	//		bool yes = resize((m_capacity + count) * 2);
	//		if (!yes) return { 0,nullptr,nullptr };
	//	}
	//	
	//	ParticleData out = {count, m_particles + m_count, m_particleInfos + m_count };

	//	if (setZero) {
	//		memset(m_particles, 0, count*sizeof(ParticleVertex));
	//		memset(m_particleInfos, 0, count*sizeof(ParticleInfo));
	//	}
	//	m_count += count;
	//	return out;
	//}
	//void ParticleModule::update(UpdateInfo& info) {
	//	//for (int i = 0;i < m_count; i++) {
	//	//	m_particleInfos[i].life -= info.timeStep;

	//	//	if (m_particleInfos[i].life <= 0) {
	//	//		//m_particles[i].color = { 1,1,1 };
	//	//		m_particles[i] = m_particles[m_count - 1];
	//	//		m_particleInfos[i] = m_particleInfos[m_count - 1];
	//	//		m_count--;
	//	//		i--;
	//	//		continue;
	//	//	}

	//	//	//m_particles[i].pos += m_particleInfos[i].velocity*info.timeStep;
	//	//}
	//	//m_updateBuffer = true;
	//}
	//void ParticleModule::render(RenderInfo& info) {
	//	if (m_count == 0||!m_parent) return;

	//	//EnableDepth();
	//	EnableBlend();
	//	Assets* assets = m_parent->getAssets();
	//	Renderer* renderer = m_parent->getRenderer();
	//	Shader* shad = assets->get<Shader>("particle");
	//	if (shad) {
	//		shad->bind();
	//		renderer->updateProjection(shad);

	//		bool shouldSet = m_count>PARTICLE_BATCH||m_updateBuffer;
	//		// will be false if one batch draw is enough and particles hasn't been updated. Good for performance.
	//		int remaining = m_count;
	//		while (remaining > 0) {
	//			int drawPoints = std::min(remaining,PARTICLE_BATCH);

	//			if (m_initiatedBuffers) {
	//				if (shouldSet) {
	//					//m_particleSB.setData(drawPoints * sizeof(ParticleVertex) / sizeof(char), (char*)(m_particles + m_count - remaining));
	//					//m_particleSB.setData(drawPoints * sizeof(ParticleVertex) / sizeof(char), (char*)(m_particles + m_count - remaining));
	//				}
	//			} else {
	//				m_initiatedBuffers = true;
	//				//m_particleSB.setData(PARTICLE_BATCH * sizeof(ParticleVertex) / sizeof(char), (char*)(m_particles + m_count - remaining));
	//				//m_particleSB.setData(PARTICLE_BATCH * sizeof(ParticleVertex) / sizeof(char), (char*)(m_particles + m_count - remaining));
	//			}

	//			m_particleSB.drawPoints(m_count, 0);

	//			remaining -= drawPoints;
	//		}
	//		m_updateBuffer = false;
	//	}
	//	ui::TextBox partCount = { "Particles: " + std::to_string(m_count),10,50,40,assets->get<Font>("consolas"),{1,1,1,1}};
	//	ui::Draw(partCount);
	//}
	//void ParticleModule::clear() {
	//	m_count = 0;
	//}
	//void ParticleModule::cleanup() {
	//	resize(0);
	//}
	//bool ParticleModule::resize(int count) {
	//	if (m_particles) {
	//		return false;
	//	}
	//	m_particles = new Particle[count];
	//	return true;
	//	//if (count == 0) {
	//	//	if (m_particles)
	//	//		free(m_particles);
	//	//	if (m_particleInfos)
	//	//		free(m_particleInfos);

	//	//	int capacity = 0;
	//	//	int count = 0;

	//	//	m_particles = nullptr;
	//	//	m_particleInfos = nullptr;
	//	//} else {
	//	//	if (!m_particles) {
	//	//		ParticleVertex* parts = (ParticleVertex*)alloc::_malloc(count*sizeof(ParticleVertex));
	//	//		if (!parts) {
	//	//			log::out << log::RED << "ParticleModule::resize - failed!\n";
	//	//			return false;
	//	//		}
	//	//		ParticleInfo* infos = (ParticleInfo*)alloc::_malloc(count*sizeof(ParticleInfo));
	//	//		if (!infos) {
	//	//			log::out << log::RED << "ParticleModule::resize - failed!\n";
	//	//			delete parts;
	//	//			return false;
	//	//		}

	//	//		m_particles = parts;
	//	//		m_particleInfos = infos;
	//	//		m_capacity = count;
	//	//		m_count = 0;
	//	//	} else {
	//	//		ParticleVertex* parts = (ParticleVertex*)alloc::_realloc(m_particles,m_capacity*sizeof(ParticleVertex), count * sizeof(ParticleVertex));
	//	//		if (!parts) {
	//	//			log::out << log::RED << "ParticleModule::resize - failed!\n";
	//	//			return false;
	//	//		}
	//	//		ParticleInfo* infos = (ParticleInfo*)alloc::_realloc(m_particleInfos,m_capacity*sizeof(ParticleInfo), count * sizeof(ParticleInfo));
	//	//		if (!infos) {
	//	//			// parts succeeded while info didn't. They don't have the same capacity so shrink parts.
	//	//			m_particles = (ParticleVertex*)alloc::_realloc(m_particles, count * sizeof(ParticleVertex), m_capacity*sizeof(ParticleVertex));
	//	//			log::out << log::RED << "ParticleModule::resize - failed!\n";
	//	//			return false;
	//	//		}
	//	//		m_particles = parts;
	//	//		m_particleInfos = infos;
	//	//		m_capacity = count;

	//	//		if (count < m_count)
	//	//			m_count = count;
	//	//	}
	//	//}
	//	//return true;
	//}
}