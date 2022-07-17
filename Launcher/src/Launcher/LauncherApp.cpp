#include "Launcher/LauncherApp.h"
#include "Engone/EventModule.h"
#include <windows.h>

#include "../resource.h"

namespace launcher {
	enum LauncherNetType {
		LauncherClientFiles, // the files the client has
		LauncherServerSendFile, // the files the client has
		LauncherServerFinished, // server has sent all files
	};

	static wchar_t buffer[256]{};
	std::wstring convert(const std::string& in) {
		ZeroMemory(buffer, 256);
		for (int i = 0; i < in.length();i++) {
			buffer[i] = in[i];
		}
		return buffer;
	}
	bool startGame(const std::string& path) {
		if (!engone::FindFile(path)) {
			return false;
		}

		// additional information
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		
		int slashIndex = path.find_last_of("\\");

		std::string workingDir = path.substr(0,slashIndex);

#ifdef NDEBUG
		std::wstring exeFile = convert(path);
		std::wstring workDir = convert(workingDir);
#else
		const std::string& exeFile = path;
		std::string& workDir = workingDir;
#endif
		//CreateProcess(path.c_str(),   // the path
		//	NULL,        // Command line
		//	NULL,           // Process handle not inheritable
		//	NULL,           // Thread handle not inheritable
		//	FALSE,          // Set handle inheritance to FALSE
		//	0,              // No creation flags
		//	NULL,           // Use parent's environment block
		//	workingDir.c_str(),   // starting directory 
		//	&si,            // Pointer to STARTUPINFO structure
		//	&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		//);
		CreateProcess(exeFile.c_str(),   // the path
			NULL,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			workDir.c_str(),   // starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		);

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}
	LauncherApp::LauncherApp(engone::Engone* engone, const std::string& settings) : Application(engone) {
		using namespace engone;
		m_window = createWindow({ WindowMode::BorderlessMode,600,150 });
		m_window->setTitle("Launcher");

		//consolas = getAssets()->set<Font>("consolas", "fonts/consolas42");

		// Windows stuff
		HRSRC hs = FindResource(NULL, MAKEINTRESOURCE(IDB_PNG1), "PNG");
		HGLOBAL hg = LoadResource(NULL, hs);
		void* ptr = LockResource(hg);
		DWORD size = SizeofResource(NULL, hs);
		const char* txtBuffer="4\n35";
		consolas = getAssets()->set<Font>("consolas", new Font((char*)ptr,size,txtBuffer,strlen(txtBuffer), getAssets()));

		bool yes = m_settings.load(settings);
		if (yes) {
			address.text = m_settings.get("ip") + ":" + m_settings.get("port");
		}
		m_cache.load();

		m_server.setOnEvent([this](NetEvent e, uint32_t uuid) {
			if (NetEvent::Connect == e) {
				launcherMessage = "Connect "+std::to_string(uuid);
			} else if (NetEvent::Stopped == e) {
				launcherMessage = "Stopped server";
			}
			return true;
		});
		m_server.setOnReceive([this](MessageBuffer buf, uint32_t uuid) {
			LauncherNetType type;
			buf.pull(&type);
			if (type == LauncherClientFiles) {
				//log::out << "recieved\n";
				std::vector<FileInfo> sentFiles;
				uint32_t size;
				buf.pull(&size);
				for (int i = 0; i < size;i++) {
					FileInfo f;
					buf.pull(f.path);
					buf.pull(&f.time);
					sentFiles.push_back(f);
				}

				struct FileSend {
					std::string fullPath;// where server finds the file
					std::string path; // path where client should put file
					uint64_t time;
				};
				std::vector<FileSend> sendFiles;
				
				auto gameFiles = LoadGameFiles();				

				for (int i = 0; i < gameFiles.size();i++) {
					std::string& origin = gameFiles[i].originPath;
					std::string& endPath = gameFiles[i].path;

					bool isDir = std::filesystem::is_directory(origin);

					if (!isDir) {
						if (i == 0) {
							if (origin.find(".exe") == -1) {
								log::out << log::YELLOW << "first file is not .exe\n";
							}
						}
						uint64_t time = GetFileLastModified(origin);
						if (time != 0) {
							bool sendFile = true;
							for (int i = 0; i < sentFiles.size(); i++) {
								FileInfo& f = sentFiles[i];
								if (f.path == endPath) {
									if (f.time != time) {
										// send file
									} else {
										// file is up to date
										sendFile = false;
									}
								}
							}
							if (sendFile) {
								sendFiles.push_back({ origin,endPath,time });
							}
						} else {
							log::out <<log::RED<< "file not found\n";
						}
					} else {
						std::filesystem::recursive_directory_iterator itera(origin);
						// PATHS MAY NOT BE DIRECTORIES
						for (auto const& entry : itera) {
							if (entry.is_directory()) continue;
							std::string filePath = entry.path().generic_string(); // THIS MAY BE BAD CASTING PATH
							FormatPath(filePath);

							std::string partPath = endPath+filePath.substr(origin.size()); // THIS MAY BE BAD CASTING PATH
							uint64_t time = std::chrono::duration_cast<std::chrono::seconds>(entry.last_write_time().time_since_epoch()).count();
							bool sendFile = true;
							for (int i = 0; i < sentFiles.size(); i++) {
								FileInfo& f = sentFiles[i];
								if (f.path == partPath) {
									if (f.time != time) {
										// send file
									} else {
										// it's all good
										sendFile = false;
									}
								}
							}
							if (sendFile) {
								sendFiles.push_back({ filePath,partPath,time });
							}
						}
					}
				}
				
				for (int i = 0; i < sendFiles.size();i++) {
					FileSend& f = sendFiles[i];
					//std::cout << f.path << "\n";
					std::ifstream file(f.fullPath, std::ios::binary);
					if (file) {
						file.seekg(0, file.end);
						uint32_t fileSize = file.tellg();
						file.seekg(0, file.beg);

						MessageBuffer sendBuf(LauncherServerSendFile);
						sendBuf.push(f.path);
						sendBuf.push(f.time);
						sendBuf.push(fileSize);
						char* data = sendBuf.pushBuffer(fileSize);
						file.read(data, fileSize);

						m_server.send(sendBuf,uuid);
						file.close();
					}
				}
				if (sendFiles.size() == 0) {
					log::out << "Server: client is up to date\n";
				}
				MessageBuffer sendBuf(LauncherServerFinished);
				m_server.send(sendBuf, uuid);
				//for (std::string& path : paths) {
				//	std::filesystem::directory_iterator iter(path);
				//	for (auto const& entry : iter) {
				//		for (int i = 0; i < files.size();i++) {
				//			
				//		}
				//		//entry.is_directory()
				//	}
				//}

				// client has file, server does not - Send delete message for all files to delete
				// client has file, server has file - if client file time is different server file time, send file.
				// server has file, client does not - server send file
			}

			return true;
		});
		m_client.setOnEvent([this](NetEvent e) {
			if (NetEvent::Connect == e) {
				launcherMessage = "Connected";

				std::vector<FileInfo>& files = m_cache.getFiles();

				MessageBuffer buf(LauncherClientFiles);
				buf.push((uint32_t)files.size());
				for (int i = 0; i < files.size();i++) {
					buf.push(files[i].path);
					buf.push(files[i].time);
				}

				m_client.send(buf);
			} else if (NetEvent::Failed==e) {
				launcherMessage = "Network Failed";
			}
			return true;
		});
		m_client.setOnReceive([this](MessageBuffer buf) {
			LauncherNetType type;
			buf.pull(&type);
			if (type == LauncherServerSendFile) {
				std::string path;
				std::string fullPath;
				uint64_t time;
				uint32_t fileSize;
				buf.pull(path);
				buf.pull(&time);
				buf.pull(&fileSize);
				char* data = buf.pullBuffer(&fileSize);

				log::out << fullPath << "\n";

				fullPath = root + path;

				//if (!FindFile(fullPath)) {
				try {
					std::filesystem::create_directories(fullPath.substr(0, fullPath.find_last_of(PATH_DELIM)));
				} catch (std::filesystem::filesystem_error err) {
					log::out<<log::RED << err.what() << "\n";
				}
				//}
				//int find = fullPath.find("wall.png");
				//if (find!=-1) {
				//	log::out << "int " << fullPath.find_last_of("eaeeaf");
				//	//int ea=16;
				//}
				std::ofstream file(fullPath,std::ios::binary);
				if (file) {
					file.write(data, fileSize);
					launcherMessage = path;
					m_cache.set({ path, time });
					file.close();
				} else {
					log::out << log::RED << " Failed\n";
				}
			} else if (type == LauncherServerFinished) {
				// download is done, save to cache
				m_cache.save();
				std::string exePath;
				

				if (m_cache.getFiles().size()!=0) {
					if (m_cache.getFiles()[0].path.find_last_of(".exe") != -1) {
						exePath = root + m_cache.getFiles()[0].path;
					}
				}
				log::out << "Client: finished ," << exePath << "\n";
				if (!exePath.empty()) {
					startGame(exePath);
				} else {
					log::out << log::RED << "First file was not executable\n";
				}
			}
			return true;
		});

		// This makes the top area of the window draggable
		m_window->attachListener(new Listener((EventClick | EventMove), [this](Event& e) {
			if (e.eventType==EventClick) {
				if (e.button == GLFW_MOUSE_BUTTON_1) {
					if (e.action == 1) {
						if (e.my < 15&&m_window->hasFocus()) {
							draggingWindow = true;
							diffMX = e.mx;
							diffMY = e.my;
						}
					} else if(e.action==0) {
						draggingWindow = false;
					}
				}
			} else if(e.eventType==EventMove) {
				if (draggingWindow) {
					float x = m_window->getX() + e.mx;
					float y = m_window->getY() + e.my;
					m_window->setPosition(x-diffMX, y-diffMY);
				}
			}
			return EventNone;
		}));
	}

	void LauncherApp::update(engone::UpdateInfo& info) {

	}
	void LauncherApp::render(engone::RenderInfo& info) {
		using namespace engone;

		ui::Box quitBack = { GetWidth() - 25,0,25,25,{0.5,0.05,0.1,1} };
		ui::TextBox quit = { "X", quitBack.x+5,3,20,consolas,{1,1,1,1}};

		if (ui::Hover(quitBack)) {
			quitBack.rgba.r = 0.7;
			quitBack.rgba.g = 0.4;
			quitBack.rgba.b = 0.4;
		}
		ui::Draw(quitBack);
		ui::Draw(quit);
		if (ui::Clicked(quitBack)) {
			m_window->close();
		}

		ui::Color inputBack = { 0.7,0.7,0.7,1 };
		ui::Box boxAddress = { 5,15,500,40,inputBack };
		ui::Draw(boxAddress);

		ui::Color textColor = { 0.05,0.05,0.05,1 };

		address.x = boxAddress.x;
		address.y = boxAddress.y;
		address.h = boxAddress.h;
		address.font = consolas;
		address.rgba = textColor;

		if (ui::Clicked(boxAddress)) {
			address.editing = true;
		} else if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {
			address.editing = false;
		}
		bool editBefore = address.editing;
		ui::Edit(&address);
		ui::Draw(address);

		// Changing settings based on input
		if (address.editing != editBefore && editBefore) {
			std::vector<std::string> split = SplitString(address.text, ":");

			if (split.size() == 0) {
				launcherMessage = "Address is empty";
			} else if (split.size()<=2) {
				if (split[0] == "host") {
					m_settings.set("isServer", "true");
				} else {
					m_settings.remove("isServer");
				}
				m_settings.set("ip", split[0]);
				if (split.size() == 2) {
					m_settings.set("port", split[1]);
				} else {
					m_settings.set("port", Settings::PORT);
				}
				doAction();
			} else {
				launcherMessage = "Format should be ip:port";
			}
		}

		// Launcher message
		ui::Box boxInfo = { 5,boxAddress.y + boxAddress.h + 5,500,20,inputBack };
		ui::Draw(boxInfo);
		engone::ui::TextBox launcherInfo = { launcherMessage,boxInfo.x,boxInfo.y,boxInfo.h,consolas,textColor };
		ui::Draw(launcherInfo);
	}
	void LauncherApp::onClose(engone::Window* window) {
		m_settings.save();

		m_server.stop();
		m_client.disconnect();
	}
	void LauncherApp::doAction() {

		std::string ip = m_settings.get("ip");
		std::string port = m_settings.get("port");
		std::string isServer = m_settings.get("isServer");
		if (isServer == "true") {
			if (m_server.isRunning()) {
				launcherMessage = "Server already started";
			} else {
				if (m_client.isRunning())
					m_client.disconnect();
				m_server.start(port);
				launcherMessage = "Server started";
			}
		} else {
			if (ip.length() != 0) {
				if (m_client.isRunning()) {
					launcherMessage = "Already connected";
				} else{
					if (m_server.isRunning())
						m_server.stop();
					m_client.connect(ip, port);
				}
			} else {
				launcherMessage = "Missing IP";
			}
		}
	}
}