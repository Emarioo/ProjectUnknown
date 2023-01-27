#pragma once

#include "Launcher/Defaults.h"

namespace launcher {
#define PATH_DELIM '\\'
	void FormatPath(std::string& path);
	class Settings {
	public:
		// will not to anything
		Settings() = default;

		// Will load settings file, and set default values
		bool load(const std::string& path);
		bool load();
		// Will save settings file
		void save();

		void setPath(const std::string& path);

		std::string get(const std::string& key) const;
		void set(const std::string& key, const std::string& value);
		void remove(const std::string& key);

	private:
		std::unordered_map<std::string, std::string> m_map;
		std::string m_path;
	};
	struct FileInfo {
		std::string path;
		uint64_t time;
	};
	class GameCache {
	public:
		GameCache() = default;

		void load();
		void save();
		void set(FileInfo file);
		
		void setPath(const std::string& path);

		inline std::vector<FileInfo>& getFiles() { return m_files; }

	private:
		std::string m_path;
		std::vector<FileInfo> m_files;
	};
}