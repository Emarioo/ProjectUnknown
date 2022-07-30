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
		m_window = createWindow({ WindowMode::BorderlessMode,600,150 });
		m_window->setTitle("Launcher");

		//m_window->close();
		//consolas = getAssets()->set<Font>("consolas", "fonts/consolas42");

		PNG png = LoadPNG(IDB_PNG1);
		const char* txtBuffer = "4\n35";

		consolas = getAssets()->set<Font>("consolas", new Font(png.data, png.size, txtBuffer, strlen(txtBuffer)));

		bool yes = m_settings.load(settings);

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
			LauncherNetType type;
			buf.pull(&type);
			if (type == LauncherClientFiles) {
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

				struct FileSend {
					std::string fullPath;// where server finds the file
					std::string path; // where client should put file
					uint64_t time;
				};
				std::vector<FileSend> sendFiles;

				auto gameFiles = LoadGameFiles();

				bool ohNo = false;

				for (int i = 0; i < gameFiles.size(); i++) {
					std::string& origin = gameFiles[i].originPath;
					std::string& endPath = gameFiles[i].path;

					bool isDir = std::filesystem::is_directory(origin); // if origin is invalid, this will be false.

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
							log::out << log::RED << "file not found\n";
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

				for (int i = 0; i < sendFiles.size(); i++) {
					FileSend& f = sendFiles[i];
					std::ifstream file(f.fullPath, std::ios::binary);
					if (file) {
						file.seekg(0, file.end);
						uint32_t fileSize = file.tellg();
						file.seekg(0, file.beg);

						//log::out << "SEND "<<f.path << "\n";

						UUID fileUuid = UUID::New();
						uint32_t bytesLeft = fileSize;
						while (bytesLeft!=0) {
							MessageBuffer sendBuf(LauncherServerSendFile);
							sendBuf.push(fileUuid);
							StreamStates state = 0;
							if (bytesLeft == fileSize) state = state|StreamStart;
							if (bytesLeft <= m_server.getTransferLimit()) state = state|StreamEnd;

							sendBuf.push(state);
							if (state&StreamStart) {
								sendBuf.push(f.path);
								sendBuf.push(f.time);
							}

							uint32_t headerSize = sendBuf.size() + sizeof(uint32_t); // <- for transferBytes in pushBuffer

							uint32_t transferBytes = std::min(bytesLeft,m_server.getTransferLimit()-headerSize);
							
							char* data = sendBuf.pushBuffer(transferBytes);
							file.read(data, transferBytes);

							//log::out << "Size " << sendBuf.size() << "\n";

							m_server.send(sendBuf, uuid);

							bytesLeft -= transferBytes;
						}
						file.close();
					}
				}
				// Make a log of messages. infoText will show how the last connection went but not the rest since infoText will override that info.
				if (ohNo) {
					infoText.text = "GameFile Paths are probably wrong";
					MessageBuffer sendBuf(LauncherServerError);
					m_server.send(sendBuf, uuid);
				} else {
					infoText.text = "";
					if (sendFiles.size() == 0) {
						//log::out << "Server: client is up to date\n";
					}
					MessageBuffer sendBuf(LauncherServerFinished);
					m_server.send(sendBuf, uuid);
				}
			} else {
				log::out << log::RED << "Received invalid type\n";
			}

			return true;
		});
		m_client.setOnEvent([this](NetEvent e) {
			if (NetEvent::Connect == e) {
				switchState(LauncherDownloading);
				delayDownloadFailed.start(8);

				std::vector<FileInfo>& files = m_cache.getFiles();

				MessageBuffer buf(LauncherClientFiles);
				buf.push((uint32_t)files.size());
				for (int i = 0; i < files.size(); i++) {
					buf.push(files[i].path);
					buf.push(files[i].time);
				}

				m_client.send(buf);
			} else if (NetEvent::Failed == e) {
				switchInfoText = "Could not connect\n";
				switchState(LauncherSetting);
			}
			return true;
		});
		m_client.setOnReceive([this](MessageBuffer buf) {
			LauncherNetType type;
			buf.pull(&type);
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

					std::string fullPath = root + path;
					try {
						std::filesystem::create_directories(fullPath.substr(0, fullPath.find_last_of(PATH_DELIM)));
					} catch (std::filesystem::filesystem_error err) {
						log::out << log::RED << err.what() << "\n";
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
				std::string exePath;
				delayDownloadFailed.stop();

				if (m_cache.getFiles().size() != 0) {
					if (m_cache.getFiles()[0].path.find_last_of(".exe") != -1) {
						exePath = root + m_cache.getFiles()[0].path;
					}
				}
				//log::out << "Client: finished ," << exePath << "\n";
				if (!exePath.empty()) {
					//StartProgram(exePath);

					m_window->close();
				} else {
					//log::out << log::RED << "First file was not executable\n";
					infoText.text = "Developer messed up which order to send files\n";
				}
			} else if (type == LauncherServerError) {
				m_client.disconnect();
				switchInfoText = "Server didn't want to cooperate";
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
		//doAction(true);
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

		float width = GetWidth(), height = GetHeight();

		Font* font = consolas;

		ui::Color backColor = { 52 / 255.f,23 / 255.f,109 / 255.f,1 };
		ui::Color lightBackColor = { 52 / 255.f * 1.2,23 / 255.f * 1.2,109 / 255.f * 1.2,1 };
		ui::Color white = { 1,1,1,1 };
		ui::Color gray = { 0.7,0.7,0.7,1 };

		ui::Box background = { 0,0,width,height, backColor };
		ui::Draw(background);
		
		ui::Box quitBack = { width - 25,0,25,25,{0.5,0.05,0.1,1} };
		ui::TextBox quit = { "X", quitBack.x+5,3,20,font,{1,1,1,1}};

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

		if (state == LauncherConnecting) {
			// Launcher name
			ui::TextBox launcherText = { launcherName,0,0,30,font,white };
			launcherText.x = width / 2 - font->getWidth(launcherName, launcherText.h) / 2;
			launcherText.y = height / 2 - launcherText.h / 2;
			ui::Draw(launcherText);

			// Connecting info, should also show other info, like connection failed
			infoText.text = "Connecting";
			infoText.y = launcherText.y + launcherText.h;
			infoText.h = launcherText.h * 0.7;
			infoText.x = width / 2 - font->getWidth("Connecting..", infoText.h) / 2;
			infoText.font = font;
			infoText.rgba = gray;
			float timeDiff = 0.5;
			int dots = (int)(loadingTime / timeDiff) % 6;
			if (dots > 3)
				dots = 6 - dots;
			for (int i = 0; i < dots; i++) {
				infoText.text += '.';
			}
			ui::Draw(infoText);
		} else if (state == LauncherDownloading) {
			// Download text
			ui::TextBox download = { "Downloading",0,height * 0.3,30,font,white };
			float timeDiff = 0.5;
			int dots = (int)(loadingTime / timeDiff) % 6;
			if (dots > 3)
				dots = 6 - dots;
			for (int i = 0; i < dots; i++) {
				download.text += '.';
			}
			download.x = width / 2 - font->getWidth("Downloading..", download.h) / 2;
			ui::Draw(download);

			// Last downloaded file
			ui::TextBox lastFile = { lastDownloadedFile,0,download.y + download.h,download.h * 0.9,font,gray };
			lastFile.x = width / 2 - font->getWidth(lastDownloadedFile, lastFile.h) / 2;
			ui::Draw(lastFile);

			if (infoText.text.length() != 0) {
				infoText.h = lastFile.h * 0.8;
				infoText.font = font;
				infoText.rgba = gray;
				infoText.y = lastFile.y + lastFile.h + height * 0.08;
				infoText.x = width / 2 - font->getWidth(infoText.text, infoText.h) / 2;
				ui::Draw(infoText);
			}
		} else if (state == LauncherSetting) {
			ui::TextBox serverText = { "Address",0,0,30,font,white };
			serverText.x = width / 2 - font->getWidth(serverText.text, serverText.h)/2;
			serverText.y = height * 0.25-serverText.h/2;
			ui::Draw(serverText);

			ui::Box addressBack = { 0,0,0,40,lightBackColor };
			addressBack.w = width*0.9;
			addressBack.y = serverText.y + serverText.h+height*0.03;
			addressBack.x = width/2 -addressBack.w/2;
			ui::Draw(addressBack);


			bool editBefore = addressText.editing;
			if (ui::Clicked(addressBack)) {
				addressText.editing = true;
				editBefore = true;
			} else if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)||!info.window->hasFocus()) {
				addressText.editing = false;
			}

			// prevent some characters like # $, or rather, only allow a-Z, 0-9 and .
			ui::Edit(&addressText);
			
			ui::TextBox suggestText = { "",0,addressBack.y,addressBack.h,font,gray };

			int index = addressText.text.find_last_of(':');
			if (addressText.editing) {
				if (index == -1)
					suggestText.text = ":" + Settings::PORT;
				else if (index == addressText.text.length() - 1)
					suggestText.text = Settings::PORT;
			}

			addressText.h = addressBack.h;
			addressText.x = addressBack.x+addressBack.w/2-font->getWidth(addressText.text+suggestText.text,addressText.h)/2;
			addressText.y = addressBack.y;
			addressText.font = font;
			addressText.rgba = white;

			ui::Draw(addressText);

			if (addressText.editing) {
				suggestText.x = addressText.x + font->getWidth(addressText.text, addressText.h);
				ui::Draw(suggestText);
			}

			// Changing settings based on input
			if (addressText.editing != editBefore && editBefore) {
				interpretAddress();
				doAction();
			}

			if (infoText.text.length() != 0) {
				infoText.h = addressText.h * 0.5;
				infoText.font = font;
				infoText.rgba = gray;
				infoText.y = addressBack.y + addressBack.h + height * 0.08;
				infoText.x = width / 2 - font->getWidth(infoText.text, infoText.h) / 2;
				ui::Draw(infoText);
			}
		} else if (state == LauncherServerSide) {
			ui::TextBox serverTitle = { "Server connections:"+std::to_string(m_server.getConnectionCount()),0,height * 0.07,35,font,white };
			serverTitle.x = width / 2 - font->getWidth(serverTitle.text, serverTitle.h) / 2;
			ui::Draw(serverTitle);

			const uint32_t bufferSize = 150;
			char infoBuffer[bufferSize];
			ZeroMemory(infoBuffer, bufferSize);
			uint32_t write = 0;
			
			write+=snprintf(infoBuffer, bufferSize, "Bytes S/R: %llu/%llu\nMsgs S/R: %llu/%llu", m_server.sentBytes(), m_server.receivedBytes(),m_server.sentMessages(), m_server.receivedMessages());
			infoBuffer[bufferSize - 1] = 0;
			ui::TextBox connectInfo = { infoBuffer,0,serverTitle.y + serverTitle.h,serverTitle.h * 0.7,font,white};
			float wid = font->getWidth(connectInfo.text, connectInfo.h);
			connectInfo.x = width / 2-wid / 2;
			ui::Draw(connectInfo);
			
			// Add more info here, like how many bytes server received and sent.

			if (infoText.text.length() != 0) {
				infoText.h = serverTitle.h * 0.5;
				infoText.y = connectInfo.y + connectInfo.h*2 + height * 0.08;
				infoText.x = width / 2 - font->getWidth(infoText.text, infoText.h) / 2;
				infoText.font = font;
				infoText.rgba = gray;
				ui::Draw(infoText);
			}
		}
	}
	void LauncherApp::interpretAddress() {
		std::vector<std::string> split = engone::SplitString(addressText.text, ":");
		if (split.size() == 0) {
		} else if (split.size() <= 2) {
			if (split[0] == "host") {
				m_settings.set("isServer", "true");
			} else {
				m_settings.remove("isServer");
			}
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
		infoText.text = switchInfoText;
		switchInfoText = "";
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
		m_client.disconnect();
	}
	void LauncherApp::doAction(bool delay) {
		using namespace engone;
		std::string ip = m_settings.get("ip");
		std::string port = m_settings.get("port");
		std::string isServer = m_settings.get("isServer");
		if (isServer == "true") {
			if (m_server.isRunning()) {
				log::out << "Server is already running\n";
			} else {
				if (m_client.isRunning())
					m_client.disconnect();
				if (!m_server.start(port)) {
					switchInfoText = "Invalid Port";
					if (delay)
						delaySwitch.start(1);
					else
						switchState(LauncherSetting);
				} else {
					switchState(LauncherServerSide);
				}
			}
		} else {
			if (m_client.isRunning()) {
				log::out << "Client is already running\n";
			} else {
				if (m_server.isRunning())
					m_server.stop();
				if (!m_client.connect(ip, port)) {
					// infoText can be ambiguous, was the ip invalid because the server didn't exist and the client couldn't connect or
					// because the ip wasn't a correctly formatted ip address. 12#515Ar <- this would be an invalid ip.
					switchInfoText= "Invalid IP/Port";
					if (delay) {
						delaySwitch.start(1);
					} else
						switchState(LauncherSetting);
				} else {
					switchState(LauncherConnecting);
				}
			}
		}
	}
}