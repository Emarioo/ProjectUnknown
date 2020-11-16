#pragma once

#include <string>
#include <vector>

#include "Objects/GameObject.h"

void InitChat(std::vector<GameObject*>* o);
char CHAT_CMD_CHAR();
void ChatExecuteCommand(std::string s);