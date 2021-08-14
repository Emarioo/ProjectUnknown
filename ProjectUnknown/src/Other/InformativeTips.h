#pragma once

#include "GameStatistics.h"

class Criteria {
private:
	Statistic stat;
	float f0 = 0;
	float f1 = 0;
	std::string equal;
public:
	/*
	Works for both integer and float stats
	*/
	Criteria(Statistic stat,float min, float max);
	Criteria(Statistic stat,const std::string& equal);
	~Criteria();

	bool IsFulfilled();
};

class Tip {
private:
	std::vector<Statistic> stats;
	std::vector<Criteria> criterias;
public:
	std::string text;
	Tip(const std::string& text);
	Tip(const std::string& text, Criteria crit1);
	Tip(const std::string& text, Criteria crit1, Criteria crit2);
	Tip(const std::string& text, Statistic stat1);
	Tip(const std::string& text, Statistic stat1, Criteria crit1);
	Tip(const std::string& text, Statistic stat1, Criteria crit1, Criteria crit2);
	Tip(const std::string& text, Statistic stat1, Statistic stat2);
	Tip(const std::string& text, Statistic stat1, Statistic stat2, Criteria crit1);
	Tip(const std::string& text, Statistic stat1, Statistic stat2, Criteria crit1, Criteria crit2);
	~Tip();
	std::string GetTip();
	/*
	The argument will replace [x] in the text.
	*/
	void AddStat(Statistic stat);
	/*
	The criterias have to be fulfilled if the Tip is going to be used.
	*/
	Tip AddCritera(Criteria crit);
	bool IsFulfilled();
	double chance=1;
	/*
	Criteria numerical stat between two values
	*/
};

class InformativeTips {
private:
	std::vector<Tip> deathTips;
	std::vector<Tip> tips;
public:
	InformativeTips();
	~InformativeTips();
	Tip& AddDeathTip(Tip tip);
	std::string GetDeathTip();
	Tip& AddTip(Tip tip);
	std::string GetTip();
};
extern InformativeTips informativeTips;