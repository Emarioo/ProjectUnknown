#include "ProUnk/UI/PanelHandler.h"

#include "Engone/EventModule.h"

#include "Engone/Window.h"

namespace prounk {
	Panel::Panel() {
		// has to initialize here
		m_left = 0;
		m_top = 0;
		m_bottom = 0;
		m_right = 0;
		m_width = 0;
		m_height = 0;
	}
	float Panel::getDepth() {
		return m_depth;
	}
	void Panel::setDepth(float depth) {
		m_depth = depth;
		if (m_panelHandler) // false if not added to panelHandler. Depth is always sorted on add so all is well.
			m_panelHandler->m_pendingSort = true;
	}
	void Panel::setMovable(bool yes) {
		m_movable = yes;
	}
	bool Panel::isMovable() {
		return m_movable;
	}
	engone::ui::Box Panel::getBox() {
		return { m_left,m_top,m_right-m_left,m_bottom-m_top };
	}
	//void Panel::addConstraint(EdgeType type, float offset, float minOffset, Panel* attached) {
	//	Constraint& c = m_constraints[type];
	//	// remove old attachment
	//	if (c.attached) {
	//		Panel* p = c.attached;
	//		for (int i = 0; i < p->m_attachments.size(); i++) {
	//			if (this == p->m_attachments[i]) {
	//				p->m_attachments.erase(p->m_attachments.begin() + i);
	//				break;
	//			}
	//		}
	//		m_constraints[type].attached = nullptr;
	//	}

	//	c.active = true;
	//	c.offset = offset;
	//	c.minOffset = minOffset;
	//	c.attached = attached;
	//	if (c.attached) {
	//		c.attached->m_attachments.push_back(this);
	//	}

	//	updateConstraints();
	//}
	//void Panel::removeConstraint(EdgeType type) {
	//	Constraint& c = m_constraints[type];
	//	if (c.attached) {
	//		Panel* p = c.attached;
	//		for (int i = 0; i < p->m_attachments.size();i++) {
	//			if (this == p->m_attachments[i]) {
	//				p->m_attachments.erase(p->m_attachments.begin() + i);
	//				break;
	//			}
	//		}
	//		m_constraints[type].attached = nullptr;
	//	}

	//	m_constraints[type].active = false;

	//	updateConstraints();
	//}
	void Panel::setTop(float f) {
		using namespace engone;
		ui::Box area = getBox();
		//Constraint& c = m_constraints[TOP];

		if (f < 0) {
			m_top = 0;
		} else if (f > GetHeight()) {
			m_top = GetHeight();
		} else {
			m_top = f;
		}
		if (m_bottom - m_top > m_maxHeight) {
			m_top = m_bottom - m_maxHeight;
		}
		if (m_bottom - m_top < m_minHeight) {
			m_top = m_bottom - m_minHeight;
		}
		//m_height = fabs(m_bottom - f);
		//if (c.active) {
		//	if (c.attached) {
		//		c.offset = f - c.attached->m_bottom;
		//		if (c.offset < c.minOffset)
		//			c.offset = c.minOffset;
		//	}
		//} else {
			//m_top = f;
		//}
		//updateConstraints();
	}
	void Panel::setLeft(float f) {
		using namespace engone;
		ui::Box area = getBox();
		//Constraint& c = m_constraints[LEFT];

		if (f < 0) {
			m_left = 0;
		} else if (f > GetWidth()) {
			m_left = GetWidth(); // rare scenario, check if m_right is less than m_left?
		} else {
			m_left = f;
		}
		if (m_right-m_left > m_maxWidth) {
			m_left = m_right - m_maxWidth;
		}
		if (m_right - m_left < m_minWidth) {
			m_left = m_right - m_minWidth;
		}
		//m_width = fabs(m_right - f);
		//if (c.active) {
		//	if (c.attached) {
		//		c.offset = f - c.attached->m_right;
		//		if (c.offset < c.minOffset)
		//			c.offset = c.minOffset;
		//	}
		//} else {
			//m_left = f;
		//}
		//updateConstraints();
	}
	void Panel::setRight(float f) {
		using namespace engone;
		ui::Box area = getBox();
		//Constraint& c = m_constraints[RIGHT];

		if (f < 0) {
			m_right = 0; // m_left?
		} else if (f > GetWidth()) {
			m_right = GetWidth();
		} else {
			m_right = f; // m_left?
		}
		if (m_right - m_left > m_maxWidth) {
			m_right = m_left + m_maxWidth;
		}
		if (m_right - m_left < m_minWidth) {
			m_right = m_left + m_minWidth;
		}
		//m_width = fabs(f - m_left);
		//if (c.active) {
		//	if (c.attached) {
		//		c.offset = -f + c.attached->m_left;
		//		if (c.offset < c.minOffset)
		//			c.offset = c.minOffset;
		//	}
		//} else {
			//m_right = f;
		//}
		//updateConstraints();
	}
	void Panel::setBottom(float f) {
		using namespace engone;
		ui::Box area = getBox();
		//Constraint& c = m_constraints[BOTTOM];

		if (f < 0) {
			m_bottom = 0;
		} else if (f > GetHeight()) {
			m_bottom = GetHeight();
		} else {
			m_bottom = f;
		}
		if (m_bottom - m_top > m_maxHeight) {
			m_bottom = m_top + m_maxHeight;
		}
		if (m_bottom - m_top < m_minHeight) {
			m_bottom = m_top + m_minHeight;
		}
		//m_height = fabs(f - m_top);
		//if (c.active) {
		//	if (c.attached) {
		//		c.offset = -f + c.attached->m_top;
		//		if (c.offset < c.minOffset)
		//			c.offset = c.minOffset;
		//	}
		//} else {
			//m_bottom = f;
		//}
		//updateConstraints();
	}
	void Panel::setMinWidth(float f) {
		if (m_minWidth != f) {
			m_minWidth = f;
			updateConstraints();
		}
	}
	void Panel::setMinHeight(float f) {
		if (m_minHeight != f) {
			m_minHeight = f;
			updateConstraints();
		}
	}
	void Panel::setMaxWidth(float f) {
		if (m_maxWidth != f) {
			m_maxWidth = f;
			updateConstraints();
		}
	}
	void Panel::setMaxHeight(float f) {
		if (m_maxHeight != f) {
			m_maxHeight = f;
			updateConstraints();
		}
	}
	void Panel::setSize(float w, float h) {
		setRight(m_left + w);
		setBottom(m_top + h);
	}
	void Panel::setPosition(float x, float y) {
		move(x - m_left,y-m_top);
	}
	void Panel::move(float dx, float dy) {
		using namespace engone;
		if (m_left + dx < 0) {
			m_right = m_right - m_left;
			m_left = 0;
		}else if (m_right + dx > GetWidth()) {
			m_left = GetWidth() - (m_right-m_left);
			m_right = GetWidth();
		} else {
			m_left += dx;
			m_right += dx;
		}
		if (m_top + dy < 0) {
			m_bottom = m_bottom - m_top;
			m_top = 0;
		}else if (m_bottom + dy > GetHeight()) {
			m_top = GetHeight() - (m_bottom - m_top);
			m_bottom = GetHeight();
		} else {
			m_top += dy;
			m_bottom += dy;
		}
	}
	//void Panel::resize(float dx, float dy) {
	//	using namespace engone;
	//	if (dx < 0) {
	//		if (m_left + dx < 0) {
	//			m_left = 0;
	//		} else {
	//			m_left += dx;
	//		}
	//	} else if(dx>0) {
	//		if (m_right + dx > GetWidth()) {
	//			m_right = GetWidth();
	//		} else {
	//			m_right += dx;
	//		}
	//	}
	//	if (dy < 0) {
	//		if (m_top + dy < 0) {
	//			m_top = 0;
	//		} else {
	//			m_top += dy;
	//		}
	//	} else if (dy > 0) {
	//		if (m_top + dy > GetHeight()) {
	//			m_top = GetHeight();
	//		} else {
	//			m_bottom += dy;
	//		}
	//	}
	//}
	void Panel::updateConstraints() {
		using namespace engone;
		
		if (m_right - m_left > GetWidth()) {
			m_left = 0;
			m_right = GetWidth();
		}
		if (m_bottom - m_top > GetHeight()) {
			m_left = 0;
			m_right = GetWidth();
		}
		if (m_left < 0) {
			m_right = m_right - m_left;
			m_left = 0;
		}
		if (m_right > GetWidth()) {
			m_left = GetWidth()-(m_right-m_left);
			m_right = GetWidth();
		}
		if (m_top < 0) {
			m_bottom = m_bottom - m_top;
			m_top = 0;
		}
		if (m_bottom > GetHeight()) {
			m_top = GetHeight() - (m_bottom - m_top);
			m_bottom = GetHeight();
		}
		
		if (m_bottom - m_top > m_maxHeight) {
			m_bottom = m_top + m_maxHeight;
		}
		if (m_bottom - m_top < m_minHeight) {
			m_bottom = m_top + m_minHeight;
		}
		if (m_right - m_left > m_maxWidth) {
			m_right = m_left + m_maxWidth;
		}
		if (m_right - m_left < m_minWidth) {
			m_right = m_left + m_minWidth;
		}

		//if (m_top < 0)
		//	m_top = 0;
		//if (m_right > GetWidth())
		//	m_right = GetWidth(); // should it be GetWidth()-1 ?
		//if (m_bottom > GetHeight())
		//	m_bottom = GetHeight();

		m_width = m_right - m_left;
		m_height = m_bottom - m_top;
		//log::out <<"w/h: "<< m_width << " " << m_height << "\n";
	}
	//void Panel::updateConstraints() {
	//	using namespace engone;
	//	// size to maintain
	//	float prev[4]{ m_top,m_left, m_bottom,m_right };
	//	//float sizes[2]{ m_bottom-m_top,m_right-m_left };
	//	float limits[4]{ 0,0,GetHeight(),GetWidth() };

	//	// update constraints
	//	for (EdgeType type = 0; type < 4; type++) {
	//		Constraint& c = m_constraints[type];
	//		EdgeType type2 = type^2; // xor
	//		if (c.active) {
	//			if (c.attached) {
	//				m_edges[type] = c.attached->m_edges[type2];
	//				if (type & 2) m_edges[type] -= c.offset;
	//				else m_edges[type] += c.offset;
	//			} else {
	//				m_edges[type] = limits[type];
	//				if (type & 2) m_edges[type] -= c.offset;
	//				else m_edges[type] += c.offset;
	//			}
	//		}
	//		if (!m_constraints[type2].active) {
	//			// maintain previous width or height
	//			float size = m_sizes[type&1];
	//			//float size = m_sizes[type&1];

	//			m_edges[type2] = m_edges[type];
	//			if (type & 2) {
	//				m_edges[type2] -= size;
	//			} else {
	//				m_edges[type2] += size;
	//			}
	//		}
	//	}
	//	
	//	// check minimums, forced change like pushing repelling magnets together
	//	float w = m_right - m_left;
	//	if (w < m_minWidth) {
	//		//log::out << w<<" "<<m_right<<" "<<m_left<<" "<<prev[LEFT] << "\n";
	//		if (prev[LEFT] == m_left) {
	//			m_left = m_right - m_minWidth;
	//			if(m_constraints[LEFT].active)
	//				if(m_constraints[LEFT].attached)
	//					m_constraints[LEFT].offset = m_left - m_constraints[LEFT].attached->m_right;
	//			//log::out << "new left " << m_left << "\n";
	//		}
	//		if(prev[RIGHT] == m_right) {
	//			m_right = m_left + m_minWidth;
	//			if (m_constraints[RIGHT].active)
	//				if (m_constraints[RIGHT].attached)
	//					m_constraints[RIGHT].offset = -m_right + m_constraints[RIGHT].attached->m_left;
	//			//log::out << "new right " << m_right << "\n";
	//		}
	//	}
	//	float h = m_bottom - m_top;
	//	if(h < m_minHeight) {
	//		if (prev[TOP] == m_top) {
	//			m_top = m_bottom - m_minHeight;
	//			if (m_constraints[TOP].active)
	//				if (m_constraints[TOP].attached)
	//					m_constraints[TOP].offset = m_top - m_constraints[TOP].attached->m_bottom;
	//			//log::out << "new left " << m_left << "\n";
	//		}
	//		if (prev[BOTTOM] == m_bottom) {
	//			m_bottom = m_top + m_minHeight;
	//			if (m_constraints[BOTTOM].active)
	//				if (m_constraints[BOTTOM].attached)
	//					m_constraints[BOTTOM].offset = -m_bottom + m_constraints[BOTTOM].attached->m_top;
	//			//log::out << "new right " << m_right << "\n";
	//		}
	//		//if (prev[TOP] == m_top) {
	//			//m_bottom = m_top + m_minHeight;
	//		//} else {
	//		//	m_top = m_bottom - m_minHeight;
	//		//}
	//	}

		//m_width = fabs(m_right - m_left);
		//m_height = fabs(m_bottom - m_top);

	//	// update panels from the chain
	//	//for (Panel* p : m_attachments) {
	//	//	p->updateConstraints();
	//	//}
	//}
	void PanelHandler::addPanel(Panel* panel) {
		m_panels.push_back(panel);
		panel->m_panelHandler = this;
		m_pendingSort = true;
	}
	void PanelHandler::sortPanels() {
		using namespace engone;
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
		using namespace engone;
		if (m_pendingSort)
			sortPanels();

		//if (!m_listener) {
		//	m_listener = new engone::Listener(EventResize,[&](Event& e) {
		//		if (e.eventType == EventResize) {
		//			// scale panels
		//			for (int i = 0; i < m_panels.size(); i++) {
		//				Panel* panel = m_panels[i];
		//				//panel->area.x *= e.width / info.window->getWidth();
		//				//panel->area.y *= e.height / info.window->getHeight();
		//				//panel->area.w *= e.width / info.window->getWidth();
		//				//panel->area.h *= e.height / info.window->getHeight();

		//				// margins, minWidth and minHeight should be applied here.
		//				// It should be applied automatically though so you don't need to do this every time.
		//			}
		//			//log::out << "resize " << e.width << " " << e.height << "\n";
		//		}
		//		return EventNone;
		//	});
		//	info.window->attachListener(m_listener); // Todo: if window is destroyed and created again then the listener will not be attached anymore.
		//}

		checkInput();
		
		// These colors needs changing
		const ui::Color moveHighlight = { 1.0,1.0,0.1,1.f };
		const ui::Color darkHighlight = { 0.6,0.6,0.04,1.f };
		const ui::Color resizeHighlight = { 1.0,1.0,0.8,1.f };
		
		for (int i = 0; i < m_panels.size(); i++) {
			if (m_panels[i]->getHidden())
				continue;
			m_panels[i]->updateConstraints();
		}

		for (int i =0; i < m_panels.size(); i++) {
			if (m_panels[i]->getHidden())
				continue;
			// first panel has lowest depth, last has highest. (if list is sorted)
			if (m_panels[i] == m_editPanel) {
				float border = 2;
				ui::Box area = m_panels[i]->getBox();
				
				if (m_editType == EDIT_MOVE) {
					area.rgba = moveHighlight;
					area.x -= border;
					area.y -= border;
					area.w += border * 2;
					area.h += border * 2;
					ui::Draw(area);
				}else if(m_editType == EDIT_RESIZE) {
					area.rgba = darkHighlight;

					ui::Box frontx;
					frontx.rgba = resizeHighlight;
					frontx.w = border * 2; // *2 gives some extra width incase of gaps between boxes
					frontx.h = area.h;
					frontx.y = area.y;
					if(m_editResizingX==-1)
						frontx.x = area.x-border;
					else 
						frontx.x = area.x+area.w-border;

					ui::Box fronty;
					fronty.rgba = resizeHighlight;
					fronty.w = area.w;
					fronty.h = border * 2;
					fronty.x = area.x;
					if (m_editResizingY == -1)
						fronty.y = area.y - border;
					else
						fronty.y = area.y + area.h - border;

					if (m_editResizingX != 0 && m_editResizingY!=0) {
						if(m_editResizingY == -1)
							frontx.y -= border;
						frontx.h += border;
					}

					area.x -= border;
					area.y -= border;
					area.w += border * 2;
					area.h += border * 2;
					ui::Draw(area);
					if(m_editResizingX!=0)
						ui::Draw(frontx);
					if (m_editResizingY != 0)
						ui::Draw(fronty);
				}
			}
			m_panels[i]->render(info);
		}
	}
	void PanelHandler::setCanMovePanels(bool yes) {
		m_canMovePanels = yes;
	}
	void PanelHandler::checkInput() {
		using namespace engone;

		if (IsKeyDown(GLFW_KEY_LEFT_ALT)) {
			m_canMovePanels = true;
		} else {
			m_canMovePanels = false;
			m_editPanel = nullptr;
		}
		// if click was successful then no other clicks on other panels will be registered
		if (m_canMovePanels) {
			if (!m_editPanel) {
				for (int i = m_panels.size()-1; i >= 0; i--) {
					Panel* panel = m_panels[i];
					if (panel->getHidden())
						continue;
					if (!panel->isMovable())
						continue;
					ui::Box area = panel->getBox();
					int clicked = ui::Clicked(area);
					int clicked2 = ui::Clicked(area, GLFW_MOUSE_BUTTON_2);
					if (clicked == 1||clicked2==1) {
						m_editPanel = panel;
						m_editFromX = area.x-GetMouseX();
						m_editFromY = area.y-GetMouseY();
						if (IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
							if (clicked) {
								ResetKey(GLFW_MOUSE_BUTTON_1);
								m_editType = EDIT_CONSTRAIN;
							}
						} else {
							if (clicked) {
								ResetKey(GLFW_MOUSE_BUTTON_1);
								m_editType = EDIT_MOVE;
							}
							if (clicked2) {
								ResetKey(GLFW_MOUSE_BUTTON_2);
								m_editType = EDIT_RESIZE;
								m_editResizingX = 0;
								m_editResizingY = 0;
							}
						}
						break;
					}
				}
			} else {
				if (m_editType == EDIT_MOVE) {
					float nx = m_editFromX + GetMouseX();
					float ny = m_editFromY + GetMouseY();

					auto box = m_editPanel->getBox();

					m_editPanel->move(nx-box.x, ny-box.y);
					//m_editPanel->setRight(nx+box.w);
					//m_editPanel->setLeft(m_editPanel->m_right-box.w);

					//m_editPanel->setTop(ny);
					//m_editPanel->setBottom(m_editPanel->m_top + box.h);

					//m_editPanel->setBottom(ny + box.h);
					//m_editPanel->setTop(m_editPanel->m_bottom-box.h);

					if (IsKeyReleased(GLFW_MOUSE_BUTTON_1)) {
						m_editPanel = nullptr;
					}
				} else if (m_editType == EDIT_CONSTRAIN) {
					
					if (IsKeyReleased(GLFW_MOUSE_BUTTON_1)) {
						for (int i = m_panels.size() - 1; i >= 0; i--) {
							Panel* panel = m_panels[i];
							if (panel == m_editPanel)
								continue;
							if (!panel->isMovable()) // should this be here?
								continue;
							ui::Box area = panel->getBox();
							if (ui::Hover(area)) {
								// create constraint but which side?

								float mx = GetMouseX();
								float my = GetMouseY();

								int type = 0;
								float minLength = 99999;
								for (int i = 0; i < 4;i++) {
									float x, y;
									if (i % 2) {
										x = panel->m_edges[i];
										y = (panel->m_edges[0] + panel->m_edges[2]) / 2;
									} else {
										x = (panel->m_edges[1] + panel->m_edges[3]) / 2;
										y = panel->m_edges[i];
									}

									float length = sqrt(pow(x-mx,2)+pow(y-my,2));
									//log::out << i << " " << x << " " << y << " "<<length<<"\n";
									if (length < minLength){
										minLength = length;
										type = i;
										//log::out << "t " << i << "\n";
									}
								}
								int type2 = type^2;
								
								float minOffset = -9999;
								float offset = m_editPanel->m_edges[type2] - panel->m_edges[type];
								if (type2 & 2) {
									offset = -m_editPanel->m_edges[type2] + panel->m_edges[type];
								}
								if (offset < minOffset)
									offset = minOffset;
								
								//m_editPanel->addConstraint(type2, offset, minOffset, panel);
								break;
							}
						}
						m_editPanel = nullptr;
					}
				} else if (m_editType == EDIT_RESIZE) {

					float mx = GetMouseX();
					float my = GetMouseY();
					//log::out << mx << " " << my << "\n";
					ui::Box area = m_editPanel->getBox();

					if (m_editResizingX == 0) {
						if (area.x + area.w < mx) {
							m_editResizingX = 1;
						}
						if (area.x > mx) {
							m_editResizingX = -1;
						}
					} else {
						if (m_editResizingX==1) {
							float nw = mx - area.x;
							if (nw <m_editPanel->m_minWidth) {
								m_editResizingX = 0;
								nw = m_editPanel->m_minWidth;
							}
							//m_editPanel->resize(nw-(m_editPanel->m_right-m_editPanel->m_left),0);
							m_editPanel->setRight(area.x+nw);
						} else {
							float fixed = area.x + area.w;
							float nw = fixed - mx;
							float nx = mx;

							// needed because the panel system will move the panel otherwise.
							if (nw < m_editPanel->m_minWidth) {
								nw = m_editPanel->m_minWidth;
								nx = fixed - m_editPanel->m_minWidth;
								m_editResizingX = 0;
							}
							//m_editPanel->resize(nw - (m_editPanel->m_right - m_editPanel->m_left), 0);
							m_editPanel->setLeft(nx);
							//m_editPanel->setW(nw);
							//m_editPanel->setX(nx);
						}
					}
					if (m_editResizingY == 0) {
						if (area.y + area.h < my) {
							m_editResizingY = 1;
						}
						if (area.y > my) {
							m_editResizingY = -1;
						}
					} else {
						if (m_editResizingY==1) {
							float nh = my - area.y;
							// clamp
							if (nh < m_editPanel->m_minHeight) {
								m_editResizingY = 0;
								nh = m_editPanel->m_minHeight;
							}
							//m_editPanel->resize(0, nh - (m_editPanel->m_bottom - m_editPanel->m_top));
							m_editPanel->setBottom(area.y+nh);
						} else {
							float fixed = area.y + area.h;
							float nh = fixed - my;
							float ny = my;

							if (nh < m_editPanel->m_minHeight) {
								nh = m_editPanel->m_minHeight;
								ny = fixed - m_editPanel->m_minHeight;
								m_editResizingY = 0;
							}
							//m_editPanel->resize(0, nh - (m_editPanel->m_bottom - m_editPanel->m_top));
							m_editPanel->setTop(ny);
							//m_editPanel->setH(nh);
							//m_editPanel->setY(ny);
						}
					}

					if (IsKeyReleased(GLFW_MOUSE_BUTTON_2)) {
						m_editPanel = nullptr;
						//log::out << "Released 2\n";
					}
				}
			}
		}
	}
}