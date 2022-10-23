#include "Launcher/LauncherApp.h"
#include "Engone/EventModule.h"
#include <windows.h>

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

		consolas = getStorage()->set<FontAsset>("consolas", new FontAsset(IDB_PNG1, "4\n35"));

		bool yes = m_settings.load(settings);
		std::string someIp = m_settings.get("ip");
		std::string somePort = m_settings.get("port");

		//-- Setup updater
		gameFilesRefresher.check(GAME_FILES_PATH,[this](const std::string& path){
			// should also check for file deletion

			// printf("GameFiles reading\n");
			FileReader file(path,false);
			if (file) {
				auto list = file.readLines();
				for (auto& entry : list) {
					std::vector<std::string> split = engone::SplitString(entry, "=");;
					if (split.size() == 2) {
						if (split[0] == "arguments"|| split[0] == "autostart") {
							EntryInfo info = { split[0], split[1], nullptr};
							gameFileEntries[split[0]] = info;
						} else {
							auto find = gameFileEntries.find(split[0]);
							if (find == gameFileEntries.end()) {
								EntryInfo info = { split[0], split[1], new FileMonitor() };
								gameFileEntries[split[0]] = info;
								std::string source = info.source;

								info.refresher->check(info.source, [this, source](const std::string& sourcePath) {
									log::out << "Refresh " << sourcePath << "\n";
									EntryInfo& info = gameFileEntries[source];
									std::string dst = info.destination +
										sourcePath.substr(info.source.length());

									// send files
									FileSend fileSend = { sourcePath,
										dst,
										GetFileLastModified(sourcePath) };

									sendFile(fileSend, 0, true);
								}, FileMonitor::WATCH_SUBTREE);
							}
							else {
								// entry already exists so do nothing
							}
						}
					} else {
						log::out << log::RED<< "LauncherApp::LauncherApp - format wrong: " << entry << "\n";
					}
				}
				file.close();
				//printf("GameFiles closed\n");
			} else {
				log::out << log::RED << "LauncherApp::LauncherApp - access to '" << GAME_FILES_PATH << "' was denied\n";
			}
		},0);
		// just call it
		gameFilesRefresher.getCallback()(GAME_FILES_PATH);

		if (!someIp.empty()) {
			addressText.text = someIp;
		}
		if (!somePort.empty()) {
			if(somePort!=Settings::PORT)
				addressText.text += somePort;
		}

		m_cache.load();

		m_server.setOnEvent([this](NetEvent e, UUID uuid) {
			if (NetEvent::Connect == e) {
				//launcherMessage = "Connect "+std::to_string(uuid);
			} else if (NetEvent::Stopped == e) {
				//launcherMessage = "Stopped server";
			}
			return true;
		});
		m_server.setOnReceive([this](MessageBuffer buf, UUID uuid) {
			//log::out << "1 buf size " << buf.size() << "\n";
			LauncherNetType type;
			buf.pull(&type);
			ENGONE_DEBUG(log::out << "Server recv " << type << "\n", LAUNCHER_LEVEL, 1);
			if (type == LauncherClientFiles) {
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

				for (auto [_,entry] : gameFileEntries) {
					std::string& origin = entry.source;
					std::string& endPath = entry.destination;

					printf("Entry %s\n", origin.c_str());
					if (origin == "arguments") {
						sendFiles.push_back({ origin,endPath,0 });
					} else if (origin == "autostart") {
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
				if(!ohNo)
					for (int i = 0; i < sendFiles.size(); i++) {
						sendFile(sendFiles[i],uuid,false);
					}

				// Make a log of messages. infoText will show how the last connection went but not the rest since infoText will override that info.
				if (ohNo) {
					infoText.text = "GameFile Paths are probably wrong";
					MessageBuffer sendBuf;

					LauncherNetType type = LauncherServerError;
					sendBuf.push(&type);
					m_server.send(sendBuf, uuid);
				} else {
					infoText.text = "";
					if (sendFiles.size() == 0) {
						//log::out << "Server: client is up to date\n";
					}
					MessageBuffer sendBuf;
					LauncherNetType type = LauncherServerFinished;
					sendBuf.push(&type);
					m_server.send(sendBuf, uuid);
				}
			} else {
				log::out << log::RED << "Received invalid type\n";
			}

			return true;
		});
		m_client.setOnEvent([this](NetEvent e, UUID uuid) {
			if (NetEvent::Connect == e) {
				switchState(LauncherDownloading);
				delayDownloadFailed.start(8);

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
			} else if (NetEvent::Failed == e) {
				infoText.text = "Could not connect\n";
				//switchInfoText = "Could not connect\n";
				switchState(LauncherSetting);
			}
			return true;
		});
		m_client.setOnReceive([this](MessageBuffer buf, UUID) {
			LauncherNetType type;
			buf.pull(&type);
			ENGONE_DEBUG(log::out<<"Client recv "<<type <<"\n",LAUNCHER_LEVEL,1);
			if (type == LauncherServerSendFile) {
				UUID fileUuid;
				buf.pull(&fileUuid);
				StreamStates state;
				buf.pull(&state);
				FileWriter* writer = nullptr;
				if (state&StreamStart) {
					std::string path;
					buf.pull(path);
					uint64_t time;
					buf.pull(&time);

					//printf("got %s", path.c_str());

					std::string fullPath = root + path;
					try {
						std::filesystem::create_directories(fullPath.substr(0, fullPath.find_last_of(PATH_DELIM)));
					} catch (std::filesystem::filesystem_error err) {
						log::out << log::RED <<"Client recv "<< err.what() << "\n";
					}

					writer = new FileWriter(fullPath,true);
					GetTracker().track(writer);
					if (writer->isOpen()) {
						lastDownloadedFile = path;
						fileDownloads[fileUuid] = writer;
						m_cache.set({ path, time });
					} else {
						delete writer;
						GetTracker().untrack(writer);
						int index = fullPath.find_last_of(".exe");
						if (index != -1) {
							log::out << log::RED << "Failed writing " << fullPath << ". Is the game running? \n";
						} else {
							log::out << log::RED << "Failed writing " << fullPath << " \n";
						}
						return true;
					}
				} else {
					auto find = fileDownloads.find(fileUuid);
					if (find != fileDownloads.end()) {
						writer = find->second;
					} else {
						// error message is already logged.
						return true;
					}
				}

				uint32_t transferedBytes;
				char* data = buf.pullBuffer(&transferedBytes);

				writer->write(data, transferedBytes);

				if (state &StreamEnd) { // last package
					GetTracker().untrack(writer);
					delete writer; // this will also close writer

					fileDownloads.erase(fileUuid);
				}

				delayDownloadFailed.start(5);
				if (infoText.text.length() != 0)
					infoText.text = "Or not...";

			} else if (type == LauncherServerFinished) {
				// download is done, save to cache
				m_cache.save();
				delayDownloadFailed.stop();

				if (!launchGame()) {
					//switchInfoText = "Developer issues?\n";
					infoText.text = "Developer issues?\n";
					switchState(LauncherSetting);
				}
				//m_window->close();
			} else if (type == LauncherServerError) {
				delayDownloadFailed.stop();
				m_client.stop();

				//switchInfoText = "Server didn't want to cooperate";
				infoText.text = "Server didn't want to cooperate";
				switchState(LauncherSetting);
			}
			return true;
		});

		// This makes the top area of the window draggable
		m_window->attachListener(EventClick | EventMove, [this](Event& e) {
			if (e.eventType == EventClick) {
				if (e.button == GLFW_MOUSE_BUTTON_1) {
					if (e.action == 1) {
						if (e.my < 25 && m_window->hasFocus()) {
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
		});
		// ISSUE: without this, the app doesn't act how i want. it says connecting... when it's not even trying to.
		doAction(true);
	}
	void LauncherApp::sendFile(FileSend& f, engone::UUID clientUUID, bool exclude) {
		using namespace engone;
		printf("Send %s\n", f.fullPath.c_str());
		if (f.fullPath == "arguments") {
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

			m_server.send(sendBuf, clientUUID,exclude);
		} else if (f.fullPath == "autostart") {
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

					//log::out << "Size " << sendBuf.size() << "\n";

					m_server.send(sendBuf, clientUUID);

					bytesLeft -= transferBytes;
				}
				file.close();
			}
		}
	}
	bool LauncherApp::launchGame() {
		using namespace engone;
		return false; // don't launch

		FileReader file1(root + "autostart", false);
		if (file1) {
			std::string startPath;
			file1.readAll(&startPath);
			file1.close();

			startPath = root + startPath;

			bool startedProgram = false;
			FileReader file(root + "arguments", false);
			if (file) {
				std::string args;
				file.readAll(&args);
				file.close();
				startedProgram = StartProgram(startPath, args.data());
			} else {
				startedProgram = StartProgram(startPath);
			}
			if (!startedProgram) {
				log::out << log::RED << "LauncherApp::launchGame - could not start exe\n";
				return false;
			}else
				return true;
		} else {
			log::out << log::RED << "LauncherApp::launchGame - autostart file is missing\n";
			return false;
		}
	}
	void LauncherApp::update(engone::UpdateInfo& info) {
		loadingTime += info.timeStep;
		if (delaySwitch.run(info)) {
			switchState(LauncherSetting);
		}
		if (delayDownloadFailed.run(info)) {
			infoText.text = "Sorry to break it to you, but downloading failed\n";
		}
	}
	void LauncherApp::render(engone::RenderInfo& info) {
		using namespace engone;

		//-- Menu / Colors
		float width = GetWidth(), height = GetHeight();

		ui::Color backColor = { 52 / 255.f,23 / 255.f,109 / 255.f,1 };
		ui::Color lightBackColor = { 52 / 255.f * 1.2,23 / 255.f * 1.2,109 / 255.f * 1.2,1 };
		ui::Color white = { 1,1,1,1 };
		ui::Color gray = { 0.7,0.7,0.7,1 };

		ui::Box background = { 0,0,width,height, backColor };
		ui::Draw(background);
		
		ui::Box quitBack = { width - 25,0,25,25,{0.5,0.05,0.1,1} };
		ui::TextBox quit = { "X", quitBack.x+5,3,20,consolas,{1,1,1,1}};

		if (ui::Hover(quitBack)) {
			quitBack.rgba.r = 0.7;
			quitBack.rgba.g = 0.4;
			quitBack.rgba.b = 0.4;
		}
		ui::Draw(quitBack);
		ui::Draw(quit);
		if (ui::Clicked(quitBack)==1) {
			m_window->close();
		}

		//-- State dependent
		if (state == LauncherConnecting) {
			// Launcher name
			ui::TextBox launcherText = { launcherName,0,0,30,consolas,white };
			launcherText.x = width / 2 - consolas->getWidth(launcherName, launcherText.h) / 2;
			launcherText.y = height / 2 - launcherText.h / 2;
			ui::Draw(launcherText);

			// Connecting info, should also show other info, like connection failed
			infoText.text = "Connecting";
			infoText.y = launcherText.y + launcherText.h;
			infoText.h = launcherText.h * 0.7;
			infoText.x = width / 2 - consolas->getWidth("Connecting..", infoText.h) / 2;
			infoText.font = consolas;
			infoText.rgba = gray;
			float timeDiff = 0.5;
			int dots = (int)(loadingTime / timeDiff) % 6;
			if (dots > 3)
				dots = 6 - dots;
			for (int i = 0; i < dots; i++) {
				infoText.text += '.';
			}
			ui::Draw(infoText);
		} 
		else if (state == LauncherDownloading) {
			// Download text
			ui::TextBox download = { "Downloading",0,height * 0.3,30,consolas,white };
			float timeDiff = 0.5;
			int dots = (int)(loadingTime / timeDiff) % 6;
			if (dots > 3)
				dots = 6 - dots;
			for (int i = 0; i < dots; i++) {
				download.text += '.';
			}
			download.x = width / 2 - consolas->getWidth("Downloading..", download.h) / 2;
			ui::Draw(download);

			// Last downloaded file
			ui::TextBox lastFile = { lastDownloadedFile,0,download.y + download.h,download.h * 0.9,consolas,gray };
			lastFile.x = width / 2 - consolas->getWidth(lastDownloadedFile, lastFile.h) / 2;
			ui::Draw(lastFile);

			if (infoText.text.length() != 0) {
				infoText.h = lastFile.h * 0.8;
				infoText.font = consolas;
				infoText.rgba = gray;
				infoText.y = lastFile.y + lastFile.h + height * 0.08;
				infoText.x = width / 2 - consolas->getWidth(infoText.text, infoText.h) / 2;
				ui::Draw(infoText);
			}
		} 
		else if (state == LauncherSetting) {
			ui::TextBox serverText = { "Address",0,0,30,consolas,white };
			serverText.x = width / 2 - consolas->getWidth(serverText.text, serverText.h)/2;
			serverText.y = height * 0.25-serverText.h/2;
			ui::Draw(serverText);

			ui::Box addressBack = { 0,0,0,40,lightBackColor };
			addressBack.w = width*0.9;
			addressBack.y = serverText.y + serverText.h+height*0.03;
			addressBack.x = width/2 -addressBack.w/2;
			ui::Draw(addressBack);

			bool editBefore = addressText.editing;
			if (ui::Clicked(addressBack)==1) {
				addressText.editing = true;
				editBefore = true;
			} else if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)||!info.window->hasFocus()) {
				addressText.editing = false;
			}

			// prevent some characters like # $, or rather, only allow a-Z, 0-9 and .
			ui::Edit(&addressText);
			
			ui::TextBox suggestText = { "",0,addressBack.y,addressBack.h,consolas,gray };

			int index = addressText.text.find_last_of(':');
			if (addressText.editing) {
				if (index == -1)
					suggestText.text = ":" + Settings::PORT;
				else if (index == addressText.text.length() - 1)
					suggestText.text = Settings::PORT;
			}

			addressText.h = addressBack.h;
			addressText.x = addressBack.x+addressBack.w/2-consolas->getWidth(addressText.text+suggestText.text,addressText.h)/2;
			addressText.y = addressBack.y;
			addressText.font = consolas;
			addressText.rgba = white;

			ui::Draw(addressText);

			if (addressText.editing) {
				suggestText.x = addressText.x + consolas->getWidth(addressText.text, addressText.h);
				ui::Draw(suggestText);
			}

			// Changing settings based on input
			if (addressText.editing != editBefore && editBefore) {
				interpretAddress();
				doAction();
			}

			if (infoText.text.length() != 0) {
				infoText.h = addressText.h * 0.5;
				infoText.font = consolas;
				infoText.rgba = gray;
				infoText.y = addressBack.y + addressBack.h + height * 0.08;
				infoText.x = width / 2 - consolas->getWidth(infoText.text, infoText.h) / 2;
				ui::Draw(infoText);
			}
		} 
		else if (state == LauncherServerSide) {
			ui::TextBox serverTitle = { "Server connections:"+std::to_string(m_server.getConnectionCount()),0,height * 0.07,35,consolas,white };
			serverTitle.x = width / 2 - consolas->getWidth(serverTitle.text, serverTitle.h) / 2;
			ui::Draw(serverTitle);

			const uint32_t bufferSize = 150;
			char infoBuffer[bufferSize];
			ZeroMemory(infoBuffer, bufferSize);
			uint32_t write = 0;
			
			write+=snprintf(infoBuffer, bufferSize, "Bytes S/R: %llu/%llu\nMsgs S/R: %llu/%llu", m_server.getStats().sentBytes(), m_server.getStats().receivedBytes(), m_server.getStats().sentMessages(), m_server.getStats().receivedMessages());
			infoBuffer[bufferSize - 1] = 0;
			ui::TextBox connectInfo = { infoBuffer,0,serverTitle.y + serverTitle.h,serverTitle.h * 0.7,consolas,white};
			float wid = consolas->getWidth(connectInfo.text, connectInfo.h);
			connectInfo.x = width / 2-wid / 2;
			ui::Draw(connectInfo);
			
			// Add more info here, like how many bytes server received and sent.

			if (infoText.text.length() != 0) {
				infoText.h = serverTitle.h * 0.5;
				infoText.y = connectInfo.y + connectInfo.h*2 + height * 0.08;
				infoText.x = width / 2 - consolas->getWidth(infoText.text, infoText.h) / 2;
				infoText.font = consolas;
				infoText.rgba = gray;
				ui::Draw(infoText);
			}
		}
	}
	void LauncherApp::interpretAddress() {
		std::vector<std::string> split = engone::SplitString(addressText.text, ":");
		if (split.size() == 0) {
		} else if (split.size() <= 2) {
			m_settings.set("ip", split[0]);
			if (split.size() == 2) {
				if (split[1].length() != 0)
					m_settings.set("port", split[1]);
				else
					m_settings.set("port", Settings::PORT);
			} else {
				m_settings.set("port", Settings::PORT);
			}
		}
	}
	void LauncherApp::switchState(LauncherState newState) {
		state = newState;
		loadingTime = 0;
		//infoText.text = switchInfoText;
		//switchInfoText = "";
		if (state == LauncherSetting) {
			std::string ip = m_settings.get("ip");
			std::string port = m_settings.get("port");
			if (ip.length() != 0) {
				if (port.length() != 0 && port != Settings::PORT) {
					addressText.text = ip + ":" + port;
				} else {
					addressText.text = ip;
				}
			} else {
				if (port.length() != 0 && port != Settings::PORT) {
					addressText.text = ":" + port;
				} else {
					addressText.text = "";
				}
			}
		}
	}
	void LauncherApp::onClose(engone::Window* window) {
		interpretAddress();
		m_settings.save();

		m_server.stop();
		m_client.stop();
		stop();
	}
	void LauncherApp::doAction(bool delay) {
		using namespace engone;
		std::string ip = m_settings.get("ip");
		std::string port = m_settings.get("port");
		if (ip == "force") {
			if (!launchGame()) {
				//m_window->close();
				//switchInfoText = "Cache is invalid";
				infoText.text = "Cache is invalid. Connect at least once.";
				switchState(LauncherSetting);
			} else {
				// what should be done if successful?
			}
		}else if (ip == "host") {
			if (m_server.isRunning()) {
				log::out << "Server is already running\n";
			} else {
				if (m_client.isRunning())
					m_client.stop();
				if (!m_server.start(port)) {
					infoText.text = "Invalid Port";
					//switchInfoText = "Invalid Port";
					if (delay)
						delaySwitch.start(1);
					else
						switchState(LauncherSetting);
				} else {
					infoText.text = "";
					switchState(LauncherServerSide);
				}
			}
		} else {
			if (m_client.isRunning()) {
				log::out << "Client is already running\n";
			} else {
				if (m_server.isRunning())
					m_server.stop();
				if (!m_client.start(ip, port)) {
					// infoText can be ambiguous, was the ip invalid because the server didn't exist and the client couldn't connect or
					// because the ip wasn't a correctly formatted ip address. 12#515Ar <- this would be an invalid ip.
					//switchInfoText= "Invalid IP/Port";
					infoText.text = "Invalid IP/Port";
					if (delay) {
						delaySwitch.start(1);
					} else
						switchState(LauncherSetting);
				} else {
					infoText.text = "";
					switchState(LauncherConnecting);
				}
			}
		}
	}
}