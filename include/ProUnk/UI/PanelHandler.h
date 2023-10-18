#pragma once

#include "Engone/LoopInfo.h"

//#include "Engone/Logger.h"

// #include "Engone/Window.h"
#include "Engone/Rendering/UIRenderer.h"

#include "Engone/PlatformModule/GameMemory.h"
#include "Engone/Window.h"

namespace prounk {
	class Panel;
	class PanelHandler;
	
	typedef void(*PanelUpdateProc)(engone::LoopInfo*, Panel*);
	class Panel {
	public:
		Panel();

		void move(float dx, float dy);
		void setLeft(float f);
		void setRight(float f);
		void setTop(float f);
		void setBottom(float f);

		void setPosition(float x, float y);
		void setSize(float w, float h);
		engone::ui::Box getBox();
		
		// out is a pointer with space for 4 floats in x,y,w,h order
		void retrieveXYWH(float out[4]);

		struct Constraint {
			bool active = false;
			float minOffset = 0;
			float offset = 0; // the offset
			Panel* attached = nullptr; // null means attached to window
		};
		// This causes a chain reaction of constraint updates.
		// This is due to panels depending on each other's position
		void updateConstraints();

		// edges of the panel. right >= left and bottom>=top is guaranteed.
		union {
			struct {
				float m_edges[4];
			};
			struct {
				// read-only (except for internal stuff)
				float m_top, m_left, m_bottom, m_right;
			};
		};
		union {
			struct {
				float m_sizes[2];
			};
			struct {
				float m_height;
				float m_width;
			};
		};
		float m_minWidth = 20;
		float m_minHeight = 20;
		float m_maxWidth = 900;
		float m_maxHeight = 900;

		void setMinWidth(float f);
		void setMinHeight(float f);
		void setMaxWidth(float f);
		void setMaxHeight(float f);
		
		bool hidden = false;
		float depth=0; // panels are sorted by this
		bool movable=true; // Some panels like MasterInventoryPanel cannot be moved.
		PanelUpdateProc updateProc = 0;
		void* extraData = 0;
		
		std::string name;

	private:
		PanelHandler* panelHandler=nullptr; // used when updating depth

		friend class PanelHandler;
	};

	class PanelHandler {
	public:
		PanelHandler() = default;
		
		void init(engone::Window* window);
		
		Panel* createPanel();

		void render(engone::LoopInfo* info);

		void setCanMovePanels(bool yes);
		
		Panel* getPanel(const char* name);

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
		static const int EDIT_CONSTRAIN=2;

		engone::Window* window=0;
		std::vector<Panel*> m_panels;

		// sort panels by depth
		void sortPanels();
		void checkInput();

		friend class Panel;
	};

}