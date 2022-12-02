#include "ProUnk/UI/PanelHandler.h"

#include "Engone/EventModule.h"

#include "Engone/Window.h"

namespace prounk {

	float Panel::getDepth() {
		return m_depth;
	}
	void Panel::setDepth(float depth) {
		m_depth = depth;
		if (m_panelHandler)
			m_panelHandler->m_pendingSort = true;
		else
			engone::log::out << engone::log::RED << "Panel::setDepth - handler not set\n";
	}
	void Panel::setMovable(bool yes) {
		m_movable = yes;
	}
	bool Panel::isMovable() {
		return m_movable;
	}
	void Panel::setSize(int w, int h) {
		area.w = w;
		area.h = h;
	}
	void Panel::setPosition(int x, int y) {
		area.x = x;
		area.y = y;
	}
	void PanelHandler::addPanel(Panel* panel) {
		m_panels.push_back(panel);
		m_pendingSort = true;
	}
	void PanelHandler::sortPanels() {
		// Simple bubble sort. There will probably not be more than 100 panels.
		for (int i = 0; i < m_panels.size();i++) {
			for (int j = 0; j < m_panels.size()-1-i; j++) {
				if (m_panels[j]->getDepth()>m_panels[j+1]->getDepth()) {
					Panel* temp = m_panels[j];
					m_panels[j] = m_panels[j + 1];
					m_panels[j + 1] = temp;
				}
			}
		}
		m_pendingSort = false;
	}
	void PanelHandler::render(engone::LoopInfo& info) {
		if (m_pendingSort)
			sortPanels();

		checkInput();
		for (int i = m_panels.size()-1; i >= 0; i--) {
			// first panel has lowest depth, last has highest. (if list is sorted)
			m_panels[i]->render(info);
		}
	}
	void PanelHandler::setCanMovePanels(bool yes) {
		m_canMovePanels = yes;
	}
	void PanelHandler::checkInput() {
		using namespace engone;

		// if click was successful then no other clicks on other panels will be registered
		if (m_canMovePanels) {
			if (!m_editPanel) {
				for (int i = 0; i < m_panels.size(); i++) {
					Panel* panel = m_panels[i];
					if (!panel->isMovable())
						continue;
					int clicked = ui::Clicked(panel->area);
					int clicked2 = ui::Clicked(panel->area, GLFW_MOUSE_BUTTON_2);
					if (clicked == 1||clicked2==1) {
						m_editPanel = panel;
						m_editFromX = panel->area.x-GetMouseX();
						m_editFromY = panel->area.y-GetMouseY();
						if (clicked) {
							m_editType = EDIT_MOVE;
						}
						if (clicked2) {
							m_editType = EDIT_RESIZE;
							m_editResizingX = 0;
							m_editResizingY = 0;
						}
					}
				}
			} else {
				if (m_editType == EDIT_MOVE) {
					m_editPanel->area.x = m_editFromX + GetMouseX();
					m_editPanel->area.y = m_editFromY + GetMouseY();

					// clamp
					int margin = 5;
					if (m_editPanel->area.x < margin) m_editPanel->area.x = margin;
					if (m_editPanel->area.x + m_editPanel->area.w >= GetWidth() - margin)
						m_editPanel->area.x = GetWidth() - margin - m_editPanel->area.w;
					if (m_editPanel->area.y < margin) m_editPanel->area.y = margin;
					if (m_editPanel->area.y + m_editPanel->area.h >= GetHeight() - margin)
						m_editPanel->area.y = GetHeight() - margin - m_editPanel->area.h;

					if (IsKeyReleased(GLFW_MOUSE_BUTTON_1)) {
						m_editPanel = nullptr;
					}
				} else if (m_editType == EDIT_RESIZE) {

					float mx = GetMouseX();
					float my = GetMouseY();

					int margin = 5;
					int minWidth = 20;
					int minHeight = 20;

					if (m_editResizingX == 0) {
						if (m_editPanel->area.x + m_editPanel->area.w < mx) {
							m_editResizingX = 1;
						}
						if (m_editPanel->area.x > mx) {
							m_editResizingX = -1;
						}
					} else {
						if (m_editResizingX==1) {
							m_editPanel->area.w = mx - m_editPanel->area.x;
							// clamp
							if (m_editPanel->area.w < minWidth) {
								m_editResizingX = 0;
								m_editPanel->area.w = minWidth;
							}
							//if (m_editPanel->area.x + m_editPanel->area.w >= GetWidth() - margin)
							//	m_editPanel->area.w = GetWidth() - margin - m_editPanel->area.x;
						} else {
							float fixed = m_editPanel->area.x + m_editPanel->area.w;
							m_editPanel->area.w = fixed - mx;
							m_editPanel->area.x = mx;
							
							// clamp
							if (m_editPanel->area.w<minWidth) {
								m_editPanel->area.w = minWidth;
								m_editPanel->area.x = fixed - minWidth;
								m_editResizingX = 0;
							}
						}
					}
					if (m_editResizingY == 0) {
						if (m_editPanel->area.y + m_editPanel->area.h < my) {
							m_editResizingY = 1;
						}
						if (m_editPanel->area.y > my) {
							m_editResizingY = -1;
						}
					} else {
						if (m_editResizingY==1) {
							m_editPanel->area.h = my - m_editPanel->area.y;
							// clamp
							if (m_editPanel->area.h < minHeight) {
								m_editResizingY = -1;
								m_editPanel->area.h = minHeight;
							}
						} else {
							float fixed = m_editPanel->area.y + m_editPanel->area.h;
							m_editPanel->area.h = fixed - my;
							m_editPanel->area.y = my;

							if (m_editPanel->area.h < minHeight) {
								m_editPanel->area.h = minHeight;
								m_editPanel->area.y = fixed - minHeight;
								m_editResizingY = 0;
							}
						}
					}
					



					if (IsKeyReleased(GLFW_MOUSE_BUTTON_2)) {
						m_editPanel = nullptr;
					}
				}
			}
		}

		if (IsKeyPressed(GLFW_KEY_P)) {
			m_canMovePanels = !m_canMovePanels;
			m_editPanel = nullptr;
		}
	}
}