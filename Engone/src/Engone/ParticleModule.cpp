
#include "Engone/ParticleModule.h"

#include "Engone/Utilities/Utilities.h"

namespace engone {

//	static const char* particleGLSL = {
//#include "Engone/Shaders/particle.glsl"
//	};

	void ParticleGroupT::initT(Window* window, Shader* shader, int pSize, int iSize, Flags flags) {
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
		m_iSize = iSize;
		m_pSize = pSize;
		m_flags = flags;
	}
	//void ParticleGroup::update(engone::UpdateInfo& info) {
		//float delta = info.timeStep;
		//for (int i = 0; i < m_count; i++) {
		//	Particle& part = m_particles[i];

		//	glm::vec3& pos = part.pos;
		//	glm::vec3& vel = part.vel;
		//	glm::vec3& acc = part.acc;

		//	double velDist = sqrt(double(part.vel.x) * double(part.vel.x) + double(part.vel.y) * double(part.vel.y) + double(part.vel.z) * double(part.vel.z));
		//	////float velDist = glm::length(part.vel);

		//	//glm::vec3 focus = glm::vec3(0, 0, 0) - part.pos;
		//	//std::setprecision(10);
		//	//log::out << velDist << "\n";

		//	//double mul = velDist2 / radius2;

		//	//glm::vec3 acc = { focus.x * mul, focus.y * mul, focus.z * mul };

		//	glm::vec3 toFocus = - pos;
		//	double radius = sqrt(double(toFocus.x) * double(toFocus.x) + double(toFocus.y) * double(toFocus.y) + double(toFocus.z) * double(toFocus.z));
		//	std::cout << std::setprecision(10) << velDist <<" "<<radius << "\n";

		//	glm::vec3 nextAcc = toFocus * dot(vel, vel) / dot(toFocus, toFocus);

		//	//-- Velocity Verlet

		//	glm::vec3 velHalf = vel + 1 / 2.f * acc * delta;
		//	glm::vec3 nextPos = pos + velHalf * delta;
		//	glm::vec3 nextVel = velHalf + 1 /2.f * nextAcc * delta;

		//	part.pos = nextPos;
		//	part.vel = nextVel;
		//	part.acc = nextAcc;
		//	
		//	m_shaderBuffer.setData(m_count * sizeof(Particle), m_particles);
		//}
		//m_updateBuffer = true;
	//}
	void ParticleGroupT::render(engone::RenderInfo& info) {
		if (m_count == 0 || !m_parent || !m_shader) return;
		if (!m_shaderBuffer.initialized()) return;

		if(m_flags&AlphaBlend)
			EnableBlend();
		else
			EnableDepth();
		AssetStorage* assets = m_parent->getStorage();
		//Assets* assets = m_parent->getAssets();
		Renderer* renderer = m_parent->getRenderer();

		m_shader->bind();
		renderer->updateProjection(m_shader);
		//m_shader->setFloat("delta", info.timeStep);
		m_shader->setFloat("delta", info.timeStep);

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
			//int* aliveCount = nullptr;
			//int usedBytes = 0;
			//if (m_flags & InfoAliveCount) {
			//	if (m_iSize - usedBytes >= sizeof(int)) {
			//		aliveCount = (int*)((char*)m_data + usedBytes);
			//	}
			//	usedBytes -= sizeof(int);
			//}
			//if (aliveCount) {
			//	if (m_count != *aliveCount) {
			//		*aliveCount = m_count;
			//		m_refreshShaderInfo = true;
			//	}
			//}
			//*aliveCount = m_count;
			//if (m_refreshShaderInfo && m_refreshShaderData) {
			//	m_shaderBuffer.setData(m_iSize+m_count*m_pSize, m_data);
			//} else {
			//	if (m_refreshShaderInfo) {
			//		m_shaderBuffer.setData(m_iSize, m_data);
			//	}
				if (m_refreshShaderData) {
					//log::out << GetActiveWindow() << "\n";
					m_shaderBuffer.setData(m_count * m_pSize - m_iSize, (char*)m_data + m_iSize, m_iSize);
				}
			//}
			//glEnable(GL_PROGRAM_POINT_SIZE);
			//glEnable(GL_CLIP_DISTANCE0);
			//m_shader->setFloat("uPointSize", m_pointSize);
			//log::out << m_count << "\n";
			m_shaderBuffer.drawPoints(m_count, 0);
			if (m_iSize != 0) {
				//m_shaderBuffer.getData(m_iSize,m_data);
				//m_shaderBuffer.getData(m_capacity,m_data);
				//if (aliveCount) {
				//	for (int i = 0; i < m_count;i++) {
				//		float life = *(float*)((char*)m_data + m_iSize + i * m_pSize+sizeof(float)*6);
				//		log::out << life << "\n";
				//	}
				//	log::out << "alive "<<*aliveCount << "\n";
				//	//m_count = *aliveCount;
				//	//log::out << *aliveCount<<"\n";
				//}
			}
			m_refreshData = true;
		}

		m_refreshShaderData = false;
		m_refreshShaderInfo = false;

		//ui::TextBox partCount = { "Particles: " + std::to_string(m_count),10,50,40,assets->get<Font>("consolas"),{1,1,1,1} };
		//ui::Draw(partCount);
	}
	ParticleGroupT::ret ParticleGroupT::createParticlesT(int count, bool setZero) {
		if (m_pSize == 0) {
			log::out << "Particle size is zero, did you forget init?\n";
		}
		int left = m_capacity - m_iSize - m_count * m_pSize;
		if (left < count*m_pSize) {
			bool yes = resize(m_capacity + count*m_pSize); // resize won't grow
			if (!yes) return { nullptr,nullptr };
		}
		m_refreshShaderData = true;

		void* parts = (char*)m_data + m_iSize+m_count * m_pSize;
		void* info = nullptr;
		if (m_iSize != 0) {
			info = m_data;
			m_refreshShaderInfo = true;
		}

		if (setZero) {
			memset(m_data, 0, m_iSize+count * m_pSize);
		}
		m_count += count;
		return {info,parts};
	}
	bool ParticleGroupT::createCube(glm::vec3 position, glm::vec3 scale, uint32_t particleCount, glm::vec3 velocity) {
		if (m_pSize < 6) {
			log::out << log::RED << "ParticleGroup::createCube - struct is to small for two glm::vec3\n";
			return false;
		}
		ret ptrs = createParticlesT(particleCount);
		if (ptrs.parts) {
			for (int i = 0; i < particleCount; i++) {
				float x = GetRandom();
				float y = GetRandom();
				float z = GetRandom();
				//float x = 0;
				//float y = 0;
				//float z = 0;
				//parts[i].pos = {position.x + scale.x * (x - 0.5), position.y + scale.y * (y - 0.5), position.z + scale.z * (z - 0.5)};
				//parts[i].vel = velocity;
				*((glm::vec3*)((char*)ptrs.parts+i*m_pSize+0)) = { position.x + scale.x * (x - 0.5), position.y + scale.y * (y - 0.5), position.z + scale.z * (z - 0.5) };
				*((glm::vec3*)((char*)ptrs.parts + i * m_pSize + sizeof(glm::vec3))) = velocity;
			}
			return true;
		}
		return false;
	}
	void* ParticleGroupT::getParticlesT() {
		if (m_refreshData) {
			m_shaderBuffer.getData(m_capacity-m_iSize, m_data,m_iSize);
			m_refreshData = false;
			m_refreshShaderData = true;
		}
		return (char*)m_data+m_iSize;
	}
	void* ParticleGroupT::getInfoT() {
		if (m_iSize == 0)
			return nullptr;
		m_refreshShaderInfo = true;
		// always updated
		//if (m_refreshData) {
		//	m_shaderBuffer.getData(m_capacity-m_iSize, m_data,m_iSize);
		//	m_refreshData = false;
		//	m_refreshShader = true;
		//}
		return m_data;
	}
	void ParticleGroupT::clear() {
		m_count = 0;
	}
	void ParticleGroupT::cleanup() {
		resize(0);
	}
	bool ParticleGroupT::resize(int size) {
		m_refreshShaderData = true;
		m_refreshShaderInfo = true;
		if (size == 0) {
			if (m_data)
				alloc::free(m_data,m_capacity);

			m_shaderBuffer.cleanup();
			int capacity = 0;
			int count = 0;

			m_data = nullptr;
		}
		else {
			if (!m_data) {
				void* newData = (void*)alloc::malloc(size);
				if (!newData) {
					log::out << log::RED << "ParticleModule::resize - failed!\n";
					return false;
				}
				m_shaderBuffer.setData(size, nullptr);

				m_data = newData;
				m_capacity = size;
				m_count = 0;
			}
			else {
				void* newData = (void*)alloc::realloc(m_data, m_capacity, size);
				if (!newData) {
					log::out << log::RED << "ParticleModule::resize - failed!\n";
					return false;
				}
				m_shaderBuffer.setData(size, nullptr);
				m_data = newData;
				m_capacity = size;

				int count = (m_capacity - m_iSize)/m_pSize;
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