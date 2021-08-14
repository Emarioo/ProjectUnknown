#include "propch.h"

#include "GameStatistics.h"

#include "Engone/DebugTool/DebugHandler.h"

GameStatistics::GameStatistics() {
	// Keymap - double check this
	// int
	AddStringEnum("PlayerDeaths", PlayerDeaths);
	AddStringEnum("SessionPlayerDeaths", SessionPlayerDeaths);
	AddStringEnum("PlayerDeathsLastHour", PlayerDeathsLastHour);
	AddStringEnum("TotalPlaytimeSeconds", TotalPlaytimeSeconds);
	AddStringEnum("SessionPlaytimeSeconds", SessionPlaytimeSeconds);

	AddStringEnum("KilledEnemies", KilledEnemies);
	AddStringEnum("CraftedItems", CraftedItems);
	AddStringEnum("UsedMana", UsedMana);
	AddStringEnum("UsedStamina", UsedStamina);
	AddStringEnum("ClockHour", ClockHour);
	AddStringEnum("ClockMinute", ClockMinute);

	// float
	//AddStringEnum("", );

	// string
	AddStringEnum("TotalPlaytimeFormatted", TotalPlaytimeFormatted);
	AddStringEnum("SessionPlaytimeFormatted", SessionPlaytimeFormatted);

	// Load stats from file?
	SetStat(PlayerDeaths, 9);
	SetStat(SessionPlayerDeaths,5);
	SetStat(PlayerDeathsLastHour, 9);
	SetStat(TotalPlaytimeSeconds, 60);
	SetStat(TotalPlaytimeHours, 5.f);
	SetStat(TotalPlaytimeFormatted, "25 hours 5 minutes 2 seconds");
	SetStat(SessionPlaytimeSeconds, 299);
	SetStat(SessionPlaytimeFormatted, "23 minutes 25 seconds");
	SetStat(ClockHour,5.f);
	SetStat(ClockMinute,2.f);
	SetStat(PlayerLevel,19);
	SetStat(PlayerAttackDamage,19);
	SetStat(SkippedTutorial,1);
	SetStat(WorldPlaytimeHours,.5f);
}
GameStatistics::~GameStatistics() {
	std::cout << "It's about to go!" << std::endl;

}
void GameStatistics::AddStringEnum(const std::string& name, Statistic stat) {
	keymap[name] = stat;
}
Statistic GameStatistics::GetStringEnum(const std::string& name) {
	if (keymap.count(name)>0) {
		return keymap[name];
	}
	return Statistic::NONE;
}
void GameStatistics::SetStat(Statistic stat, int value) {
	statI[stat-INT_STAT_STRIDE] = value;
}
void GameStatistics::SetStat(Statistic stat, float value) {
	statF[stat-FLOAT_STAT_STRIDE] = value;
}
void GameStatistics::SetStat(Statistic stat, const std::string& value) {
	statS[stat-STRING_STAT_STRIDE] = value;
}
void GameStatistics::SetStat(const std::string& stat, int value) {
	statI[GetStringEnum(stat) - INT_STAT_STRIDE] = value;
}
void GameStatistics::SetStat(const std::string& stat, float value) {
	statF[GetStringEnum(stat) - FLOAT_STAT_STRIDE] = value;
}
void GameStatistics::SetStat(const std::string& stat, const std::string& value) {
	statS[GetStringEnum(stat) - STRING_STAT_STRIDE] = value;
}
int GameStatistics::GetIntStat(Statistic stat) {
	return statI[stat-INT_STAT_STRIDE];
}
float GameStatistics::GetFloatStat(Statistic stat) {
	return statF[stat - FLOAT_STAT_STRIDE];
}
std::string GameStatistics::GetStringStat(Statistic stat) {
	if (stat-INT_STAT_STRIDE < MAX_INT_STAT) {
		return std::to_string(statI[stat-INT_STAT_STRIDE]);
	}
	else if (stat-FLOAT_STAT_STRIDE < MAX_FLOAT_STAT) {
		return std::to_string(statF[stat - FLOAT_STAT_STRIDE]);
	}
	else {
		return statS[stat-STRING_STAT_STRIDE];
	}
}
std::string GameStatistics::GetStatType(Statistic stat) {
	if (stat - INT_STAT_STRIDE < MAX_INT_STAT)
		return "int";
	else if (stat - FLOAT_STAT_STRIDE < MAX_FLOAT_STAT)
		return "float";
	else
		return "string";
}
GameStatistics gameStatistics;