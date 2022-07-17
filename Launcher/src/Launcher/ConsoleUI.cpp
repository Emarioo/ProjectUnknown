// Networking was copied from engone 2022-07-12

#include "Engone/Logger.h"
#include "Launcher/Settings.h"

void main() {
	using namespace engone;
	log::out << "Starting launcher...\n";
	Settings map = Settings::Load();

	while (true) {
		log::out << "Aquiring new game version\n";
		std::string ip = map.get(SETTING_IP);
		if (ip.length() != 0) {
			// try to connect
		} else {
			log::out << "Missing IP\n";
		}
		// IP
		log::out << "1. Change IP";
		if (ip.length() != 0)
			log::out << " (" << ip << ")";
		log::out << "\n";
		// PORT
		std::string port = map.get(SETTING_PORT);
		log::out << "2. Change Port";
		if (port.length() != 0)
			log::out << " (" << port << ")";
		log::out << "\n";
		// Connect
		log::out << "3. Connect\n";
		// Exit
		log::out << "4. Exit\n";
		// IS SERVER
		std::string isServer = map.get(SETTING_IS_SERVER);
		if (isServer.length() != 0) {
			if (isServer == "true") {
				log::out << "5. Disable Server\n";
			} else {
				log::out << "5. Enable Server\n";
			}
		}
		// CHOICE
		char chr = std::getchar();
		if (chr == '1') {
			log::out << "Enter Ip:\n";
			std::string str;
			std::getline(std::cin, str);
		} else if (chr == '2') {

		} else if (chr == '3') {

		} else if (chr == '4') {
			break;
		} else if (chr == '5') {

		}
	}

	std::cin.get();


}