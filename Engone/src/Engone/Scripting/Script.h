#pragma once

namespace engone {
	// A script object which can run javascript code.
	class Script {
	public:
		Script() = default;
		Script(const std::string& path);

		// run the current script
		void run();
		// load/reload a script from file, WILL NOT run the script.
		void reload(const std::string& path = "");

	private:
		std::string m_source;
		std::string m_path;

		static void ReadScript(const std::string& path, Script* script);
	};
}