#include "Convertion.h"

#include <vector>

int main(int argc, char* argv[])
{
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			int err;
			std::string file = argv[i];
			if (file.substr(file.length() - 7) == "rawcoll") {
				err = ConvertCollider(file);
			} else if (file.substr(file.length() - 4) == "coll") {
				err = ReadCollider(file);
			} else if (file.substr(file.length() - 7) == "rawmesh") {
				err = ConvertMesh(file);
			} else if (file.substr(file.length() - 7) == "rawanim") {
				err = ConvertAnim(file);
			} else if (file.substr(file.length() - 4) == "mesh") {
				err = ReadMesh(file);
			} else if (file.substr(file.length() - 4) == "anim") {
				err = ReadAnim(file);
			}
			if (err != Success)
				std::cin.get();
			std::cout << std::endl;
		}
	} else {
		std::cout << "Drag rawmesh/rawanim/mesh/anim file to read" << std::endl;

		bool loop = true;
		while (loop) {
			std::string inp;
			std::cin >> inp;

			if (inp == "exit" || inp == "quit") {
				loop = false;
			} else {
				
				if (inp.substr(inp.length() - 7) == "rawcoll") {
					ConvertCollider(inp);
				} else if (inp.substr(inp.length() - 4) == "coll") {
					ReadCollider(inp);
				} if (inp.substr(inp.length() - 7) == "rawmesh") {
					ConvertMesh(inp);
				} else if (inp.substr(inp.length() - 7) == "rawanim") {
					ConvertAnim(inp);
				} else if (inp.substr(inp.length() - 4) == "mesh") {
					ReadMesh(inp);
				} else if (inp.substr(inp.length() - 4) == "anim") {
					ReadAnim(inp);
				}
			}
			std::cout << std::endl;
		}
	}
}