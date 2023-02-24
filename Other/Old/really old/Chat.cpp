#include "Chat.h"

std::vector<GameObject*>* objects;
void InitChat(std::vector<GameObject*>* o) {
	objects = o;
}
char CHAT_CMD_CHAR() {
	return '#';
}
/* Commands
help : give list of commands

*/

void ChatExecuteCommand(std::string s) {

}