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
	engone::ui::Box Panel::getBox() {
		return { m_left,m_top,m_right-m_left,m_bottom-m_top };
	}
	void Panel::retrieveXYWH(float out[4]){
		*(out+0) = m_left;
		*(out+1) = m_top;
		*(out+2) = m_right-m_left;
		*(out+3) = m_bottom-m_top;	
	}
	void Panel::setTop(float f) {
		using namespace engone;
		ui::Box area = getBox();
		Window* win = engone::GetActiveWindow();
		if (f < 0) {
			m_top = 0;
		} else if (f > win->getHeight()) {
			m_top = win->getHeight();
		} else {
			m_top = f;
		}
		if (m_bottom - m_top > m_maxHeight) {
			m_top = m_bottom - m_maxHeight;
		}
		if (m_bottom - m_top < m_minHeight) {
			m_top = m_bottom - m_minHeight;
		}
	}
	void Panel::setLeft(float f) {
		using namespace engone;
		ui::Box area = getBox();
		Window* win = engone::GetActiveWindow();

		if (f < 0) {
			m_left = 0;
		} else if (f > win->getWidth()) {
			m_left = win->getWidth(); // rare scenario, check if m_right is less than m_left?
		} else {
			m_left = f;
		}
		if (m_right-m_left > m_maxWidth) {
			m_left = m_right - m_maxWidth;
		}
		if (m_right - m_left < m_minWidth) {
			m_left = m_right - m_minWidth;
		}
	}
	void Panel::setRight(float f) {
		using namespace engone;
		ui::Box area = getBox();
		Window* win = engone::GetActiveWindow();

		if (f < 0) {
			m_right = 0; // m_left?
		} else if (f > win->getWidth()) {
			m_right = win->getWidth();
		} else {
			m_right = f; // m_left?
		}
		if (m_right - m_left > m_maxWidth) {
			m_right = m_left + m_maxWidth;
		}
		if (m_right - m_left < m_minWidth) {
			m_right = m_left + m_minWidth;
		}
	}
	void Panel::setBottom(float f) {
		using namespace engone;
		Window* win = engone::GetActiveWindow();
		ui::Box area = getBox();

		if (f < 0) {
			m_bottom = 0;
		} else if (f > win->getHeight()) {
			m_bottom = win->getHeight();
		} else {
			m_bottom = f;
		}
		if (m_bottom - m_top > m_maxHeight) {
			m_bottom = m_top + m_maxHeight;
		}
		if (m_bottom - m_top < m_minHeight) {
			m_bottom = m_top + m_minHeight;
		}
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
		Window* win = panelHandler->window;
		if (m_left + dx < 0) {
			m_right = m_right - m_left;
			m_left = 0;
		}else if (m_right + dx > win->getWidth()) {
			m_left = win->getWidth() - (m_right-m_left);
			m_right = win->getWidth();
		} else {
			m_left += dx;
			m_right += dx;
		}
		if (m_top + dy < 0) {
			m_bottom = m_bottom - m_top;
			m_top = 0;
		}else if (m_bottom + dy > win->getHeight()) {
			m_top = win->getHeight() - (m_bottom - m_top);
			m_bottom = win->getHeight();
		} else {
			m_top += dy;
			m_bottom += dy;
		}
	}
	void Panel::updateConstraints() {
		using namespace engone;
		Window* win = engone::GetActiveWindow();
		if (m_right - m_left > win->getWidth()) {
			m_left = 0;
			m_right = win->getWidth();
		}
		if (m_bottom - m_top > win->getHeight()) {
			m_left = 0;
			m_right = win->getWidth();
		}
		if (m_left < 0) {
			m_right = m_right - m_left;
			m_left = 0;
		}
		if (m_right > win->getWidth()) {
			m_left = win->getWidth()-(m_right-m_left);
			m_right = win->getWidth();
		}
		if (m_top < 0) {
			m_bottom = m_bottom - m_top;
			m_top = 0;
		}
		if (m_bottom > win->getHeight()) {
			m_top = win->getHeight() - (m_bottom - m_top);
			m_bottom = win->getHeight();
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
	void PanelHandler::init(engone::Window* win){
		window = win;
	}
	
	Panel* PanelHandler::createPanel() {
		Panel* panel = (Panel*)engone::GetGameMemory()->allocate(sizeof(Panel));
		new(panel)Panel();
		panel->panelHandler = this;
		m_panels.push_back(panel);
		return panel;
	}
	void PanelHandler::sortPanels() {
		using namespace engone;
		// Simple bubble sort. There will probably not be more than 100 panels.
		for (int i = 0; i < m_panels.size();i++) {
			for (int j = 0; j < m_panels.size()-1-i; j++) {
				if (m_panels[j]->depth>m_panels[j+1]->depth) {
					Panel* temp = m_panels[j];
					m_panels[j] = m_panels[j + 1];
					m_panels[j + 1] = temp;
				}
			}
		}
	}
	void PanelHandler::render(engone::LoopInfo* info) {
		using namespace engone;
		
		Assert(("Did you call init?",window)); // if window is null then you forgot to call init
		
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
			if (m_panels[i]->hidden)
				continue;
			m_panels[i]->updateConstraints();
		}

		for (int i =0; i < m_panels.size(); i++) {
			if (m_panels[i]->hidden)
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
			m_panels[i]->updateProc(info,m_panels[i]);
		}
	}
	
	Panel* PanelHandler::getPanel(const char* name){
		for(Panel* p : m_panels){
			if(strcmp(p->name.c_str(),name)==0)
				return p;
		}
		return 0;
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
					if (panel->hidden)
						continue;
					if (!panel->movable)
						continue;
					ui::Box area = panel->getBox();
					int clicked = ui::Clicked(area);
					int clicked2 = ui::Clicked(area, GLFW_MOUSE_BUTTON_2);
					if (clicked == 1||clicked2==1) {
						m_editPanel = panel;
						m_editFromX = area.x-GetMouseX();
						m_editFromY = area.y-GetMouseY();
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