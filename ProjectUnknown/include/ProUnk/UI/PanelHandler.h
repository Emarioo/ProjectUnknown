#pragma once

#include "Engone/LoopInfo.h"

#include "Engone/Logger.h"

#include "Engone/RenderModule.h"

namespace prounk {

	class PanelHandler;
	class Panel {
	public:
		Panel() = default;

		virtual void render(engone::LoopInfo& info) = 0;
		
		void setDepth(float depth);
		float getDepth();
		bool isMovable();
		void setMovable(bool yes);

		void setSize(int w, int h);
		void setPosition(int x, int y);

	protected:
		engone::ui::Box area;
		//float panelX,panelY,panelW,panelH;

	private:
		bool m_movable=true; // Some panels like MasterInventoryPanel cannot be moved.
		
		float m_depth=0; // panels are sorted by this

		PanelHandler* m_panelHandler=nullptr; // used when updating depth

		friend class PanelHandler;
	};

	class PanelHandler {
	public:
		PanelHandler() = default;

		void addPanel(Panel* panel);

		void render(engone::LoopInfo& info);

		// drag panels
		void checkInput();

		void setCanMovePanels(bool yes);

	private:

		bool m_canMovePanels = false;

		Panel* m_editPanel = nullptr;
		float m_editFromX=0;
		float m_editFromY=0;

		int m_editResizingX = 0;
		int m_editResizingY = 0;

		int m_editType = 0;
		static const int EDIT_MOVE=0;
		static const int EDIT_RESIZE=1;

		bool m_pendingSort = false;
		std::vector<Panel*> m_panels;

		// sort panels by depth
		void sortPanels();

		friend class Panel;
	};

}