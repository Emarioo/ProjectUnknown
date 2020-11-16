#include "Visual.h"

#include "Gui/SelectionItem.h"

#include <Windows.h>

// THIS CLASS IS DEPRECATED

//Font* font1;

// Game Menu
int gameStage;// 0 = login, 1 = selection, 2 = gameplay
/*
ISquare sBack;
IButton bLogin;
IInput iUser;
IInput iKey;

// Selection
int selectStage=0;
int selectServer=-1;
ISquare selMulti;
ISquare selMultiBack;
IButton selMultiAdd;
std::vector<IButton> serverButtons;
std::vector<SelectionItem> serverData;

int selectWorld = -1;
ISquare selSingle;
ISquare selSingleBack;
IButton selSingleAdd;
std::vector<IButton> worldButtons;
std::vector<SelectionItem> worldData;

ISquare selMidBack;
IInput selMidTop;
IInput selMidPass;
IInput selMidInfo;// Changes size
IButton selMidJoin;
IButton selMidJoin2;
IButton selMidBot;
IButton selMidDel;

// Chat
IInput iChat;
bool chatMode;
int chatActive=60*10;
void UpdateChat() {
	chatActive = 60 * 10;
}
bool GetChatMode() {
	return chatMode;
}
void SetChatMode(bool f) {
	chatMode = f;
}
int maxMsg=10;
int msgHeight=25;
std::vector<Text> chatMessages;
void NewMessage(std::string msg) {
	bool createNew=true;
	for (int i = 0; i < chatMessages.size(); i++) {
		if (chatMessages[i].ypos<Height()-maxMsg*msgHeight) {
			chatMessages[i].SetPY(Height() - 75);
			chatMessages[i].SetText(msg);
			createNew = false;
		} else {
			chatMessages[i].SetPY(chatMessages[i].ypos-msgHeight);
		}
	}
	if (createNew) {
		chatMessages.push_back(Text());
		chatMessages[chatMessages.size() - 1].Setup(10, Height()-75, 60, font1, 26);
		chatMessages[chatMessages.size() - 1].SetText(msg);
		chatMessages[chatMessages.size() - 1].SetCol(0.3, 0.3, 0.3, 1);
	}
}

// Pause
ISquare pauseFading;

IButton pauseSettings;
IButton pauseLeave;
IButton pauseQuit;

// Settings
IButton settingsFullscreen;
// Shadow Toggle?
*/
// Gameplay
bool RenderGame = false;
bool cursorMode=true;
/*
Move into Renderer
*/
void SetCursorMode(bool f) {
	if (cursorMode != f) {
		if (f) {
			/*
			if (GetLastWindowType()==WindowBorder) {
				SetWindowType(WindowBorder);
			} else if (GetLastWindowType()==Maximized) {
				SetWindowType(Maximized);
			}
			*/
			glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			if (glfwRawMouseMotionSupported()) {
				glfwSetInputMode(GetWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
			}
		} else {
			/*
			if (GetWindowType() == WindowBorder) {
				SetWindowType(WindowBorderless);
			} else if (GetWindowType() == Maximized) {
				SetWindowType(WindowBorderless);
			}
			*/
			glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (glfwRawMouseMotionSupported()) {
				glfwSetInputMode(GetWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			}
		}
		cursorMode = f;
	}
}
bool pauseMode = false;
void SetPauseMode(bool f) {
	if (f) {
		pauseMode = true;
		SetCursorMode(true);
	} else {
		pauseMode = false;
		SetCursorMode(false);
	}
}
bool GetPauseMode() {
	return pauseMode;
}
bool GetCursorMode() {
	return cursorMode;
}

void AddServer() {
	// connect to ip and get data;
	/*
	IButton temp;
	temp.SetTex(0.4,0.5,0.6,1);
	temp.SetDimP(10+5,0,250,40);
	temp.SetText(20, font1, 0.08);
	temp.text.SetText(selMidTop.text.text+" 0/10");
	temp.text.SetCol(0, 0, 0, 1);
	serverButtons.push_back(temp);
	serverData.push_back(SelectionItem(selMidTop.text.text,selMidPass.text.text));
	*/
}
void AddWorld() {
	/*
	IButton temp;
	temp.SetTex(0.4, 0.5, 0.6, 1);
	temp.SetDimP(Wid()-265, 0, 250, 40);
	temp.SetText(20, font1, 0.08);
	temp.text.SetText(selMidTop.text.text);
	temp.text.SetCol(0, 0, 0, 1);
	worldButtons.push_back(temp);
	worldData.push_back(SelectionItem(selMidTop.text.text, ""));
	*/
}
void SelectStage(int n) {
	/*
	int lx = 10, sw = 260, sh = Hei() - 2 * 60;
	int rx = Wid() - sw - 10;
	int mx = 10 + sw + 10, mw = Wid() - 2 * (10 + sw + 10);
	selectStage = n;
	if (n == 0) {
		selectServer = -1;
		selectWorld = -1;
	} else if (n == 1) {
		selMidTop.text.SetText("Name");
		selMidBot.text.SetText("Create World");
	} else if (n == 2) {
		selMidTop.text.SetText(worldData[selectWorld].first);
		selMidJoin.SetDimP(mx + 5, 10 + 40 + 10 + 5 + 40 + 5, (mw - 10 - 5) / 2, 40);
		selMidBot.text.SetText("Save Changes");
	} else if (n == 3) {
		selMidTop.text.SetText("IP Address");
		selMidPass.text.SetText("Password");
		selMidBot.text.SetText("Add Server");
	} else if (n == 4) {
		selMidTop.text.SetText(serverData[selectServer].first);
		selMidPass.text.SetText(serverData[selectServer].second);
		selMidJoin.SetDimP(mx + 5, 10 + 40 + 10 + 5 + 40 + 5, mw - 10, 40);
		selMidBot.text.SetText("Save Changes");
	}
	*/
}

double mouseX, mouseY;
// Events
void FocusEvent(int focus) {
	if (!focus) {
		SetPauseMode(true);
	}
}
void KeyEvent(int key, int action) {
	if (action == 1) {
		//std::cout << "Key " << key << " " << GetChar(key, 0, 0) << std::endl;
	}
	if (gameStage==2) {// Gameplay
		if (pauseMode) {
			if (action == 1) {
				if (key == GLFW_KEY_ESCAPE) {
					SetPauseMode(false);
				}
			}
		} else {
			if (chatMode) {
				iChat.KeyPress(key, action);
				if (action == 1) {
					if (key == GLFW_KEY_ESCAPE) {
						iChat.ClearKey();
						SetChatMode(false);
						SetPauseMode(true);
					} else if (key == GLFW_KEY_ENTER) {
						// Execute code
						std::string msg = iChat.text.text;
						if (msg[0] == CHAT_CMD_CHAR()) {
							ChatExecuteCommand(msg.substr(1));
						} else {
							NewMessage(msg);
							UpdateChat();
						}
						iChat.ClearKey();
						chatMode = false;
					}
				}
			} else {
				if (action == 1) {
					if (key == GLFW_KEY_ESCAPE) {
						SetPauseMode(true);
					} else if (key == GLFW_KEY_ENTER) {
						SetChatMode(true);
					}
				}
			}
		}
	} else if (gameStage==1) {
		if (selMidTop.IsWriting) {
			selMidTop.KeyPress(key,action);
		} else if (selMidPass.IsWriting) {
			selMidPass.KeyPress(key, action);
		}
	}
}
void MouseEvent(double mx,double my,int button, int action) {
	//std::cout << "MButton " << button << " " << action << " " << mods << std::endl;
	if (gameStage == 2) {// Gameplay
		if (pauseMode) {
			/*if (action == 1) {
				SetEscMode(false);
			}*/
			if (action == 1) {
				if (pauseSettings.Clicked(mx, my)) {
					std::cout << "Open Settings..."<< std::endl;
					if (IsFullscreen()) {
						//std::cout << "a" << std::endl;
						SetWindowType(WindowBorderless);
					} else {
						SetWindowType(Fullscreen);
					}

					/* When using my own Border
					if (IsFullscreen()) {
						SetWindowType(WindowBorder);
					} else {
						SetWindowType(Fullscreen);
					}
					*/
				} else if (pauseLeave.Clicked(mx, my)) {
					std::cout << "Leave World/Server..." << std::endl;
				} else if (pauseQuit.Clicked(mx, my)) {
					std::cout << "Quit Game..." << std::endl;
				} else {
					SetPauseMode(false);
				}
			}
		}
	} else if (gameStage == 1) {// Selection
		if (button == 0 && action == 1) {
			selMidTop.IsWriting = false;
			selMidPass.IsWriting = false;
			if (selMultiAdd.Clicked(mx, my)) {
				SelectStage(3);
			} else if (selSingleAdd.Clicked(mx,my)) {
				SelectStage(1);
			} else {
				// loop server buttons
				for (int i = 0; i < serverButtons.size(); i++) {
					if (serverButtons[i].Clicked(mx, my)) {
						selectServer = i;
						SelectStage(4);
						return;
					}
				}
				for (int i = 0; i < worldButtons.size(); i++) {
					if (worldButtons[i].Clicked(mx, my)) {
						selectWorld = i;
						SelectStage(2);
						return;
					}
				}
				if (selectStage == 1) {
					if (selMidTop.Clicked(mx, my)) {
						// Change name
						selMidTop.IsWriting = true;
					} else if (selMidInfo.Clicked(mx, my)) {
						// change settings?

					} else if (selMidBot.Clicked(mx, my)) {
						// Create World
						SelectStage(0);
						AddWorld();
					}
				} else if (selectStage == 2) {
					if (selMidTop.Clicked(mx, my)) {
						// Change name
						selMidTop.IsWriting = true;
					} else if (selMidInfo.Clicked(mx, my)) {
						// change settings?

					} else if (selMidJoin.Clicked(mx, my)) {
						// Join Server

					} else if (selMidJoin2.Clicked(mx, my)) {
						// Join World

					} else if (selMidBot.Clicked(mx, my)) {
						// Save Changes
						SelectStage(0);
					} else if (selMidDel.Clicked(mx, my)) {
						// Delete World
						if (selectWorld != -1) {
							worldButtons.erase(worldButtons.begin() + selectWorld);
							worldData.erase(worldData.begin() + selectWorld);
							SelectStage(0);
						}
					}
				} else if (selectStage == 3) {
					if (selMidTop.Clicked(mx, my)) {
						// Change ip
						selMidTop.IsWriting = true;
					} else if (selMidPass.Clicked(mx, my)) {
						// Change password
						selMidPass.IsWriting = true;
					} else if (selMidBot.Clicked(mx, my)) {
						// Add server
						AddServer();
						SelectStage(0);
					}
				} else if (selectStage == 4) {
					if (selMidTop.Clicked(mx, my)) {
						// Change name
						selMidTop.IsWriting = true;
					} else if (selMidPass.Clicked(mx, my)) {
						// change pass
						selMidPass.IsWriting = true;
					} else if (selMidJoin.Clicked(mx, my)) {
						// Join Server
						
					} else if (selMidBot.Clicked(mx, my)) {
						// Save Changes
						SelectStage(0);
					} else if (selMidDel.Clicked(mx, my)) {
						// Delete Server
						if (selectServer != -1) {
							serverButtons.erase(serverButtons.begin() + selectServer);
							serverData.erase(serverData.begin() + selectServer);
							SelectStage(0);
						}
					}
				}
			}
		}
	} else if (gameStage == 0) {// Login
		if (button == 0 && action) {
			iKey.IsWriting = false;
			iUser.IsWriting = false;
			if (bLogin.Clicked(mx, my)) {
				bool user = true;
				bool key = true;
				if (iUser.text.text.length() == 1) {
					user = false;
				}
				if (iKey.text.text != "ak29t") {// TODO: TEMPORARY KEY
					key = false;
				}
				if (!user && !key) {
					std::cout << "NAME CANNOT BE EMPTY AND KEY IS WRONG" << std::endl;;
				} else if (user && !key) {
					std::cout << "KEY IS INCORRECT" << std::endl;
				} else if (!user&&key) {
					std::cout << "NAME CANNOT BE EMPTY" << std::endl;
				} else {
					std::cout << "LOGIN" << std::endl;
					gameStage = 1;
				}
			} else if (iUser.Clicked(mx, my)) {
				std::cout << "Clicked USER" << std::endl;
				iUser.IsWriting = true;
			} else if (iKey.Clicked(mx, my)) {
				std::cout << "Clicked KEY" << std::endl;
				iKey.IsWriting = true;
			}
		}
	}
}
void DragEvent(double mx, double my) {
	//std::cout << mx<< " "<<my << std::endl;
	if (gameStage==2) { // Gameplay
		if (!cursorMode) {
			//std::cout << "what" << GetCamera()->rotation.y<< std::endl;
			GetCamera()->rotation.y -= (mx - mouseX) * (3.14159f / 360)*0.18;
			GetCamera()->rotation.x -= (my - mouseY) * (3.14159f / 360)*0.18;
		}
		mouseX = mx;
		mouseY = my;
	} else if (gameStage==1){// Selection
		
	} else if (gameStage==0) { // Login
		/*
		bLogin.SetTex(1, 1, 1, 1);
		//bSingle.SetTex(1, 1, 1, 1);
		//bMulti.SetTex(1, 1, 1, 1);
		if (bLogin.Inside(mx, my)) {
			bLogin.SetTex(1.5, 1.5, 1.5, 1);
		}/* else if (bSingle.Inside(mx, my)) {
			bSingle.SetTex(0.6, 0.6, 0.6, 1);
		} else if (bMulti.Inside(mx, my)) {
			bMulti.SetTex(0.6, 0.6, 0.6, 1);
		}*/
	}
}
void ResizeEvent(int width, int height) {
	if (width > 0 && height > 0) {
		SetProjection((float)Width()/Height());
		DrawVisual();
	}
}

// Initialization
void InitVisual() {
	SetCallbacks(KeyEvent,MouseEvent,DragEvent,ResizeEvent,FocusEvent);

	// Start
	gameStage = 2;
	if (gameStage == 2) {
		RenderGame = true;
		SetPauseMode(true);
	}

	// Gui Init
	font1 = new Font("assets/fonts/verdana38");

	AddTexture("squ_back", "assets/textures/squ_back");
	//sBack.SetTex("squ_back");
	//sBack.SetDim(-1, -1, 2, 2);

	AddTexture("inp_user", "assets/textures/inp_user");
	//iUser.SetTex("inp_user");
	//iUser.SetDim(-0.8,0.4,0.4,0.2);
	
	iUser.text.Setup(iUser.x, iUser.y,20,font1,0.1);
	iUser.text.SetText("Emarioo");
	iUser.text.SetCol(0, 0, 0.9, 1);

	AddTexture("inp_key", "assets/textures/inp_key");
	//iKey.SetTex("inp_key");
	//iKey.SetDim(iUser.x,iUser.y-0.1-iUser.h,iUser.w,iUser.h);
	
	iKey.text.Setup(iKey.x,iKey.y,20,font1,0.1);
	iKey.text.SetText("ak29t");
	iKey.text.SetCol(0.2, 0, 0.9, 1);

	AddTexture("but_login", "assets/textures/but_login");
	//bLogin.SetTex("but_login");
	//bLogin.SetDim(iKey.x,iKey.y-0.1-iKey.h, 0.3, 0.15);
	
	// Selection
	int lx = 10, sw=260, sh=Hei()-2*60;
	int rx = Wid()-sw-10;
	int mx = 10+sw+10, mw=Wid()-2*(10+sw+10);

	// Multi
	//selMulti.SetTex(1,1,1,1);
	//selMulti.SetDimP(lx, Hei()-10-40, sw, 40);
	//selMulti.SetText(15, font1, 0.1);
	selMulti.text.SetText("Multiplayer");
	selMulti.text.SetCol(0, 0, 0, 1);

	//selMultiBack.SetTex(0.7, 0.7, 0.7, 1);
	//selMultiBack.SetDimP(lx, 10+40+10, sw, Hei()-2*(10+40+10));
	
	//selMultiAdd.SetTex(1, 1, 1, 1);
	//selMultiAdd.SetDimP(lx, 10, sw, 40);
	//selMultiAdd.SetText(15,font1,0.1);
	selMultiAdd.text.SetText("Add Server");
	selMultiAdd.text.SetCol(0, 0, 0, 1);

	// Single
	//selSingle.SetTex(1, 1, 1, 1);
	//selSingle.SetDimP(rx, Hei()-10-40, sw, 40);
	//selSingle.SetText(15, font1, 0.1);
	selSingle.text.SetText("Singleplayer");
	selSingle.text.SetCol(0, 0, 0, 1);

	//selSingleBack.SetTex(0.7, 0.7, 0.7, 1);
	//selSingleBack.SetDimP(rx, 10+40+10, sw, sh);

	//selSingleAdd.SetTex(1, 1, 1, 1);
	//selSingleAdd.SetDimP(rx, 10, sw, 40);
	//selSingleAdd.SetText(15, font1, 0.1);
	selSingleAdd.text.SetText("Add World");
	selSingleAdd.text.SetCol(0, 0, 0, 1);

	// Mid
	//selMidBack.SetTex(1, 1, 1, 1);
	//selMidBack.SetDimP(mx, 10+40+10, mw, Hei()-10-40-10-10);

	//selMidTop.SetTex(0.5, 1, 1, 1);
	//selMidTop.SetDimP(mx+5, Hei()-10-5-40, mw-10, 40);
	//selMidTop.SetText(25,font1,0.1);
	selMidTop.text.SetCol(0, 0, 0, 1);

	//selMidPass.SetTex(1, 0.5, 1, 1);
	//selMidPass.SetDimP(mx+5, Hei()-10-5-40-5-40, mw-10, 40);
	//selMidPass.SetText(15, font1, 0.1);
	selMidPass.text.SetCol(0, 0, 0, 1);

	// REMOVED WITH WID()
	//selMidInfo.SetTex(1, 1, 0.5, 1);
	//selMidInfo.SetDimP(mx+Wid(), 70+20+70, mw-10, 40);
	//selMidInfo.SetText(15, font1, 0.1);
	selMidInfo.text.SetCol(0, 0, 0, 1);

	//selMidJoin.SetTex(0.7, 1, 0.5, 1);
	//selMidJoin.SetDimP(mx+5, 10+40+10+5+40+5, (mw - 10 - 5) / 2, 40);
	//selMidJoin.SetText(15, font1, 0.1);
	//selMidJoin.text.SetText("Join Server");
	selMidJoin.text.SetCol(0, 0, 0, 1);

	//selMidJoin2.SetTex(0.5, 1, 0.7, 1);
	//selMidJoin2.SetDimP(mx+5+ (mw - 10 - 5) / 2+5, 10+40+10+5+40+5, (mw-10-5)/2, 40);
	//selMidJoin2.SetText(15, font1, 0.1);
	selMidJoin2.text.SetText("Join World");
	selMidJoin2.text.SetCol(0, 0, 0, 1);

	//selMidBot.SetTex(1, 0.5, 0.5, 1);
	//selMidBot.SetDimP(mx + 5, 10+40+10+5, mw-10, 40);
	//selMidBot.SetText(15, font1, 0.1);
	selMidBot.text.SetCol(0, 0, 0, 1);

	//selMidDel.SetTex(0.5, 0.5, 0.5, 1);
	//selMidDel.SetDimP(mx, 10, mw, 40);
	//selMidDel.SetText(15, font1, 0.1);
	selMidDel.text.SetText("Delete");
	selMidDel.text.SetCol(0, 0, 0, 1);
	// Load servers from text file
	//  Create buttons

	// Chat
	//iChat.SetTex(0.4,0.4,0.4,0.4);
	//iChat.SetDim(-0.95, -0.95, 0.5, 0.15);
	iChat.text.Setup(iChat.x, iChat.y, 60, font1, 0.1);
	iChat.text.SetText("");
	iChat.text.SetCol(1, 1, 1, 1);

	// Pause
	//pauseFading.SetTex(0,0,0,0.3);
	//pauseFading.SetDim(-1,-1,2,2);
	//pauseFading.SetFade(0.5);

	AddTexture("pause_settings", "assets/textures/pause_settings");
	//pauseSettings.SetTex("pause_settings");
	//pauseSettings.SetDim(-0.95,0.6, 0.3,0.3);
	//pauseSettings.SetFade(0.5);
	//pauseSettings.SetText(10, font1, 0.1);
	pauseSettings.text.SetText("Settings");
	pauseSettings.text.SetCol(0.9, 0.9, 0.9, 1);

	//AddTexture("pause_quit", "assets/textures/pause_quit");
	//pauseQuit.SetTex("pause_settings");
	//pauseQuit.SetDim(pauseSettings.x,-0.9,pauseSettings.w,pauseSettings.h);
	//pauseQuit.SetFade(0.5);
	//pauseQuit.SetText(10, font1, 0.1);
	pauseQuit.text.SetText("Quit");
	pauseQuit.text.SetCol(0.9, 0.9, 0.9, 1);


	//AddTexture("pause_leave", "assets/textures/pause_leave");
	//pauseLeave.SetTex("pause_settings");
	//pauseLeave.SetDim(pauseSettings.x, pauseQuit.y+pauseQuit.h+0.1, pauseSettings.w, pauseSettings.h);
	//pauseLeave.SetFade(0.5);
	//pauseLeave.SetText(10, font1, 0.1);
	pauseLeave.text.SetText("Leave");
	pauseLeave.text.SetCol(0.9, 0.9, 0.9, 1);
}

Dimension* loadedDim;
void SetDimension(Dimension* d){
	loadedDim = d;
}

// Rendering Controller
std::vector<GameObject*> texRender;
std::vector<GameObject*> colorRender;
void AddRenderO(std::string s,GameObject* o) {
	if (s=="tex") {
		texRender.push_back(o);
	}else if (s == "color") {
		colorRender.push_back(o);
	}
}
void DelRenderO(std::string s,GameObject* o) {
	if (s == "tex") {
		for (int i = 0; i < texRender.size(); i++) {
			if (texRender[i] == o) {
				texRender.erase(texRender.begin() + i);
				break;
			}
		}
	} else if (s == "color") {
		for (int i = 0; i < colorRender.size(); i++) {
			if (colorRender[i] == o) {
				colorRender.erase(colorRender.begin() + i);
				break;
			}
		}
	}
}
void ClearRender(std::string s) {
	if (s == "tex") {
		texRender.clear();
	} else if (s == "color") {
		colorRender.clear();
	}
}
void UpdateVisual() {
}
/*
Runs glfwSwapBuffers at the end
Don't run if window is minimized
*/
void DrawVisual() {
	if (IsMinimized())
		return;

	RenderClearScreen(0.1f, 0.1f, 0.2f, 1);

	// Border
	//RenderBorder();
	
	// Game
	if (RenderGame) {
		SwitchBlendDepth(false);
		BindShader("tex");
		for (GameObject* o : texRender) {
			o->Draw();
		}
		BindShader("color");
		for (GameObject* o : colorRender) {
			o->Draw();
		}
		BindShader("terrain");
		if (loadedDim != nullptr) {
			for (Chunk c : loadedDim->loadedChunks) {
				Location base;
				base.Translate(glm::vec3(c.x*(loadedDim->chunkSize), 0, c.z*(loadedDim->chunkSize)));
				ObjectTransform(base.mat());
				c.con.Draw();
			}
		}
	}

	// Gui
	SwitchBlendDepth(true);
	BindShader("gui");

	if (chatMode) {
		iChat.Draw(true);
	}
	if (gameStage==2) {
		if (chatActive > 0)
			chatActive--;
		if (chatMode || chatActive > 0) {
			for (int i = 0; i < chatMessages.size(); i++) {
				chatMessages[i].DrawString();
			}
		}
	}
	// Menu
	if (gameStage == 0) {
		sBack.Draw(true);

		iUser.Draw(true);
		iKey.Draw(true);
		bLogin.Draw(true);
	}
	
	// Selection
	/*
	if (gameStage == 1) {
		selMulti.Draw();
		selMultiBack.Draw();
		selMultiAdd.Draw();

		selSingle.Draw();
		selSingleBack.Draw();
		selSingleAdd.Draw();

		for (int i = 0; i < serverButtons.size();i++) {
			/erverButtons[i].SetpY(Hei()-(70+5+40+i*45));
			serverButtons[i].Draw();
		}
		for (int i = 0; i < worldButtons.size(); i++) {
			worldButtons[i].SetpY(Hei() - (70 + 5 + 40+i * 45));
			worldButtons[i].Draw();
		}

		if (selectStage==1) {
			selMidBack.Draw();
			selMidTop.Draw();

			selMidInfo.Draw();// change size

			selMidBot.Draw();
		} else if (selectStage==2) {
			selMidBack.Draw();
			selMidTop.Draw();

			selMidInfo.Draw();// change size
			selMidJoin.Draw();// change size
			selMidJoin2.Draw();

			selMidBot.Draw();
			selMidDel.Draw();
		} else if (selectStage==3) {
			selMidBack.Draw();
			selMidTop.Draw();

			selMidPass.Draw();

			selMidBot.Draw();
		} else if (selectStage==4) {
			selMidBack.Draw();
			selMidTop.Draw();

			selMidPass.Draw();
			selMidInfo.Draw();// change size
			selMidJoin.Draw();// change size

			selMidBot.Draw();
			selMidDel.Draw();
		}
	}
	*/
	// Pause
	if (gameStage == 2) {
		pauseFading.Draw(pauseMode);
		pauseSettings.Draw(pauseMode);
		pauseLeave.Draw(pauseMode);
		pauseQuit.Draw(pauseMode);
	}
	
	glfwSwapBuffers(GetWindow());
}