#pragma once

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

		std::string get(const std::string& key) const;
		void set(const std::string& key, const std::string& value);
		void remove(const std::string& key);

		static std::string PORT;
	private:
		std::unordered_map<std::string, std::string> m_map;
		std::string m_path="settings.dat";
	};
	struct FileInfo {
		std::string path;
		uint64_t time;
	};
#define GAME_CACHE_PATH "launcher_cache.dat"
	class GameCache {
	public:
		GameCache() = default;

		void load();
		void save();
		void set(FileInfo file);
		
		inline std::vector<FileInfo>& getFiles() { return m_files; }

	private:
		std::vector<FileInfo> m_files;
	};
	struct FilePath {
		std::string originPath;
		std::string path;
	};

#define GAME_FILES_PATH "gameFiles.dat"
	std::vector<FilePath> LoadGameFiles();
}