#include "Launcher/Settings.h"
#include "Engone/Utilities/Utilities.h"
#include "Engone/Logger.h"

namespace launcher {
	// Replace any // or \\ with PATH_DELIM
	void FormatPath(std::string& path) {
		for (int i = 0; i < path.size(); i++) {
			if ((PATH_DELIM=='\\'&&path.data()[i] == '/')|| (PATH_DELIM=='/'&&path.data()[i]=='\\'))
				path.data()[i] = PATH_DELIM;
		}
	}
	std::string Settings::PORT = "5492";
	bool Settings::load(const std::string& path) {
		m_path = path;
		return load();
	}
	bool Settings::load() {
		using namespace engone;
		FileReader reader(m_path);
		if (reader) {
			try {
				std::vector<std::string> list = reader.readLines();
				for (int i = 0; i < list.size(); i++) {
					std::vector<std::string> split = SplitString(list[i], "=");
					if (split.size() == 2) {
						m_map[split[0]] = split[1];
					}
				}
			} catch (Error err) {
				log::out << "Setting::load - " << err << "\n";
				return false;
			}
			if (m_map.find("port") == m_map.end()) {
				m_map["port"] = Settings::PORT;
			}
			return true;
		}
		return false;
	}
	void Settings::save() {
		std::ofstream file(m_path, std::ios::binary);
		if (file) {
			for (auto [key, value] : m_map) {
				file.write(key.c_str(), key.length());
				file.write("=", 1);
				file.write(value.c_str(), value.length());
				file.write("\n", 1);
			}
		}
	}
	std::string Settings::get(const std::string& key) const {
		auto find = m_map.find(key);
		if (find != m_map.end())
			return find->second;
		return "";
	}
	void Settings::set(const std::string& key, const std::string& value) {
		if (key.length() != 0)
			m_map[key] = value;
	}
	void Settings::remove(const std::string& key) {
		if (key.length() != 0)
			m_map.erase(key);
	}
	void GameCache::load() {
		m_files.clear();
		engone::FileReader file(GAME_CACHE_PATH);
		if (file) {
			try {
				uint32_t fileCount;
				file.read(&fileCount);
				for (int i = 0; i < fileCount; i++) {
					FileInfo f;
					file.read(&f.path);
					FormatPath(f.path);
					file.read(&f.time);
					m_files.push_back(f);
				}
			} catch (engone::Error err) {
				engone::log::out << "GameCache::load - " << err << "\n";
			}
		}
	}
	void GameCache::save() {
		engone::FileWriter file(GAME_CACHE_PATH);
		if (file) {
			try {
				uint32_t fileCount = m_files.size();
				file.write(&fileCount);
				for (FileInfo& f : m_files) {
					file.write(&f.path);
					file.write(&f.time);
				}
			} catch (engone::Error err) {
				engone::log::out << "GameCache::save - " << err << "\n";
			}
		}
	}
	void GameCache::set(FileInfo file) {
		// FormatPath sohuld be done before
		for (int i = 0; i < m_files.size();i++) {
			FileInfo& f = m_files[i];
			if (f.path == file.path) {
				f.time = file.time;
				return;
			}
		}
		m_files.push_back(file);
	}
	std::vector<Entry> LoadGameFiles() {
		std::vector<Entry> files;
		engone::FileReader file(GAME_FILES_PATH);
		if (file) {
			try {
				std::vector<std::string> lines = file.readLines();
				for (std::string& line : lines) {
					std::vector<std::string> split = engone::SplitString(line, "=");
					Entry f;
					if (split.size() != 2) {
						engone::log::out << engone::log::RED<< "LoadGameFiles - line must have 1 of '=' : "<<line<<"\n";
						continue;
					}else{
						f.originPath = split[0];
						FormatPath(f.originPath);
						//if (split.size() == 1&&f.originPath.find_last_of(".exe")!=-1) {
						//	f.path = f.originPath.substr(f.originPath.find_last_of('\\') + 1);
						//}
						f.path = split[1];
						FormatPath(f.path);
					}
					files.push_back(f);
				}
			} catch (engone::Error err) {
				engone::log::out << engone::log::RED<<"LoadGameFiles - err? " << err << "\n";
			}
		}
		return files;
	}
}