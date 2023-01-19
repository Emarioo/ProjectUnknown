#include "ProUnk/Effects/VisualEffects.h"

#include "Engone/Window.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	void VisualEffects::CreateTextParticle(glm::vec3 position, float scale, const std::string& text) {

	}
	float s_time = 0;
	void VisualEffects::update(engone::LoopInfo & info){
		using namespace engone;
		s_time += info.timeStep;
		for (int i = 0; i < m_floatingTexts.size();i++) {
			FloatingText& text = m_floatingTexts[i];
			
			text.position += text.velocity * (float)info.timeStep;
			text.lifetime -= info.timeStep;

			if (text.lifetime <= 0) {
				m_floatingTexts.erase(m_floatingTexts.begin() + i);
				i--;
			}
		}
		for (int i = 0; i < m_damageParticles.size(); i++) {
			DamageParticle& part = m_damageParticles[i];

			part.position += part.velocity * (float)info.timeStep;
			part.lifetime -= info.timeStep;
			part.size -= info.timeStep * GetRandom() * 7;

			if (part.lifetime <= 0||part.size<=0) {
				m_damageParticles.erase(m_damageParticles.begin() + i);
				i--;
			}
		}
	}
	void VisualEffects::render(engone::LoopInfo& info) {
		using namespace engone;
		EnableBlend();
		//EnableDepth();

		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		CommonRenderer* renderer = GET_COMMON_RENDERER();

		auto& viewMatrix = renderer->getCamera()->getViewMatrix();
		auto& persMatrix = renderer->getPerspective();
		glm::mat4 projMat = (persMatrix * viewMatrix);

		//{
		//	glm::vec4 pos = glm::vec4(0, 4, 0, 1);
		//	glm::vec4 final = projMat * pos;

		//	if (final.z > 0) {
		//		float h = 100 / final.z;
		//		float w = consolas->getWidth("Hello", h);

		//		ShaderAsset* shader = info.window->getStorage()->get<ShaderAsset>("gui");
		//		shader->bind();

		//		shader->setVec2("uPos", { GetWidth() * (0.5 + final.x / final.z / 2) - w / 2, GetHeight() * (0.5 - final.y / final.z / 2) - h / 2 });
		//		shader->setVec2("uSize", { 1,1 });
		//		shader->setVec4("uColor", { 0, 0, 0, 1 });

		//		renderer->drawString(consolas, "Hello", h, -1);
		//	}
		//}
		ShaderAsset* shader = info.window->getStorage()->get<ShaderAsset>("gui");
		shader->bind();
		consolas->texture.bind(0);
		shader->setVec2("uSize", { 1,1 });
		shader->setInt("uColorMode", 1);
		for (int i = 0; i < m_floatingTexts.size(); i++) {
			FloatingText& text = m_floatingTexts[i];
			
			glm::vec4 pos = projMat * glm::vec4(text.position,1);
			if (pos.z > 0) {
				float h = text.height / pos.z;
				float w = consolas->getWidth(text.text, h);
				
				shader->setVec2("uPos", { 
					GetWidth() * (0.5 + pos.x / pos.z / 2) - w / 2,
					GetHeight() * (0.5 - pos.y / pos.z / 2) - h / 2 });

				const float fadeTime = 0.23;
				if (text.lifetime < fadeTime) {
					glm::vec4 temp = text.color;
					temp.a = text.lifetime / fadeTime;
					shader->setVec4("uColor", temp);
				} else
					shader->setVec4("uColor", text.color);

				renderer->drawString(consolas, text.text, h, -1);

				//ui::TextBox box = { text.text, 0,0,h,consolas,text.color };
				//float w = box.getWidth();
				//pos.x = GetWidth() * (0.5 + pos.x / pos.z / 2) - w / 2;
				//pos.y = GetHeight() * (0.5 - pos.y / pos.z / 2) - h / 2;
				//ui::Draw(box);
			}
		}

		//-- Creature names
		Session* session = ((GameApp*)info.app)->getActiveSession();
		Dimension* dim = session->getDimension("0");
		EngineWorld* world = dim->getWorld();

		auto iterator = world->createIterator();
		EngineObject* obj=nullptr;
		int index = 1;
		while (obj = iterator.next()) {
			if (obj->getObjectType() & OBJECT_CREATURE) {
				auto& oinfo = session->objectInfoRegistry.getCreatureInfo(obj->getObjectInfo());
				
				if (!oinfo.displayName.empty()) {
					glm::vec3 pos3 = obj->getPosition() + glm::vec3(0, obj->getModel()->boundingPoint.y + obj->getModel()->boundingRadius, 0);
					//obj->getModel()->boundingPoint
					// use model asset to get bounds and offset text position by?
					glm::vec4 pos = projMat * glm::vec4(pos3, 1);
					//std::string name = "Player " + std::to_string(index);
					index++;
					float th = 70;
					ui::Color col = { 1 };
					if (pos.z > 0) {
						float h = th / pos.z;
						float w = consolas->getWidth(oinfo.displayName, h);

						float x = GetWidth() * (0.5 + pos.x / pos.z / 2) - w / 2;
						float y = GetHeight() * (0.5 - pos.y / pos.z / 2) - h / 2;
						const float paddingBack = 3;
						const float paddingHealth = 1;
						shader->setVec4("uColor", { 0,0,0,0.5 });
						shader->setVec2("uPos", { x - paddingBack, y - paddingBack });
						//shader->setVec2("uPos", { 50, 200 });
						shader->setVec2("uSize", { w + paddingBack * 2, h + paddingBack * 2 });
						//shader->setVec2("uSize", {50, 50});
						shader->setInt("uColorMode", 0);
						renderer->drawQuad(info);

						float healthP = oinfo.combatData.health/oinfo.combatData.getMaxHealth();

						shader->setVec4("uColor", { 1.f,0,0,0.5 });
						shader->setVec2("uPos", { x - paddingHealth, y - paddingHealth });
						//shader->setVec2("uPos", { 50, 200 });
						shader->setVec2("uSize", { (w + paddingHealth * 2)* healthP, h + paddingHealth * 2 });
						//shader->setVec2("uSize", {50, 50});
						shader->setInt("uColorMode", 0);
						renderer->drawQuad(info);

						shader->setInt("uColorMode", 1);
						shader->setVec2("uSize", { 1, 1 });
						shader->setVec2("uPos", { x, y });
						shader->setVec4("uColor", col.toVec4());

						renderer->drawString(consolas, oinfo.displayName, h, -1);
					}
				}
			}
			//if (obj->getObjectType() == OBJECT_PLAYER) {
			//	glm::vec3 pos3 = obj->getPosition() + glm::vec3(0, obj->getModel()->boundingPoint.y+obj->getModel()->boundingRadius, 0);
			//	//obj->getModel()->boundingPoint
			//	// use model asset to get bounds and offset text position by?
			//	glm::vec4 pos = projMat * glm::vec4(pos3, 1);
			//	std::string name = "Player " + std::to_string(index);
			//	index++;
			//	float th = 80;
			//	ui::Color col = { 1 };
			//	if (pos.z > 0) {
			//		float h = th / pos.z;
			//		float w = consolas->getWidth(name, h);

			//		float x = GetWidth() * (0.5 + pos.x / pos.z / 2) - w / 2;
			//		float y = GetHeight() * (0.5 - pos.y / pos.z / 2) - h / 2;
			//		float padding = 2;
			//		shader->setVec4("uColor", {0,0,0,0.5});
			//		shader->setVec2("uPos", { x- padding, y- padding });
			//		//shader->setVec2("uPos", { 50, 200 });
			//		shader->setVec2("uSize", {w+ padding*2, h+ padding *2});
			//		//shader->setVec2("uSize", {50, 50});
			//		shader->setInt("uColorMode", 0);
			//		renderer->drawQuad(info);

			//		shader->setInt("uColorMode", 1);
			//		shader->setVec2("uSize", {1, 1 });
			//		shader->setVec2("uPos", {x, y });
			//		shader->setVec4("uColor", col.toVec4());

			//		renderer->drawString(consolas, name, h, -1);
			//	}
			//}
		}

		TextureAsset* textureAss = info.window->getStorage()->load<TextureAsset>("textures/effects/damage2");
		textureAss->texture.bind();

		for (int i = 0; i < m_damageParticles.size(); i++) {
			DamageParticle& part = m_damageParticles[i];
			glm::vec4 pos = projMat * glm::vec4(m_damageParticles[i].position, 1);

			if (pos.z > 0) {
				float s = part.size / pos.z;

				float x = GetWidth() * (0.5 + pos.x / pos.z / 2) - s / 2;
				float y = GetHeight() * (0.5 - pos.y / pos.z / 2) - s / 2;
				shader->setVec4("uColor", { 1,1,1,1 });
				shader->setVec2("uPos", { x, y });
				shader->setVec2("uSize", { s, s });
				shader->setInt("uColorMode", 1);
				renderer->drawQuad(info);
			}
		}
	}
	void VisualEffects::addDamageNumber(const std::string& text, glm::vec3 pos, glm::vec4 color) {
		using namespace engone;
		m_floatingTexts.push_back({});
		auto& t = m_floatingTexts.back();
		t.text = text;
		t.position = pos;
		t.velocity = {(GetRandom()-0.5)*0.15,0.4+GetRandom() * 0.25,(GetRandom()-0.5) * 0.15 };
		t.color = color;
		t.height = 70;
		t.lifetime = 1.3 + GetRandom() * 0.6;
	}
	void VisualEffects::addDamageParticle(glm::vec3 position) {
		using namespace engone;
		m_damageParticles.push_back({});
		auto& t = m_damageParticles.back();
		t.lifetime = 0.5 + GetRandom() * 1;
		t.size = 55 + 43*GetRandom();
		t.position = position;
		//+glm::vec3((GetRandom() - 0.5) * 0.25, (GetRandom() - 0.5) * 0.25, (GetRandom() - 0.5) * 0.25);
		//t.velocity = { (GetRandom() - 0.5),(GetRandom()-0.5),(GetRandom() - 0.5) };
		t.velocity = { sinf(s_time),(GetRandom()-0.5)*0.1,cosf(s_time)};
		//t.velocity = { sinf(s_time),sinf(s_time)+cosf(s_time),cosf(s_time)};
		t.velocity *= 1;
	}
}