#include "Launcher/LauncherApp.h"
#include "Engone/EventModule.h"

#include "../resource.h"

namespace launcher {
	enum LauncherNetType : uint8 {
		LauncherClientFiles, // the files the client has
		LauncherServerSendFile, // the files the client has
		LauncherServerFinished, // server has sent all files
		LauncherServerError,
	};
	const char* ToString(LauncherNetType type) {
		switch (type) {
		case LauncherClientFiles: return "ClientFiles";
		case LauncherServerSendFile: return "ServerSendFile";
		case LauncherServerFinished: return "ServerFinished";
		case LauncherServerError: return "ServerError";
		}
		return "Unknown";
	}
	engone::Logger& operator<<(engone::Logger& logger, LauncherNetType type) {
		return logger << ToString(type);
	}
	enum StreamState : uint8_t {
		StreamStart = 1,
		StreamEnd = 2,
	};
	const char* ToString(StreamState type) {
		if (type == 1) return "Start";
		else if (type==2) return "End";
		else if (type == 3) return "Start|End";
		return "None";
	}
	engone::Logger& operator<<(engone::Logger& logger, StreamState type) {
		return logger << ToString(type);
	}
	LauncherApp::LauncherApp(engone::Engone* engone, LauncherAppInfo& info) : Application(engone) {
		using namespace engone;
		m_window = createWindow({ WindowMode::ModeBorderless,600,240 });
		//m_window = createWindow({ WindowMode::ModeWindowed,600,200 });
		m_window->setTitle("Launcher");

		consolas = getStorage()->set<FontAsset>("consolas", ALLOC_NEW(FontAsset)(IDB_PNG1, "4\n35"));
		
		m_cache.setPath(info.cachePath);
		m_settings.setPath(info.settingsPath);

		bool yes = m_settings.load();

		address = m_settings.get("address");

		if (info.state == LauncherClientMenu) {
			launcherState = info.state;
		} else if(info.state == LauncherServerMenu) {
			launcherState = info.state;
		} else {
			std::string state = m_settings.get("state");
			if (state == "client")
				launcherState = LauncherClientMenu;
			if (state == "server")
				launcherState = LauncherServerMenu;
		}

		m_cache.load();

		m_server.setOnEvent([this](NetEvent e, UUID uuid) {
			m_server.getStats().reset();
			if (NetEvent::Connect == e) {
				//lastError = "";
				//launcherMessage = "Connect "+std::to_string(uuid);
			} else if (NetEvent::Stopped == e) {
				//launcherMessage = "Stopped server";
				//log::out << "Serv Stopped\n";
				//lastError = "";
			} else if (NetEvent::Failed == e) {

			}
			return true;
		});
		m_server.setOnReceive([this](MessageBuffer& buf, UUID uuid) {
			LauncherNetType type;
			buf.pull(&type);
			//log::out << "Server recv " << type << "\n";
			if (type == LauncherClientFiles) {
				recClientFiles(buf, uuid);
			} else {
				log::out << log::RED << "(Server) Received " << type<< " (" << (uint8)type << ") net type\n";
			}

			return true;
		});
		m_client.setOnEvent([this](NetEvent e, UUID uuid) {
			if (NetEvent::Connect == e) {
				isConnecting = false;
				//lastError = "";
			} else if (NetEvent::Failed == e) {
				isConnecting = false;
				lastError = "Could not connect. Is server offline?";
			}
			return true;
		});
		m_client.setOnReceive([this](MessageBuffer& buf, UUID clientUUID) {
			LauncherNetType type;
			buf.pull(&type);
			//log::out<<"Client recv "<<type <<"\n";
			if (type == LauncherServerSendFile) {
				recServerSendFile(buf, clientUUID);
			} else if (type == LauncherServerFinished) {
				recServerFinished(buf, clientUUID);
			} else if (type == LauncherServerError) {
				recServerError(buf, clientUUID);
			} else {
				log::out << log::RED << "(Client) Received " << type << " ("<<(uint8)type << ") net type\n";
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

		if (info.autoConnect) {
			connect();
		} else {
			if (launcherState == LauncherServerMenu)
				setupGamefiles(); // connect will setup files, if we don't auto connect then set them up here
		}
	}
	LauncherApp::~LauncherApp() {
		using namespace engone;
		//for (auto [key, entry] : gameFileEntries) {
		//	if (entry.refresher)
		//		ALLOC_DELETE(FileMonitor, entry.refresher);
		//}
		gameFileEntries.clear();
		fileDownloadsMutex.lock();
		for (auto [key, download] : fileDownloads) {
			if (download.writer)
				ALLOC_DELETE(FileWriter, download.writer);
		}
		fileDownloads.clear();
		fileDownloadsMutex.unlock();
	}
	void LauncherApp::recClientFiles(engone::MessageBuffer& buf, engone::UUID clientUUID) {
		using namespace engone;
		//log::out << "recieved\n";
		std::vector<FileInfo> sentFiles;
		uint32_t size;
		buf.pull(&size);
		for (int i = 0; i < size; i++) {
			FileInfo f;
			buf.pull(f.path);
			buf.pull(&f.time);
			sentFiles.push_back(f);
		}
		
		log::out << "Received " << LauncherClientFiles<< ": Client has " << size;
		if(size==1) log::out << " file in cache\n";
		else log::out << " files in cache\n";

		std::vector<FileSend> sendFiles;

		bool ohNo = false;

		if (gameFileEntries.size() == 0) {
			ohNo = true;
		}

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
			
			sendBuf.push("Game files on server side are wrong");
			lastError = "Game files are wrong";
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
		StreamState state;
		buf.pull(&state);
		FileDownload* fileDownload = nullptr;
		if (state & StreamStart) {
			std::string path;
			buf.pull(path);
			uint64_t time;
			buf.pull(&time);
			uint32 fileSize;
			buf.pull(&fileSize);

			std::string fullPath = root + path;
			try {
				std::filesystem::create_directories(fullPath.substr(0, fullPath.find_last_of(PATH_DELIM)));
			} catch (std::filesystem::filesystem_error err) {
				log::out << log::RED << "Client : ServerSendFile : " << err.what() << "\n";
			}

			FileWriter* writer = ALLOC_NEW(FileWriter)(fullPath, true);
			//log::out << "New writer " << fullPath << "\n";
			//GetTracker().track(writer);
			if (writer->isOpen()) {
				m_cache.set({ path, time });

				downloadMutex.lock();
				recentDownloads.push_back(fileUuid);
				downloadMutex.unlock();

				FileDownload fd = { 0,fileSize,path,writer };
				fileDownloadsMutex.lock();
				auto find = fileDownloads.find(fileUuid);
				if (find == fileDownloads.end()) {
					fileDownload = &fileDownloads[fileUuid];
					*fileDownload = fd;
				} else {
					fileDownload = &find->second;
					if (find->second.writer) {
						ALLOC_DELETE(FileWriter, writer);
						log::out << log::RED << __FILE__ << " FILE UUID EXISTS, deleting FileWriter, replacing FileDownload\n";

					}
					*fileDownload = fd;
				}
			} else {
				ALLOC_DELETE(FileWriter, writer);
				//delete writer;
				//GetTracker().untrack(writer);
				int index = fullPath.find_last_of(".exe");
				if (index != -1) {
					log::out << log::RED <<"Received " << LauncherServerSendFile << " "<< ": Failed writing "<<fullPath<<" (is the exe running?)\n";
					//log::out << log::RED << "Failed writing " << fullPath << ". Is the game running? \n";
				} else {
					log::out << log::RED <<"Received " << LauncherServerSendFile << " " << ": Failed writing "<<fullPath<<"\n";
					//log::out << log::RED << "Failed writing " << fullPath << " \n";
				}
				return;
			}
		} else {
			fileDownloadsMutex.lock();
			auto find = fileDownloads.find(fileUuid);
			if (find == fileDownloads.end()) {
				fileDownloadsMutex.unlock();
				log::out << log::RED << "Received " << LauncherServerSendFile <<": UUID doesn't exist\n";
				return;
			} else {
				fileDownload = &find->second;
			}
		}


		uint32_t transferedBytes;
		char* data = buf.pullBuffer(&transferedBytes);

		fileDownload->downloadedBytes += transferedBytes;
		
		if (fileDownload->writer) {
			char buffer[100];
			snprintf(buffer, sizeof(buffer), "Recv. (%.1f%%) ", (100.f * fileDownload->downloadedBytes / fileDownload->totalBytes));
			log::out << buffer << fileDownload->path << "\n";

			//log::out << "Received " << LauncherServerSendFile << " "<< state<<": " << fileDownload->path <<buffer;

			fileDownload->writer->write(data, transferedBytes);
		} else {
			// it should never be nullptr
			log::out << log::RED << __FILE__ << " WRITER IS NULLPTR!\n";
			DebugBreak();
		}
		if (state & StreamEnd) { // last package
			//GetTracker().untrack(writer);
			//delete writer; // this will also close writer
			//log::out << "del writer " << writer->getPath() << "\n";
			if(fileDownload->writer)
				ALLOC_DELETE(FileWriter, fileDownload->writer);
			fileDownload->writer = nullptr;

			if (fileDownload->downloadedBytes!=fileDownload->totalBytes) {
				log::out << "Downloaded bytes does not match total bytes(" << fileDownload->downloadedBytes << " != " << fileDownload->totalBytes << ")";
			}

			//fileDownloads.erase(fileUuid); // don't delete here, recentDownloads uses it.
		}

		fileDownloadsMutex.unlock();
		//delayDownloadFailed.start(5);
		//if (infoText.text.length() != 0)
		//	infoText.text = "Or not...";

	}
	void LauncherApp::cleanDownloads() {
		using namespace engone;
		fileDownloadsMutex.lock();
		for (auto [key, download] : fileDownloads) {
			if (download.writer)
				ALLOC_DELETE(FileWriter, download.writer);
		}
		fileDownloads.clear();
		fileDownloadsMutex.unlock();
	}
	void LauncherApp::recServerFinished(engone::MessageBuffer& buf, engone::UUID clientUUID){
		using namespace engone;

		log::out << "Received " << LauncherServerFinished << "\n";

		// download is done, save to cache
		m_cache.save();

		launcherState = LauncherClientMenu;
		//delayDownloadFailed.stop();
		bool success = launchGame();
		if (!success) {
			lastError = "Could not launch game";
		}
	}
	void LauncherApp::recServerError(engone::MessageBuffer& buf, engone::UUID clientUUID) {
		using namespace engone;
		//delayDownloadFailed.stop();

		std::string message;
		buf.pull(message);

		log::out << "Received " << LauncherServerError<< ": "<<message<<"\n";

		launcherState = LauncherClientMenu;

		lastError = message;

	}
	void LauncherApp::download() {
		using namespace engone;
		if (!clientFullyConnected())
			return;

		launcherState = LauncherDownloading;
		downloadMutex.lock();
		recentDownloads.clear();
		downloadMutex.unlock();
		downloadTime = 0;

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
		bool success = gameFilesRefresher.check(GAME_FILES_PATH, [this](const std::string& path, uint32 changeType) {
			// should also check for file deletion
			FileReader file(path, false);
			if (file) {
				pendingCalculation = true;
				lastError = "";
				//log::out << "reset " << path << "\n";
				//gameFileEntries.erase("startconfig"); // reset config to replace with new ones
				gameFileEntries.clear();

				auto list = file.readLines();
				for (auto& entry : list) {
					if (entry.empty()) continue;

					std::vector<std::string> split = engone::SplitString(entry, "=");;
					if (split.size() == 2) {
						if (split[0] == "args" || split[0] == "exe") {
							auto find = gameFileEntries.find("startconfig");
							if (find == gameFileEntries.end()) {
								EntryInfo info = { "startconfig", split[0] + "=" + split[1] };
								gameFileEntries["startconfig"] = info;
							} else {
								find->second.destination += "\n";
								find->second.destination += split[0] + "=" + split[1];
							}
						} else {
							auto find = gameFileEntries.find(split[0]);
							if (find == gameFileEntries.end()) {
								EntryInfo info = { split[0], split[1] };
								if (!engone::FindFile(info.source)) {
									log::out << log::RED << "(Game files) Invalid source '" << info.source << "'\n";
									lastError = "Invalid "+info.source;
								} else {
									gameFileEntries[split[0]] = info;
								}

								//info.refresher = ALLOC_NEW(FileMonitor)();
								//info.refresher->check(info.source, [this, source](const std::string& sourcePath, uint32 type) {
								//	EntryInfo& info = gameFileEntries[source];

								//	// send files
								//	FileSend fileSend = { source + "\\" + sourcePath,
								//		info.destination + "\\" + sourcePath,
								//		GetFileLastModified(sourcePath) };
								//	sendFile(fileSend, 0, true);

								//}, FileMonitor::WATCH_SUBTREE);

							} else {
								// entry already exists so do nothing
							}
						}
					} else {
						lastError = "Bad format on line: '" + entry + "'";
						log::out << log::RED << "(Game Files) : Bad format on line: '" << entry << "'\n";
					}
				}
				file.close();
				//printf("GameFiles closed\n");
			} else {
				//log::out << log::RED << "LauncherApp::LauncherApp - access to '" << GAME_FILES_PATH << "' was denied\n";
			}
		}, 0);
		if (success) {
			// manually call it to load the files, this will synchronize files and stuff.
			// you can call this many times if you want
			gameFilesRefresher.getCallback()(GAME_FILES_PATH, 0);
		} else {
			lastError = "Game files have not been setup";
		}
	}
	void LauncherApp::calculateGameFiles() {
		gameFilesCount = 0;
		for (auto [_, entry] : gameFileEntries) {
			std::string& origin = entry.source;
			if (!std::filesystem::is_directory(origin)) {
				gameFilesCount++;
				continue;
			}
			std::filesystem::recursive_directory_iterator itera(origin);

			// PATHS MAY NOT BE DIRECTORIES
			for (auto const& entry : itera) {
				if (entry.is_directory()) continue;
				gameFilesCount++;
			}
		}
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
			StreamState state = (StreamState)(StreamStart | StreamEnd); // arguments is probably not going to be huge so one send is enough.

			uint32 fileSize = f.path.length();

			sendBuf.push(state);
			sendBuf.push(f.fullPath);
			sendBuf.push(f.time);
			sendBuf.push(fileSize);

			char* data = sendBuf.pushBuffer(fileSize);
			memcpy(data, f.path.c_str(), fileSize);

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
					StreamState state = (StreamState)0;
					if (bytesLeft == fileSize) state = (StreamState)(state | StreamStart);
					if (bytesLeft <= m_server.getTransferLimit()) state = (StreamState)(state | StreamEnd);

					sendBuf.push(state);
					if (state & StreamStart) {
						sendBuf.push(f.path);
						sendBuf.push(f.time);
						sendBuf.push(fileSize);
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
				log::out << log::RED << "LauncherApp::sendFile : Cannot read '" << f.fullPath << "'\n";
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
			log::out << log::RED << "Failed starting game. Is cache invalid?\n";
			return false;
		}
		return true;
	}
	void LauncherApp::update(engone::LoopInfo& info) {
		if (isConnecting) {
			connectTime += info.timeStep;
		}
		if (pendingCalculation) {
			pendingCalculation = false;
			calculateGameFiles();
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
			float w = errorText.getWidth();
			float ws = GetWidth() - border * 2;
			if (w > ws) {
				errorText.h = errorHeight / w * ws;
			}
			errorText.x = GetWidth() / 2 - errorText.getWidth() / 2;
			errorText.y = GetHeight() - errorText.getHeight() - border;

			ui::Draw(errorText);
		}
	}
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
		std::string ip = DEFAULT_IP;
		std::string port = DEFAULT_PORT;

		//-- Convert address
		std::vector<std::string> split = engone::SplitString(address, ":");
		if (split.size() >= 1) {
			if(!split[0].empty())
				ip = split[0];
		}
		if (split.size() >= 2) {
			if (!split[1].empty())
				port = split[1];
		}
		if (launcherState == LauncherClientMenu) {
			if (m_client.isRunning()) {
				log::out << "Client is already running\n";
				lastError = "";
			} else {
				if (m_server.isRunning()) {
					m_server.stop();
				}
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
				lastError = "";
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
			setupGamefiles();
		}
	}
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
		const float titleHeight = topMoveEdge * 0.9;
		const float addressHeight = 20;
		const float connectHeight = 16;
		const float errorHeight = 20;
		const float border = 5;

		//-- Title
		ui::TextBox titleBox = { "Client",0,0,titleHeight, consolas,normalTextColor };
		titleBox.x = sw / 2 - titleBox.getWidth() / 2;
		titleBox.y = topMoveEdge / 2 - titleBox.h / 2;;
		ui::Draw(titleBox);

		if (ui::Clicked(titleBox) == 1) {
			launcherState = LauncherServerMenu;
			m_client.stop();
		}

		////-- Download/Start
		//ui::TextBox downloadText = { "Download", 0,0,addressHeight, consolas,normalTextColor };

		//if (!clientFullyConnected()) {
		//	// cannot download, launch game instead
		//	downloadText.text = "Start";
		//	// Todo: Game running if it is so you don't start it again?
		//}

		//downloadText.x = border;
		//downloadText.y = topMoveEdge+border;

		//ui::Box downloadBack = { 0,0,0,0 ,backBoxColor };
		//downloadBack.x = downloadText.x - border;
		//downloadBack.y = downloadText.y - border;
		//downloadBack.w = downloadText.getWidth()+ border*2;
		//downloadBack.h = downloadText.h+ border*2;

		//if(ui::Hover(downloadBack)) {
		//	if (IsKeyDown(GLFW_MOUSE_BUTTON_1)) {
		//		downloadBack.rgba = backBoxColorClicked;
		//	} else {
		//		downloadBack.rgba = backBoxColorHover;
		//	}
		//}

		//// Todo: feedback that you clicked
		//if (ui::Clicked(downloadBack) == 1) {
		//	if (clientFullyConnected()) {
		//		// Download
		//		download();
		//	} else {
		//		// Start game
		//		bool success = launchGame();
		//		if (!success) {
		//			log::out << log::RED << "Failed launching game\n";
		//		}
		//	}
		//}

		//ui::Draw(downloadBack);
		//ui::Draw(downloadText);

		////-- Address/Connecting
		//ui::TextBox connectText = { "Connecting", 0,0,connectHeight, consolas,normalTextColor };
		//connectText.x = border;
		//connectText.y = downloadBack.y + downloadBack.h + border;
		//if (isConnecting) {
		//	float timeDiff = 0.5;
		//	int dots = (int)(connectTime / timeDiff) % 6;
		//	if (dots > 3)
		//		dots = 6 - dots;
		//	for (int i = 0; i < dots; i++) {
		//		connectText.text += '.';
		//	}
		//} else if (m_client.isRunning()) {
		//	connectText.text = "Connected\n";
		//}

		//-- Address text
		ui::TextBox addressDesc = { "Address", 0,0,connectHeight, consolas,normalTextColor };
		addressDesc.x = border;
		addressDesc.y = topMoveEdge + border;

		//-- Address ip/port
		ui::Box addressBack = { 0,0,0,0 ,backBoxColor};
		addressBack.x = border;
		addressBack.y = addressDesc.y + addressDesc.h + border;

		ui::TextBox addressText = { address, 0,0,addressHeight, consolas,normalTextColor };
		addressText.x = addressBack.x + border;
		addressText.y = addressBack.y + border;
		
		addressBack.w = addressText.getWidth()+ border*2;
		addressBack.h = addressText.h+ border*2;

		ui::TextBox suggestText = { "",0,0,addressText.h,consolas,hiddenTextColor };
		suggestText.x = addressText.x + addressText.getWidth();
		suggestText.y = addressText.y;
		if (address.empty()) {
			suggestText.text = std::string(DEFAULT_IP) + ":" + std::string(DEFAULT_PORT);
		} else {
			int index = addressText.text.find_last_of(':');
			if (index == -1)
				suggestText.text = ":" + std::string(DEFAULT_PORT);
			else if (index == addressText.text.length() - 1)
				suggestText.text = DEFAULT_PORT;
		}
		addressBack.w += suggestText.getWidth();

		if (ui::Hover(addressBack)) {
			suggestText.rgba = hiddenTextColorHover;
			addressText.rgba = normalTextColorHover;
		}

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

		//ui::TextBox addressDesc = { "Address", 0,0,connectHeight, consolas,normalTextColor };
		//addressDesc.x = border;
		//addressDesc.y = addressBack.y - addressDesc.h - border;

		//-- Address/Connecting
		ui::TextBox connectText = { "Connecting", 0,0,connectHeight, consolas,normalTextColor };
		connectText.x = border;
		connectText.y = addressBack.y + addressBack.h + border;
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

		//-- Download/Start
		ui::Box downloadBack = { 0,0,0,0 ,backBoxColor };
		downloadBack.x = border;
		downloadBack.y = connectText.y + connectText.h + border;

		ui::TextBox downloadText = { "Download", 0,0,addressHeight, consolas,normalTextColor };

		if (!clientFullyConnected()) {
			// cannot download, launch game instead
			downloadText.text = "Start";
			// Todo: Game running if it is so you don't start it again?
		}

		downloadText.x = downloadBack.x + border;
		downloadText.y = downloadBack.y + border;

		downloadBack.w = downloadText.getWidth() + border * 2;
		downloadBack.h = downloadText.h + border * 2;

		if (ui::Hover(downloadBack)) {
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

		ui::Draw(addressDesc);
		if (m_client.isRunning()) {
			ui::Draw(connectText);
		}
		ui::Draw(addressBack);
		ui::Draw(addressText);
		//if (addressText.editing) {
			ui::Draw(suggestText);
		//}
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
		downloadTime += info.timeStep;
		int dots = (int)(downloadTime / timeDiff) % 6;
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
		char buffer[255];
		while(true){
			downloadMutex.lock();
			if (recentDownloads.size() <= index) {
				downloadMutex.unlock();
				break;
			}
			UUID& uuid = recentDownloads[index];
			fileDownloadsMutex.lock();
			auto find = fileDownloads.find(uuid);
			
			if (find == fileDownloads.end()) {
				fileDownloadsMutex.unlock();
				downloadMutex.unlock();
				continue;
			} else {
				snprintf(buffer, sizeof(buffer),"(%.1f%%) %s", 100.f*find->second.downloadedBytes / find->second.totalBytes,find->second.path.c_str());
				fileDownloadsMutex.unlock();
			}
			downloadMutex.unlock();

			ui::TextBox lastFile = { buffer, 0,0,downloadFileHeight,consolas,hiddenColor };
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
		const float titleHeight = topMoveEdge*0.9;
		const float addressHeight = 20;
		const float connectHeight = 16;
		const float errorHeight = 20;
		const float border = 5;

		//-- Title
		ui::TextBox titleBox = { "Server",0,0,titleHeight, consolas,normalTextColor };
		titleBox.x = sw / 2 - titleBox.getWidth() / 2;
		titleBox.y = topMoveEdge/2-titleBox.h/2;
		ui::Draw(titleBox);

		if (ui::Clicked(titleBox) == 1) {
			launcherState = LauncherClientMenu;
			m_server.stop();
		}

		ui::TextBox addressDesc = { "Address", 0,0,connectHeight, consolas,normalTextColor };
		addressDesc.x = border;
		addressDesc.y = topMoveEdge + border;

		ui::Box addressBack = { 0,0,0,0 ,backBoxColor };
		addressBack.x = border;
		addressBack.y = addressDesc.y + addressDesc.h + border;

		ui::TextBox addressText = { address, 0,0,addressHeight, consolas,normalTextColor };
		addressText.x = addressBack.x + border;
		addressText.y = addressBack.y + border;

		addressBack.w = addressText.getWidth() + border * 2;
		addressBack.h = addressText.h + border * 2;

		ui::TextBox suggestText = { "",0,0,addressText.h,consolas,hiddenTextColor };
		suggestText.x = addressText.x + addressText.getWidth();
		suggestText.y = addressText.y;
		if (address.empty()) {
			suggestText.text = std::string(DEFAULT_IP) + ":" + std::string(DEFAULT_PORT);
		} else {
			int index = addressText.text.find_last_of(':');
			if (index == -1)
				suggestText.text = ":" + std::string(DEFAULT_PORT);
			else if (index == addressText.text.length() - 1)
				suggestText.text = DEFAULT_PORT;
		}
		addressBack.w += suggestText.getWidth();
		//if (addressBack.w < 70)
		//	addressBack.w = 70;

		if (ui::Hover(addressBack)) {
			suggestText.rgba = hiddenTextColorHover;
			addressText.rgba = normalTextColorHover;
		}

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
		
		ui::TextBox connectText = { "Connecting", 0,0,connectHeight, consolas,normalTextColor };
		connectText.x = border;
		connectText.y = addressBack.y + addressBack.h + border;
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

		ui::Draw(addressDesc);
		if (m_server.isRunning()) {
			ui::Draw(connectText);
		}
		ui::Draw(addressBack);
		ui::Draw(addressText);
		//if (addressText.editing) {
			ui::Draw(suggestText);
		//}

		if (!m_server.isRunning())
			return;

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
			displaySB = recvBytes;
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

		snprintf(buffer, sizeof(buffer), "Game Files: %d", gameFilesCount);
		ui::TextBox gameFilesInfo = { buffer,0,0,addressHeight,consolas,normalTextColor };
		gameFilesInfo.x = sw - gameFilesInfo.getWidth() - border;
		gameFilesInfo.y = connectInfoR.y + connectInfoR.h;
		ui::Draw(gameFilesInfo);
	}
}