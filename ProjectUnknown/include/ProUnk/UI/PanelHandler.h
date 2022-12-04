#pragma once

#include "Engone/LoopInfo.h"

//#include "Engone/Logger.h"

#include "Engone/Window.h"

namespace prounk {

	class PanelHandler;
	class Panel {
	public:
		Panel() = default;

		typedef int EdgeType;
		static const EdgeType TOP=0;
		static const EdgeType LEFT=1;
		static const EdgeType BOTTOM=2;
		static const EdgeType RIGHT=3;

		virtual void render(engone::LoopInfo& info) = 0;
		
		void setDepth(float depth);
		float getDepth();
		bool isMovable();
		void setMovable(bool yes);

		// will update values of the constraints
		//void setX(float f);
		//void setY(float f);
		//void setW(float f);
		//void setH(float f);
		void setLeft(float f);
		void setRight(float f);
		void setTop(float f);
		void setBottom(float f);
		void setPosition(float x, float y);
		void setSize(float w, float h);
		engone::ui::Box getBox();

		struct Constraint {
			bool active = false;
			float minOffset = 0;
			float offset = 0; // the offset
			Panel* attached = nullptr; // null means attached to window
		};
		// This causes a chain reaction of constraint updates.
		// This is due to panels depending on each other's position
		void updateConstraints();

		// new type will replace old types
		void addConstraint(EdgeType type, float offset, float minOffset, Panel* attached);
		void removeConstraint(EdgeType type);

	protected:
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
				float m_height, m_width;
			};
		};
		float m_minWidth = 20;
		float m_minHeight = 20;

		void setMinWidth(float f);
		void setMinHeight(float f);

	private:
		bool m_movable=true; // Some panels like MasterInventoryPanel cannot be moved.
		
		float m_depth=0; // panels are sorted by this

		PanelHandler* m_panelHandler=nullptr; // used when updating depth

		Constraint m_constraints[4];
		std::vector<Panel*> m_attachments;

		friend class PanelHandler;
	};

	class PanelHandler {
	public:
		PanelHandler() = default;

		void addPanel(Panel* panel);

		void render(engone::LoopInfo& info);

		void setCanMovePanels(bool yes);

	private:

		bool m_canMovePanels = true; // Todo: this should be false by default

		Panel* m_editPanel = nullptr;
		float m_editFromX=0;
		float m_editFromY=0;

		int m_editResizingX = 0;
		int m_editResizingY = 0;

		int m_editType = 0;
		static const int EDIT_MOVE=0;
		static const int EDIT_RESIZE=1;
		static const int EDIT_CONSTRAIN=2;

		engone::Listener* m_listener=nullptr;

		bool m_pendingSort = false;
		std::vector<Panel*> m_panels;

		//GameApp* m_app;

		// sort panels by depth
		void sortPanels();
		void checkInput();

		friend class Panel;
	};

}