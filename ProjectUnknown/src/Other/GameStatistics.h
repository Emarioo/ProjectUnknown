#pragma once

#define MAX_INT_STAT 50
#define MAX_FLOAT_STAT 10
#define MAX_STRING_STAT 20

#define INT_STAT_STRIDE 1
#define FLOAT_STAT_STRIDE INT_STAT_STRIDE+MAX_INT_STAT
#define STRING_STAT_STRIDE FLOAT_STAT_STRIDE+MAX_FLOAT_STAT

enum Statistic : unsigned short {
	NONE=0,

	// Integer
	PlayerDeaths=INT_STAT_STRIDE,
	SessionPlayerDeaths,
	PlayerDeathsLastHour,
	TotalPlaytimeSeconds,
	SessionPlaytimeSeconds,
	KilledEnemies,
	CraftedItems,
	UsedMana,
	UsedStamina,
	PlayerLevel,
	PlayerAttackDamage,
	SkippedTutorial,

	// Float
	ClockHour= FLOAT_STAT_STRIDE,
	ClockMinute,
	TotalPlaytimeHours,
	WorldPlaytimeHours,

	// String
	TotalPlaytimeFormatted=STRING_STAT_STRIDE,
	SessionPlaytimeFormatted,
	

};

// Add a string keymap for statistic enum

class GameStatistics
{
public:
	GameStatistics();
	
	void AddStringEnum(const std::string& name,Statistic stat);
	/*
	Get enum stat from string, enum keymap
	*/
	Statistic GetStringEnum(const std::string& name);
	void SetStat(Statistic stat, int value);
	void SetStat(Statistic stat, float value);
	void SetStat(Statistic stat, const std::string& value);
	void SetStat(const std::string& stat, int value);
	void SetStat(const std::string&, float value);
	void SetStat(const std::string&, const std::string& value);
	/*
	Types: "int", "float", "string"
	*/
	std::string GetStatType(Statistic stat);

	int GetIntStat(Statistic stat);
	float GetFloatStat(Statistic stat);
	/*
	Can get int and float stats and turn them into strings
	*/
	std::string GetStringStat(Statistic stat);
private:
	int statI[MAX_INT_STAT];
	float statF[MAX_FLOAT_STAT];
	std::string statS[MAX_STRING_STAT];

	std::unordered_map<std::string, Statistic> keymap;
};
extern GameStatistics gameStatistics;