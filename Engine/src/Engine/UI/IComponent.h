#pragma once

namespace engine {
	class IComponent {
		IComponent();
		~IComponent();

		virtual void Update(float delta);
		virtual void Render();
	};
}