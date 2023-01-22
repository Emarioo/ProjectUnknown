#include "Launcher/LauncherApp.h"
#include "Engone/EventModule.h"

#include "../resource.h"

namespace launcher {
	enum LauncherNetType {
		LauncherClientFiles, // the files the client has
		LauncherServerSendFile, // the files the client has
		LauncherServerFinished, // server has sent all files
		LauncherServerError,
	};
	enum StreamState : uint8_t {
		StreamStart = 1,
		StreamEnd = 2,
	};
	typedef uint8_t StreamStates;
	LauncherApp::LauncherApp(engone::Engone* engone, const std::string& settings) : Application(engone) {
		using namespace engone;
		m_window = createWindow({ WindowMode::ModeBorderless,600,150 });
		m_window->setTitle("Launcher");

		consolas = getStorage()->set<FontAsset>("consolas", ALLOC_NEW(FontAsset)(IDB_PNG1, "4\n35"));

		bool yes = m_settings.load(settings);

		address = m_settings.get("address");
		std::string state = m_settings.get("state");
		if (state == "client")
			launcherState = LauncherClientMenu;
		if (state == "server")
			launcherState = LauncherServerMenu;
			//launcherState = LauncherDownloading;

		setupGamefiles();

		m_cache.load();

		m_server.setOnEvent([this](NetEvent e, UUID uuid) {
			if (NetEvent::Connect == e) {
				//launcherMessage = "Connect "+std::to_string(uuid);
			} else if (NetEvent::Stopped == e) {
				//launcherMessage = "Stopped server";
				//log::out << "Serv Stopped\n";
			} else if (NetEvent::Failed == e) {
			}
			return true;
		});
		m_server.setOnReceive([this](MessageBuffer& buf, UUID uuid) {
			//log::out << "1 buf size " << buf.size() << "\n";
			LauncherNetType type;
			buf.pull(&type);
			//ENGONE_DEBUG(log::out << "Server recv " << type << "\n", LAUNCHER_LEVEL, 1);
			if (type == LauncherClientFiles) {
				recClientFiles(buf, uuid);
			} else {
				log::out << log::RED << "Server received invalid net type\n";
			}

			return true;
		});
		m_client.setOnEvent([this](NetEvent e, UUID uuid) {
			if (NetEvent::Connect == e) {
				isConnecting = false;
			} else if (NetEvent::Failed == e) {
				isConnecting = false;
				lastError = "Could not connect. Timed out?\n";
			}
			return true;
		});
		m_client.setOnReceive([this](MessageBuffer& buf, UUID clientUUID) {
			LauncherNetType type;
			buf.pull(&type);
			//ENGONE_DEBUG(log::out<<"Client recv "<<type <<"\n",LAUNCHER_LEVEL,1);
			if (type == LauncherServerSendFile) {
				recServerSendFile(buf, clientUUID);
			} else if (type == LauncherServerFinished) {
				recServerFinished(buf, clientUUID);
			} else if (type == LauncherServerError) {
				recServerError(buf, clientUUID);
			} else {
				log::out << log::RED << "Client received invalid net type\n";
			}
			return true;
		});
		// This makes the top area of the window draggable
		m_window->attachListener(new Listener(EventClick | EventMove, [this](Event& e) {
			if (e.eventType == EventClick) {
				if (e.button == GLFW_MOUSE_BUTTON_1) {
					if (e.action == 1) {
						if (e.my < topMoveEdge && m_window->hasFocus()) {
							draggingWindow = true;
							diffMX = e.mx;
							diffMY = e.my;
						}
					} else if (e.action == 0) {
						draggingWindow = false;
					}
				}
			} else if (e.eventType == EventMove) {
				if (draggingWindow) {
					float x = m_window->getX() + e.mx;
					float y = m_window->getY() + e.my;
					m_window->setPosition(x - diffMX, y - diffMY);
				}
			}
			return EventNone;
		}));
		// ISSUE: without this, the app doesn't act how i want. it says connecting... when it's not even trying to.
		//doAction(true);
	}
	LauncherApp::~LauncherApp() {
		using namespace engone;
		for (auto [key, entry] : gameFileEntries) {
			if (entry.refresher)
				ALLOC_DELETE(FileMonitor, entry.refresher);
		}
		for (auto [key, download] : fileDownloads) {
			ALLOC_DELETE(FileWriter, download);
		}
	}
	void LauncherApp::recClientFiles(engone::MessageBuffer& buf, engone::UUID clientUUID) {
		using namespace engone;
		//log::out << "recieved\n";
		std::vector<FileInfo> sentFiles;
		uint32_t size;
		buf.pull(&size);
		//ENGONE_DEBUG(log::out << "Files: "<<size << "\n", LAUNCHER_LEVEL, 2);
		for (int i = 0; i < size; i++) {
			FileInfo f;
			buf.pull(f.path);
			buf.pull(&f.time);
			sentFiles.push_back(f);
		}

		std::vector<FileSend> sendFiles;

		bool ohNo = false;

		for (auto [_, entry] : gameFileEntries) {
			std::string& origin = entry.source;
			std::string& endPath = entry.destination;

			//log::out << "Entry "<< origin<<"\n";
			if (origin == "startconfig") {
				sendFiles.push_back({ origin,endPath,0 });
			} else {
				bool isDir = std::filesystem::is_directory(origin); // if origin is invalid, this will be false.

				if (!isDir) {
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
						log::out << log::RED << "Launcher::Serv::recv - " << origin << " not found\n";
						ohNo = true;
						break;
					}
				} else {
					std::filesystem::recursive_directory_iterator itera(origin);
					// PATHS MAY NOT BE DIRECTORIES
					for (auto const& entry : itera) {
						if (entry.is_directory()) continue;
						std::string filePath = entry.path().generic_string(); // THIS MAY BE BAD CASTING PATH
						FormatPath(filePath);

						std::string partPath = endPath + filePath.substr(origin.size()); // THIS MAY BE BAD CASTING PATH
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
		}
		if (!ohNo)
			for (int i = 0; i < sendFiles.size(); i++) {
				sendFile(sendFiles[i], clientUUID, false);
			}

		// Make a log of messages. infoText will show how the last connection went but not the rest since infoText will override that info.
		if (ohNo) {
			log::out << log::RED<< "GameFile Paths are probably wrong\n";
			MessageBuffer sendBuf;

			LauncherNetType type = LauncherServerError;
			sendBuf.push(&type);
			m_server.send(sendBuf, clientUUID);
		} else {
			//infoText.text = "";
			if (sendFiles.size() == 0) {
				//log::out << "Server: client is up to date\n";
			}
			MessageBuffer sendBuf;
			LauncherNetType type = LauncherServerFinished;
			sendBuf.push(&type);
			m_server.send(sendBuf, clientUUID);
		}
	}
	void LauncherApp::recServerSendFile(engone::MessageBuffer& buf, engone::UUID clientUUID){
		using namespace engone;
		UUID fileUuid;
		buf.pull(&fileUuid);
		StreamStates state;
		buf.pull(&state);
		FileWriter* writer = nullptr;
		if (state & StreamStart) {
			std::string path;
			buf.pull(path);
			uint64_t time;
			buf.pull(&time);

			std::string fullPath = root + path;
			try {
				std::filesystem::create_directories(fullPath.substr(0, fullPath.find_last_of(PATH_DELIM)));
			} catch (std::filesystem::filesystem_error err) {
				log::out << log::RED << "Client recv " << err.what() << "\n";
			}

			writer = ALLOC_NEW(FileWriter)(fullPath, true);
			//log::out << "New writer " << fullPath << "\n";
			GetTracker().track(writer);
			if (writer->isOpen()) {
				downloadMutex.lock();
				//log::out << "Path " << path << "\n";
				recentDownloads.push_back(path);
				downloadMutex.unlock();
				fileDownloads[fileUuid] = writer;
				m_cache.set({ path, time });
			} else {
				ALLOC_DELETE(FileWriter, writer);
				//delete writer;
				GetTracker().untrack(writer);
				int index = fullPath.find_last_of(".exe");
				if (index != -1) {
					log::out << log::RED << "Failed writing " << fullPath << ". Is the game running? \n";
				} else {
					log::out << log::RED << "Failed writing " << fullPath << " \n";
				}
				return;
			}
		} else {
			auto find = fileDownloads.find(fileUuid);
			if (find != fileDownloads.end()) {
				writer = find->second;
			} else {
				// error message is already logged.
				return;
			}
		}

		uint32_t transferedBytes;
		char* data = buf.pullBuffer(&transferedBytes);

		writer->write(data, transferedBytes);

		if (state & StreamEnd) { // last package
			GetTracker().untrack(writer);
			//delete writer; // this will also close writer
			//log::out << "del writer " << writer->getPath() << "\n";
			ALLOC_DELETE(FileWriter, writer);

			fileDownloads.erase(fileUuid);
		}

		//delayDownloadFailed.start(5);
		//if (infoText.text.length() != 0)
		//	infoText.text = "Or not...";

	}
	void LauncherApp::recServerFinished(engone::MessageBuffer& buf, engone::UUID clientUUID){
		using namespace engone;
		// download is done, save to cache
		m_cache.save();

		launcherState = LauncherClientMenu;
		//delayDownloadFailed.stop();
		bool success = launchGame();
		if (!success) {
			lastError = "Developer issues";
		}
	}
	void LauncherApp::recServerError(engone::MessageBuffer& buf, engone::UUID clientUUID) {
		using namespace engone;
		//delayDownloadFailed.stop();

		launcherState = LauncherClientMenu;

		lastError = "Server didn't want to cooperate";

	}
	void LauncherApp::download() {
		using namespace engone;
		if (!clientFullyConnected())
			return;

		launcherState = LauncherDownloading;
		downloadMutex.lock();
		recentDownloads.clear();
		downloadMutex.unlock();

		std::vector<FileInfo>& files = m_cache.getFiles();

		MessageBuffer buf;
		LauncherNetType type = LauncherClientFiles;
		buf.push(&type);
		uint32_t size = files.size();
		buf.push(size);
		//ENGONE_DEBUG(log::out <<"Sending files count " <<size << "\n", LAUNCHER_LEVEL, 1);
		for (int i = 0; i < files.size(); i++) {
			buf.push(files[i].path);
			buf.push(files[i].time);
		}
		//log::out << "buf " << buf.size()<<"\n";
		m_client.send(buf);
	}
	void LauncherApp::setupGamefiles() {
		using namespace engone;
		gameFilesRefresher.check(GAME_FILES_PATH, [this](const std::string& path, uint32 changeType) {
			// should also check for file deletion
			FileReader file(path, false);
		if (file) {
			gameFileEntries.erase("startconfig"); // reset config to replace with new ones

			auto list = file.readLines();
			for (auto& entry : list) {
				std::vector<std::string> split = engone::SplitString(entry, "=");;
				if (split.size() == 2) {
					if (split[0] == "args" || split[0] == "exe") {
						auto find = gameFileEntries.find("startconfig");
						if (find == gameFileEntries.end()) {
							EntryInfo info = { "startconfig", split[0] + "=" + split[1], nullptr };
							gameFileEntries["startconfig"] = info;
						} else {
							find->second.destination += "\n";
							find->second.destination += split[0] + "=" + split[1];
						}
					} else {
						auto find = gameFileEntries.find(split[0]);
						if (find == gameFileEntries.end()) {
							EntryInfo info = { split[0], split[1], ALLOC_NEW(FileMonitor)() };
							gameFileEntries[split[0]] = info;
							std::string source = info.source;

							info.refresher->check(info.source, [this, source](const std::string& sourcePath, uint32 type) {
								EntryInfo& info = gameFileEntries[source];

							// send files
							FileSend fileSend = { source + "\\" + sourcePath,
								info.destination + "\\" + sourcePath,
								GetFileLastModified(sourcePath) };

							sendFile(fileSend, 0, true);
							}, FileMonitor::WATCH_SUBTREE);
						} else {
							// entry already exists so do nothing
						}
					}
				} else {
					log::out << log::RED << "LauncherApp::LauncherApp - format wrong: " << entry << "\n";
				}
			}
			file.close();
			//printf("GameFiles closed\n");
		} else {
			log::out << log::RED << "LauncherApp::LauncherApp - access to '" << GAME_FILES_PATH << "' was denied\n";
		}
		}, 0);
		// just call it
		gameFilesRefresher.getCallback()(GAME_FILES_PATH, 0);
	}
	void LauncherApp::sendFile(FileSend& f, engone::UUID clientUUID, bool exclude) {
		using namespace engone;
		//log::out << "Send "<< f.fullPath<<"\n" ;
		if (f.fullPath == "startconfig") {
			MessageBuffer sendBuf;
			LauncherNetType type = LauncherServerSendFile;
			sendBuf.push(&type);
			UUID fileUuid = UUID::New();
			sendBuf.push(fileUuid);
			StreamStates state = StreamStart | StreamEnd; // arguments is probably not going to be huge so one send is enough.

			sendBuf.push(state);
			sendBuf.push(f.fullPath);
			sendBuf.push(f.time);

			char* data = sendBuf.pushBuffer(f.path.length());
			memcpy(data, f.path.c_str(), f.path.length());

			m_server.send(sendBuf, clientUUID, exclude);
		} else {
			std::ifstream file(f.fullPath, std::ios::binary);
			if (file) {
				file.seekg(0, file.end);
				uint32_t fileSize = file.tellg();
				file.seekg(0, file.beg);

				UUID fileUuid = UUID::New();
				uint32_t bytesLeft = fileSize;
				while (bytesLeft != 0) {
					MessageBuffer sendBuf;
					LauncherNetType type = LauncherServerSendFile;
					sendBuf.push(&type);
					sendBuf.push(fileUuid);
					StreamStates state = 0;
					if (bytesLeft == fileSize) state = state | StreamStart;
					if (bytesLeft <= m_server.getTransferLimit()) state = state | StreamEnd;

					sendBuf.push(state);
					if (state & StreamStart) {
						sendBuf.push(f.path);
						sendBuf.push(f.time);
					}

					uint32_t headerSize = sendBuf.size() + sizeof(uint32_t); // <- for transferBytes in pushBuffer

					uint32_t transferBytes = std::min(bytesLeft, m_server.getTransferLimit() - headerSize);

					char* data = sendBuf.pushBuffer(transferBytes);
					file.read(data, transferBytes);

					//log::out << "SERV SEND " << sendBuf.size() << "\n";

					m_server.send(sendBuf, clientUUID, exclude);

					bytesLeft -= transferBytes;
				}
				file.close();
			} else {
				log::out << log::RED << "LauncherApp::sendFile - denied access to '" << f.fullPath << "'\n";
			}
		}
	}
	bool LauncherApp::launchGame() {
		using namespace engone;

		FileReader file(root + "startconfig", false);
		if (!file) {
			log::out << log::RED << "Missing startconfig, cannot launch game\n";
			return false;
		}

		std::vector<std::string> lines = file.readLines();
		file.close();
		std::string exeFile;
		std::string args;
		for (std::string& line : lines) {
			std::vector<std::string> split = SplitString(line, "=");
			if (split.size() == 2) {
				if (split[0] == "exe") {
					exeFile = split[1];
				} else if (split[0] == "args") {
					args = split[1];
				}
			} else {
				log::out << log::YELLOW << "LauncherApp::launchGame : startconfig has bad format (line: '" << line << "')\n";
			}
		}
		if (exeFile.empty()) {
			log::out << log::RED << "Missing exe path in startconfig, cannot launch game\n";
			return false;
		}
		exeFile = root + exeFile;
		bool success;
		if (args.empty())
			success = StartProgram(exeFile);
		else
			success = StartProgram(exeFile, args.data());

		if (!success) {
			log::out << log::RED << "LauncherApp::launchGame : Failed starting executable. Is the game already running?\n";
			return false;
		}
		return true;
	}
	void LauncherApp::update(engone::LoopInfo& info) {
		if (isConnecting) {
			connectTime += info.timeStep;
		}
		//loadingTime += info.timeStep;
		//if (delaySwitch.run(info)) {
		//	switchState(LauncherSetting);
		//}
		//if (delayDownloadFailed.run(info)) {
		//	infoText.text = "Sorry to break it to you, but downloading failed\n";
		//}
	}
	void LauncherApp::render(engone::LoopInfo& info) {
		using namespace engone;
		float sw = GetWidth(), sh = GetHeight();

		float errorHeight = 20;
		float border = 5;

		//-- Background
		ui::Box backgroundBox = { 0,0,sw,sh,backgroundColor };
		ui::Draw(backgroundBox);

		ui::Box backgroundEdgeBox = { 0,0,sw,topMoveEdge,backgroundTopColor };
		ui::Draw(backgroundEdgeBox);

		//-- Quit
		ui::Box quitBack = { 0,0,25,25,{0.5,0.05,0.1,1} };
		quitBack.x = sw - quitBack.w;
		quitBack.y = 0;
		ui::TextBox quit = { "X", quitBack.x + 5,3,20,consolas,{1,1,1,1} };
		if (ui::Hover(quitBack)) {
			quitBack.rgba.r = 0.7;
			quitBack.rgba.g = 0.4;
			quitBack.rgba.b = 0.4;
		}
		ui::Draw(quitBack);
		ui::Draw(quit);
		if (ui::Clicked(quitBack) == 1) {
			m_window->close();
		}

		if (launcherState == LauncherClientMenu)
			renderClient(info);
		else if (launcherState == LauncherServerMenu)
			renderServer(info);
		else if (launcherState == LauncherDownloading)
			renderDownloading(info);

		//-- Error
		if (!lastError.empty()) {
			ui::TextBox errorText = { lastError, 0,0,errorHeight, consolas,errorColor };
			errorText.x = sw - errorText.getWidth() - border;
			errorText.y = sh - errorText.h - border;

			ui::Draw(errorText);
		}

		return;

		////-- Menu / Colors
		//float width = GetWidth(), height = GetHeight();

		//ui::Color backColor = { 52 / 255.f,23 / 255.f,109 / 255.f,1 };
		//ui::Color lightBackColor = { 52 / 255.f * 1.2,23 / 255.f * 1.2,109 / 255.f * 1.2,1 };
		//ui::Color white = { 1,1,1,1 };
		//ui::Color gray = { 0.7,0.7,0.7,1 };

		//ui::Box background = { 0,0,width,height, backColor };
		//ui::Draw(background);

		//ui::Box quitBack = { width - 25,0,25,25,{0.5,0.05,0.1,1} };
		//ui::TextBox quit = { "X", quitBack.x + 5,3,20,consolas,{1,1,1,1} };

		//if (ui::Hover(quitBack)) {
		//	quitBack.rgba.r = 0.7;
		//	quitBack.rgba.g = 0.4;
		//	quitBack.rgba.b = 0.4;
		//}
		//ui::Draw(quitBack);
		//ui::Draw(quit);
		//if (ui::Clicked(quitBack) == 1) {
		//	m_window->close();
		//}

		//-- State dependent
		//if (state == LauncherConnecting) {
		//	// Launcher name
		//	ui::TextBox launcherText = { launcherName,0,0,30,consolas,white };
		//	launcherText.x = width / 2 - consolas->getWidth(launcherName, launcherText.h) / 2;
		//	launcherText.y = height / 2 - launcherText.h / 2;
		//	ui::Draw(launcherText);

		//	// Connecting info, should also show other info, like connection failed
		//	infoText.text = "Connecting";
		//	infoText.y = launcherText.y + launcherText.h;
		//	infoText.h = launcherText.h * 0.7;
		//	infoText.x = width / 2 - consolas->getWidth("Connecting..", infoText.h) / 2;
		//	infoText.font = consolas;
		//	infoText.rgba = gray;
		//	float timeDiff = 0.5;
		//	int dots = (int)(loadingTime / timeDiff) % 6;
		//	if (dots > 3)
		//		dots = 6 - dots;
		//	for (int i = 0; i < dots; i++) {
		//		infoText.text += '.';
		//	}
		//	ui::Draw(infoText);
		//} 
		//else if (state == LauncherDownloading) {
		//	// Download text
		//	ui::TextBox download = { "Downloading",0,height * 0.3,30,consolas,white };
		//	float timeDiff = 0.5;
		//	int dots = (int)(loadingTime / timeDiff) % 6;
		//	if (dots > 3)
		//		dots = 6 - dots;
		//	for (int i = 0; i < dots; i++) {
		//		download.text += '.';
		//	}
		//	download.x = width / 2 - consolas->getWidth("Downloading..", download.h) / 2;
		//	ui::Draw(download);

		//	// Last downloaded file
		//	ui::TextBox lastFile = { lastDownloadedFile,0,download.y + download.h,download.h * 0.9,consolas,gray };
		//	lastFile.x = width / 2 - consolas->getWidth(lastDownloadedFile, lastFile.h) / 2;
		//	ui::Draw(lastFile);

		//	if (infoText.text.length() != 0) {
		//		infoText.h = lastFile.h * 0.8;
		//		infoText.font = consolas;
		//		infoText.rgba = gray;
		//		infoText.y = lastFile.y + lastFile.h + height * 0.08;
		//		infoText.x = width / 2 - consolas->getWidth(infoText.text, infoText.h) / 2;
		//		ui::Draw(infoText);
		//	}
		//} 
		//else if (state == LauncherSetting) {
		//	ui::TextBox serverText = { "Address",0,0,30,consolas,white };
		//	serverText.x = width / 2 - consolas->getWidth(serverText.text, serverText.h)/2;
		//	serverText.y = height * 0.25-serverText.h/2;
		//	ui::Draw(serverText);

		//	ui::Box addressBack = { 0,0,0,40,lightBackColor };
		//	addressBack.w = width*0.9;
		//	addressBack.y = serverText.y + serverText.h+height*0.03;
		//	addressBack.x = width/2 -addressBack.w/2;
		//	ui::Draw(addressBack);

		//	bool editBefore = addressText.editing;
		//	if (ui::Clicked(addressBack)==1) {
		//		addressText.editing = true;
		//		editBefore = true;
		//	} else if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)||!info.window->hasFocus()) {
		//		addressText.editing = false;
		//	}

		//	// prevent some characters like # $, or rather, only allow a-Z, 0-9 and .
		//	ui::Edit(&addressText);
		//	
		//	ui::TextBox suggestText = { "",0,addressBack.y,addressBack.h,consolas,gray };

		//	int index = addressText.text.find_last_of(':');
		//	if (addressText.editing) {
		//		if (index == -1)
		//			suggestText.text = ":" + Settings::PORT;
		//		else if (index == addressText.text.length() - 1)
		//			suggestText.text = Settings::PORT;
		//	}

		//	addressText.h = addressBack.h;
		//	addressText.x = addressBack.x+addressBack.w/2-consolas->getWidth(addressText.text+suggestText.text,addressText.h)/2;
		//	addressText.y = addressBack.y;
		//	addressText.font = consolas;
		//	addressText.rgba = white;

		//	ui::Draw(addressText);

		//	if (addressText.editing) {
		//		suggestText.x = addressText.x + consolas->getWidth(addressText.text, addressText.h);
		//		ui::Draw(suggestText);
		//	}

		//	// Changing settings based on input
		//	if (addressText.editing != editBefore && editBefore) {
		//		interpretAddress();
		//		doAction();
		//	}

		//	if (infoText.text.length() != 0) {
		//		infoText.h = addressText.h * 0.5;
		//		infoText.font = consolas;
		//		infoText.rgba = gray;
		//		infoText.y = addressBack.y + addressBack.h + height * 0.08;
		//		infoText.x = width / 2 - consolas->getWidth(infoText.text, infoText.h) / 2;
		//		ui::Draw(infoText);
		//	}
		//} 
		//else if (state == LauncherServerSide) {
		//ui::TextBox serverTitle = { "Server connections:" + std::to_string(m_server.getConnectionCount()),0,height * 0.07,35,consolas,white };
		//serverTitle.x = width / 2 - consolas->getWidth(serverTitle.text, serverTitle.h) / 2;
		//ui::Draw(serverTitle);

		//const uint32_t bufferSize = 150;
		//char infoBuffer[bufferSize];
		//ZeroMemory(infoBuffer, bufferSize);
		//uint32_t write = 0;

		//write += snprintf(infoBuffer, bufferSize, "Bytes S/R: %llu/%llu\nMsgs S/R: %llu/%llu", m_server.getStats().sentBytes(), m_server.getStats().receivedBytes(), m_server.getStats().sentMessages(), m_server.getStats().receivedMessages());
		//infoBuffer[bufferSize - 1] = 0;
		//ui::TextBox connectInfo = { infoBuffer,0,serverTitle.y + serverTitle.h,serverTitle.h * 0.7,consolas,white };
		//float wid = consolas->getWidth(connectInfo.text, connectInfo.h);
		//connectInfo.x = width / 2 - wid / 2;
		//ui::Draw(connectInfo);

		//// Add more info here, like how many bytes server received and sent.

		//if (infoText.text.length() != 0) {
		//	infoText.h = serverTitle.h * 0.5;
		//	infoText.y = connectInfo.y + connectInfo.h * 2 + height * 0.08;
		//	infoText.x = width / 2 - consolas->getWidth(infoText.text, infoText.h) / 2;
		//	infoText.font = consolas;
		//	infoText.rgba = gray;
		//	ui::Draw(infoText);
		//}
		//}
	}
	//void LauncherApp::interpretAddress() {
	//	std::vector<std::string> split = engone::SplitString(addressText.text, ":");
	//	if (split.size() == 0) {
	//	} else if (split.size() <= 2) {
	//		m_settings.set("ip", split[0]);
	//		if (split.size() == 2) {
	//			if (split[1].length() != 0)
	//				m_settings.set("port", split[1]);
	//			else
	//				m_settings.set("port", Settings::PORT);
	//		} else {
	//			m_settings.set("port", Settings::PORT);
	//		}
	//	}
	//}
	//void LauncherApp::switchState(LauncherState newState) {
	//	state = newState;
	//	loadingTime = 0;
	//	//infoText.text = switchInfoText;
	//	//switchInfoText = "";
	//	if (state == LauncherSetting) {
	//		std::string ip = m_settings.get("ip");
	//		std::string port = m_settings.get("port");
	//		if (ip.length() != 0) {
	//			if (port.length() != 0 && port != Settings::PORT) {
	//				addressText.text = ip + ":" + port;
	//			} else {
	//				addressText.text = ip;
	//			}
	//		} else {
	//			if (port.length() != 0 && port != Settings::PORT) {
	//				addressText.text = ":" + port;
	//			} else {
	//				addressText.text = "";
	//			}
	//		}
	//	}
	//}
	void LauncherApp::onClose(engone::Window* window) {
		m_settings.set("address", address);
		if(launcherState==LauncherClientMenu)
			m_settings.set("state", "client");
		if(launcherState==LauncherServerMenu)
			m_settings.set("state", "server");
		m_settings.save();

		m_server.stop();
		m_client.stop();
		stop();
	}
	void LauncherApp::connect() {
		using namespace engone;
		std::string ip;
		std::string port = Settings::PORT;

		//-- Convert address
		std::vector<std::string> split = engone::SplitString(address, ":");
		if (split.size() >= 1) {
			ip = split[0];
		}
		if (split.size() >= 2) {
			if (split[1].length() != 0)
				port = split[1];
		}
		if (launcherState == LauncherClientMenu) {
			if (m_client.isRunning()) {
				log::out << "Client is already running\n";
			} else {
				if (m_server.isRunning())
					m_server.stop();
				bool success = m_client.start(ip, port);
				if (!success) {
					// infoText can be ambiguous, was the ip invalid because the server didn't exist and the client couldn't connect or
					// because the ip wasn't a correctly formatted ip address. 12#515Ar <- this would be an invalid ip.
					lastError = "Connection failed";
					//if (delay) {
					//	delaySwitch.start(1);
					//} else
					//	switchState(LauncherSetting);
				} else {
					isConnecting = true; // mutex?
					connectTime = 0;
					lastError = "";
				}
			}
		} else if (launcherState == LauncherServerMenu) {
			if (m_server.isRunning()) {
				log::out << "Server is already running\n";
			} else {
				if (m_client.isRunning())
					m_client.stop();
				bool success = m_server.start(port);
				if (!success) {
					lastError = "Failed connection";
					//if (delay)
					//	delaySwitch.start(1);
					//else
					//	switchState(LauncherSetting);
				} else {
					//isConnecting = true;
					// connection is instant. (not really but the network module doesn't support good callbacks for checking the server connecting)
					//connectTime = 0;
					lastError = "";
				}
			}
		}
	}
	//void LauncherApp::doAction(bool delay) {
	//	using namespace engone;
	//	std::string ip = m_settings.get("ip");
	//	std::string port = m_settings.get("port");
	//	if (ip == "force") {
	//		if (!launchGame()) {
	//			//m_window->close();
	//			//switchInfoText = "Cache is invalid";
	//			infoText.text = "Cache is invalid. Connect at least once.";
	//			switchState(LauncherSetting);
	//		} else {
	//			// what should be done if successful?
	//		}
	//	}else if (ip == "host") {
	//		if (m_server.isRunning()) {
	//			log::out << "Server is already running\n";
	//		} else {
	//			if (m_client.isRunning())
	//				m_client.stop();
	//			if (!m_server.start(port)) {
	//				infoText.text = "Invalid Port";
	//				//switchInfoText = "Invalid Port";
	//				if (delay)
	//					delaySwitch.start(1);
	//				else
	//					switchState(LauncherSetting);
	//			} else {
	//				infoText.text = "";
	//				switchState(LauncherServerSide);
	//			}
	//		}
	//	} else {
	//		if (m_client.isRunning()) {
	//			log::out << "Client is already running\n";
	//		} else {
	//			if (m_server.isRunning())
	//				m_server.stop();
	//			if (!m_client.start(ip, port)) {
	//				// infoText can be ambiguous, was the ip invalid because the server didn't exist and the client couldn't connect or
	//				// because the ip wasn't a correctly formatted ip address. 12#515Ar <- this would be an invalid ip.
	//				//switchInfoText= "Invalid IP/Port";
	//				infoText.text = "Invalid IP/Port";
	//				if (delay) {
	//					delaySwitch.start(1);
	//				} else
	//					switchState(LauncherSetting);
	//			} else {
	//				infoText.text = "";
	//				switchState(LauncherConnecting);
	//			}
	//		}
	//	}
	//}
	bool LauncherApp::clientFullyConnected() {
		return m_client.isRunning() && !isConnecting;
	}
	void LauncherApp::renderClient(engone::LoopInfo& info) {
		using namespace engone;
		float sw = GetWidth(), sh = GetHeight();

		FontAsset* consolas = getStorage()->get<FontAsset>("consolas");
		if (!consolas) {
			log::out << log::RED << "Cannot found text font\n";
			return;
		}
		if (!consolas->valid()) {
			return;
		}

		//-- Constants
		const float titleHeight = 25;
		const float addressHeight = 20;
		const float connectHeight = 16;
		const float errorHeight = 20;
		const float border = 5;

		//-- Title
		ui::TextBox titleBox = { "Client",0,0,titleHeight, consolas,normalTextColor };
		titleBox.x = sw / 2 - titleBox.getWidth() / 2;
		titleBox.y = border;
		ui::Draw(titleBox);

		if (ui::Clicked(titleBox) == 1) {
			launcherState = LauncherServerMenu;
			m_client.stop();
		}

		//-- Download/Start
		ui::TextBox downloadText = { "Download", 0,0,addressHeight, consolas,normalTextColor };

		if (!clientFullyConnected()) {
			// cannot download, launch game instead
			downloadText.text = "Start";
			// Todo: Game running if it is so you don't start it again?
		}

		downloadText.x = 10;
		downloadText.y = sh - downloadText.h - 10;

		ui::Box downloadBack = { 0,0,0,0 ,backBoxColor };
		downloadBack.x = downloadText.x- border;
		downloadBack.y = downloadText.y- border;
		downloadBack.w = downloadText.getWidth()+ border*2;
		downloadBack.h = downloadText.h+ border*2;

		if(ui::Hover(downloadBack)) {
			if (IsKeyDown(GLFW_MOUSE_BUTTON_1)) {
				downloadBack.rgba = backBoxColorClicked;
			} else {
				downloadBack.rgba = backBoxColorHover;
			}
		}

		// Todo: feedback that you clicked
		if (ui::Clicked(downloadBack) == 1) {
			if (clientFullyConnected()) {
				// Download
				download();
			} else {
				// Start game
				bool success = launchGame();
				if (!success) {
					log::out << log::RED << "Failed launching game\n";
				}
			}
		}

		ui::Draw(downloadBack);
		ui::Draw(downloadText);

		//-- Address/Connecting
		ui::TextBox connectText = { "Connecting", 0,0,connectHeight, consolas,normalTextColor };
		connectText.x = 10;
		connectText.y = downloadBack.y - connectText.h - border;
		if (isConnecting) {
			float timeDiff = 0.5;
			int dots = (int)(connectTime / timeDiff) % 6;
			if (dots > 3)
				dots = 6 - dots;
			for (int i = 0; i < dots; i++) {
				connectText.text += '.';
			}
		} else if (m_client.isRunning()) {
			connectText.text = "Connected\n";
		}

		ui::TextBox addressText = { address, 0,0,addressHeight, consolas,normalTextColor };
		addressText.x = 10;
		addressText.y = connectText.y - addressText.h - border*2;
		
		ui::Box addressBack = { 0,0,0,0 ,backBoxColor};
		addressBack.x = addressText.x- border;
		addressBack.y = addressText.y- border;
		addressBack.w = addressText.getWidth()+ border*2;
		addressBack.h = addressText.h+ border*2;

		bool editBefore = editingAddress;
		int clickedAddress = ui::Clicked(addressBack);
		if (clickedAddress == 1) {
			editingAddress = true;
		} else if (clickedAddress==-1) {
			editingAddress = false;
		}
		addressText.editing = editingAddress;
		addressText.at = editAddressAt;

		// prevent some characters like # $, or rather, only allow a-Z, 0-9 and .
		ui::Edit(&addressText);

		// Finished editing, do new action
		if (editBefore && !addressText.editing) {
			connect();
		}

		if (!addressText.editing) {
			editingAddress = false;
		} else {
			editAddressAt = addressText.at;
		}
		address = addressText.text;

		ui::TextBox suggestText = { "",0,addressBack.y,addressBack.h,consolas,hiddenTextColor };

		int index = addressText.text.find_last_of(':');
		if (addressText.editing) {
			if (index == -1)
				suggestText.text = ":" + Settings::PORT;
			else if (index == addressText.text.length() - 1)
				suggestText.text = Settings::PORT;
			addressBack.w += suggestText.getWidth()+ border * 2;
		}
		if (m_client.isRunning()) {
			ui::Draw(connectText);
		}
		ui::Draw(addressBack);
		ui::Draw(addressText);
		if (addressText.editing) {
			suggestText.x = addressText.x + consolas->getWidth(addressText.text, addressText.h);
			ui::Draw(suggestText);
		}
		// autostart (later)
	}
	void LauncherApp::renderDownloading(engone::LoopInfo& info) {
		using namespace engone;
		float sw = GetWidth(), sh = GetHeight();

		const float downloadHeight = 30;
		const float downloadFileHeight = 15;
		ui::Color titleColor = { 1 };
		ui::Color hiddenColor = { 0.7,1 };

		// Download text
		ui::TextBox download = { "Downloading",0,0,downloadHeight,consolas,titleColor };
		float timeDiff = 0.5;
		int dots = (int)(loadingTime / timeDiff) % 6;
		if (dots > 3)
			dots = 6 - dots;
		for (int i = 0; i < dots; i++) {
			download.text += '.';
		}
		download.x = sw / 2 - download.getWidth()/2;
		download.y = topMoveEdge;
		ui::Draw(download);

		downloadMutex.lock();
		//recentDownloads.push_back(std::to_string(info.timeStep));
		int index = recentDownloads.size()-sh/downloadFileHeight;
		if (index < 0)
			index = 0;
		//log::out << "size " << recentDownloads.size() << "\n";
		downloadMutex.unlock();

		int i = 0;
		while(true){
			downloadMutex.lock();
			if (recentDownloads.size() <= index) {
				downloadMutex.unlock();
				break;
			}
			std::string& file = recentDownloads[index];
			downloadMutex.unlock();

			ui::TextBox lastFile = { file, 0,0,downloadFileHeight,consolas,hiddenColor };
			lastFile.x = sw / 2 - lastFile.getWidth() / 2;
			lastFile.y = download.y + download.h+lastFile.h*i;
			ui::Draw(lastFile);

			index++;
			i++;
		}

	}
	void LauncherApp::renderServer(engone::LoopInfo& info) {
		using namespace engone;
		float sw = GetWidth(), sh = GetHeight();
		//ui::Color white = { 1 };
		//ui::Color gray = { 0.8,1 };

		//-- Constants
		const float titleHeight = 25;
		const float addressHeight = 20;
		const float connectHeight = 16;
		const float errorHeight = 20;
		const float border = 5;

		//-- Title
		ui::TextBox titleBox = { "Server",0,0,titleHeight, consolas,normalTextColor };
		titleBox.x = sw / 2 - titleBox.getWidth() / 2;
		titleBox.y = border;
		ui::Draw(titleBox);

		if (ui::Clicked(titleBox) == 1) {
			launcherState = LauncherClientMenu;
			m_server.stop();
		}

		ui::TextBox connectText = { "Connecting", 0,0,connectHeight, consolas,normalTextColor };
		connectText.x = 10;
		connectText.y = sh - connectText.h - border;
		if (isConnecting) {
			float timeDiff = 0.5;
			int dots = (int)(connectTime / timeDiff) % 6;
			if (dots > 3)
				dots = 6 - dots;
			for (int i = 0; i < dots; i++) {
				connectText.text += '.';
			}
		} else if (m_server.isRunning()) {
			connectText.text = "Connected\n";
		}

		ui::TextBox addressText = { address, 0,0,addressHeight, consolas,normalTextColor };
		addressText.x = 10;
		addressText.y = connectText.y - addressText.h - border * 2;

		ui::Box addressBack = { 0,0,0,0 ,backBoxColor };
		addressBack.x = addressText.x - border;
		addressBack.y = addressText.y - border;
		addressBack.w = addressText.getWidth() + border * 2;
		addressBack.h = addressText.h + border * 2;

		bool editBefore = editingAddress;
		int clickedAddress = ui::Clicked(addressBack);
		if (clickedAddress == 1) {
			editingAddress = true;
		} else if (clickedAddress == -1) {
			editingAddress = false;
		}
		addressText.editing = editingAddress;
		addressText.at = editAddressAt;

		// prevent some characters like # $, or rather, only allow a-Z, 0-9 and .
		ui::Edit(&addressText);

		// Finished editing, do new action
		if (editBefore && !addressText.editing) {
			connect();
		}

		if (!addressText.editing) {
			editingAddress = false;
		} else {
			editAddressAt = addressText.at;
		}
		address = addressText.text;

		ui::TextBox suggestText = { "",0,addressBack.y,addressBack.h,consolas,hiddenTextColor };

		int index = addressText.text.find_last_of(':');
		if (addressText.editing) {
			if (index == -1)
				suggestText.text = ":" + Settings::PORT;
			else if (index == addressText.text.length() - 1)
				suggestText.text = Settings::PORT;
			addressBack.w += suggestText.getWidth() + border * 2;
		}
		if (m_server.isRunning()) {
			ui::Draw(connectText);
		}
		ui::Draw(addressBack);
		ui::Draw(addressText);
		if (addressText.editing) {
			suggestText.x = addressText.x + consolas->getWidth(addressText.text, addressText.h);
			ui::Draw(suggestText);
		}

		ui::TextBox serverTitle = { "Connections:" + std::to_string(m_server.getConnectionCount()),0,0,addressHeight,consolas,normalTextColor };
		serverTitle.x = sw - serverTitle.getWidth() - border;
		serverTitle.y = topMoveEdge;
		ui::Draw(serverTitle);

		char buffer[256];

		int sentBytes = m_server.getStats().sentBytes();
		
		float displaySB = 0;
		if (sentBytes < 1024) {
			displaySB = sentBytes;
			snprintf(buffer, sizeof(buffer), "Sent %.2f B", displaySB);
		} else if (sentBytes < 1024*1024) {
			displaySB = sentBytes / 1024.f;
			snprintf(buffer, sizeof(buffer), "Sent %.2f KB", displaySB);
		} else 
			//if (sentBytes < 1024*1024*1024)
			{
			displaySB = sentBytes / 1024.f/1024.f;
			snprintf(buffer, sizeof(buffer), "Sent %.2f MB",displaySB);
		}

		ui::TextBox connectInfo = { buffer,0,0,addressHeight,consolas,normalTextColor };
		connectInfo.x = sw - connectInfo.getWidth()-border;
		connectInfo.y = serverTitle.y + serverTitle.h;
		
		ui::Draw(connectInfo);

		int recvBytes = m_server.getStats().receivedBytes();
		displaySB = 0;
		if (recvBytes < 1024) {
			displaySB = sentBytes;
			snprintf(buffer, sizeof(buffer), "Recv. %.2f B", displaySB);
		} else if (recvBytes < 1024 * 1024) {
			displaySB = recvBytes / 1024.f;
			snprintf(buffer, sizeof(buffer), "Recv. %.2f KB", displaySB);
		} else
			//if (recvBytes < 1024*1024*1024)
		{
			displaySB = recvBytes / 1024.f / 1024.f;
			snprintf(buffer, sizeof(buffer), "Recv. %.2f MB", displaySB);
		}

		ui::TextBox connectInfoR = { buffer,0,0,addressHeight,consolas,normalTextColor };
		connectInfoR.x = sw  - connectInfoR.getWidth()-border;
		connectInfoR.y = connectInfo.y + connectInfo.h;
		ui::Draw(connectInfoR);
	}
}