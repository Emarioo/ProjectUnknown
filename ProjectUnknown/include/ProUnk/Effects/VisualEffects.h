#pragma once

#include "Engone/LoopInfo.h"

#include "Engone/RenderModule.h"

namespace prounk {
	
	class VisualEffects : public engone::RenderComponent {
	public:
		VisualEffects() : RenderComponent("VisualEffects") {}

		void update(engone::LoopInfo& info);
		void render(engone::LoopInfo& info) override;

		void CreateTextParticle(glm::vec3 position, float scale, const std::string& text);
	
		void addDamageNumber(const std::string& text, glm::vec3 pos, glm::vec4 color);
		
	private:

		struct FloatingText {
			std::string text;
			glm::vec3 position;
			float height=50;
			float lifetime=1;
			glm::vec4 color={0,0,0,1};
			glm::vec3 velocity={0,0,0};
		};

		// mutex?
		std::vector<FloatingText> m_floatingTexts;
		
	};
}