
#include <random>

#include "InformativeTips.h"

#include "Engone/Utility/Utilities.h"

namespace infotips
{
	Criteria::Criteria(Statistic stat, float f0, float f1)
		: stat(stat), f0(f0), f1(f1)
	{
	}
	Criteria::Criteria(Statistic stat, const std::string& equal)
		: stat(stat), equal(equal)
	{
	}
	Criteria::~Criteria() {}
	bool Criteria::IsFulfilled()
	{
		if (equal.empty()) {
			if (gamestats::GetStatType(stat) == "int") {
				int num = gamestats::GetIntStat(stat);
				//std::cout << "Num" << num << " " << ((max >= min) ? min < num&& num < max : min < num || num < max) << std::endl;
				return (f1 >= f0) ? f0 < num&& num < f1 : f0 < num || num < f1;
			}
			else {
				float num = gamestats::GetFloatStat(stat);
				return (f1 >= f0) ? f0 < num&& num < f1 : f0 < num || num < f1;
			}
		}
		else {
			return gamestats::GetStringStat(stat) == equal;
		}
	}

	Tip::Tip(const std::string& text)
		: text(text)
	{
	}
	Tip::Tip(const std::string& text, Criteria crit1)
		: text(text)
	{
		AddCritera(crit1);
	}
	Tip::Tip(const std::string& text, Criteria crit1, Criteria crit2)
		: text(text)
	{
		AddCritera(crit1);
		AddCritera(crit2);
	}
	Tip::Tip(const std::string& text, Statistic stat1)
		: text(text)
	{
		AddStat(stat1);
	}
	Tip::Tip(const std::string& text, Statistic stat1, Criteria crit1)
		: text(text)
	{
		AddStat(stat1);
		AddCritera(crit1);
	}
	Tip::Tip(const std::string& text, Statistic stat1, Criteria crit1, Criteria crit2)
		: text(text)
	{
		AddStat(stat1);
		AddCritera(crit1);
		AddCritera(crit2);
	}
	Tip::Tip(const std::string& text, Statistic stat1, Statistic stat2)
		: text(text)
	{
		AddStat(stat1);
		AddStat(stat2);
	}
	Tip::Tip(const std::string& text, Statistic stat1, Statistic stat2, Criteria crit1)
		: text(text)
	{
		AddStat(stat1);
		AddStat(stat2);
		AddCritera(crit1);
	}
	Tip::Tip(const std::string& text, Statistic stat1, Statistic stat2, Criteria crit1, Criteria crit2)
		: text(text)
	{
		AddStat(stat1);
		AddStat(stat2);
		AddCritera(crit1);
		AddCritera(crit2);

	}
	Tip::~Tip() {}
	std::string Tip::GetTip()
	{
		std::vector<std::string> split = engone::SplitString(text, "[x]");
		for (size_t i = 0; i < split.size() - 1; i++) {
			if (stats.size() > i)
				split[0] += gamestats::GetStringStat(stats[i]) + split[i + 1];
			else
				split[0] += split[i + 1];
		}
		return split[0];
	}
	void Tip::AddStat(Statistic stat)
	{
		stats.push_back(stat);
	}
	Tip Tip::AddCritera(Criteria criteria)
	{
		criterias.push_back(criteria);
		return *this;
	}
	bool Tip::IsFulfilled()
	{
		for (Criteria& c : criterias) {
			if (!c.IsFulfilled())
				return false;
		}
		return true;
	}
	std::mt19937 global_gen;
	std::uniform_real_distribution global_dis;
	void Init()
	{
		global_gen.seed((unsigned int)engone::GetSystemTime());
		// Death tips
		AddDeathTip({ "You have died [x] times.", PlayerDeaths });
		AddDeathTip({ "Are you raging because you have died [x] times the last hour.",PlayerDeathsLastHour,{PlayerDeathsLastHour,7,0} });
		AddDeathTip({ "Did you forget what Kevin taught you?", { TotalPlaytimeHours,0.f, 2.f } });
		AddDeathTip({ "Good night fellow human." });
		AddDeathTip({ "Fortunately for you. You are immortal." });
		AddDeathTip({ "Maybe you shouldn't have skipped the tutorial.",{WorldPlaytimeHours,0.f,1.f}, {SkippedTutorial,0,2} });

		// Other tips
		AddTip({ "You have played for [x].",SessionPlaytimeFormatted });
		AddTip({ "You have killed [x] enemies.",KilledEnemies });
		AddTip({ "You have crafted [x] items.",CraftedItems });
		AddTip({ "You have used a total of [x] mana.",UsedMana });
		AddTip({ "You have used a total of [x] stamina.",UsedStamina });
		AddTip({ "Did you know this game uses OpenGL for rendering?" });
		AddTip({ "Did you know this game uses OpenAL for sound?" });
		AddTip({ "Did you know this game uses ASIO for multiplayer?" });
		AddTip({ "Did you know this game is coded C++?" });
		AddTip({ "Did you know this game used models from Blender?" });
		AddTip({ "Did you know the creator of this game learnt to snap his fingers when he was 17 years old?" });
		AddTip({ "Good morning.",{ClockHour,7.f,13.f} });
		AddTip({ "Good day.",{ClockHour,13.f,18.f} });
		AddTip({ "Good evening.",{ClockHour,18.f,24.f} });
		AddTip({ "It is midnight...",{ClockHour,0.f,1.f} });
		AddTip({ "Should you not be sleeping?",{ClockHour,1.f,7.f} });
		AddTip({ "You are level [x]. You're welcome.",PlayerLevel });
		AddTip({ "Hello new player.",{TotalPlaytimeHours,0,3.f} });
		AddTip({ "Hello intermediate player.",{TotalPlaytimeHours,3.f,15.f} });
		AddTip({ "Hello long time player.",{TotalPlaytimeHours,15.f,40.f} });
		AddTip({ "Hello knowledgable player.",{TotalPlaytimeHours,40.f,0.f} });
		AddTip({ "Hello experienced player.",{PlayerLevel,49,0} });
		AddTip({ "Hello fearsome player.",{PlayerAttackDamage,99,0} });
		AddTip({ "Do you remember Kevin? I don't.",{TotalPlaytimeHours,40.f,0.f} });
	}
	static std::vector<Tip> deathTips;
	static std::vector<Tip> tips;
	Tip& AddDeathTip(Tip tip)
	{
		deathTips.push_back(tip);
		return deathTips.back();
	}
	std::string InformativeTips_GetTip(std::vector<Tip>& tips)
	{
		double totalChance = 0;
		for (auto& m : tips) {
			if (m.IsFulfilled())
				totalChance += m.chance;
		}

		double random = global_dis(global_gen) * totalChance;
		double at = 0;
		int index = 0;
		for (size_t j = 0; j < tips.size(); j++) {
			//if (!tips[j].IsFulfilled()) continue;
			index = j;
			if (at < random && random < at + tips[j].chance)
				break;
			at += tips[j].chance;
		}
		at = tips[index].chance;

		// Increase the chance of the tips
		for (size_t j = 0; j < tips.size(); j++) {
			tips[j].chance += at / (tips.size() - 1);
		}
		tips[index].chance = 0;

		return tips[index].GetTip();
	}
	std::string GetDeathTip()
	{
		return InformativeTips_GetTip(deathTips);
	}
	Tip& AddTip(Tip tip)
	{
		tips.push_back(tip);
		return tips.back();
	}
	std::string GetTip()
	{
		return InformativeTips_GetTip(tips);
	}
	/*
	engine::CountingTest(10000, 6, [&](){
			std::string n = informativeTips.GetDeathTip();
			return n[0]-'A';
		});
	*/
}