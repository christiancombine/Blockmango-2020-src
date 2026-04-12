#pragma once
#ifndef __PIXEL_GUN_REWARD_SETTING_HEADER__
#define __PIXEL_GUN_REWARD_SETTING_HEADER__

#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum RewardType
	{
		REWARD_GUN_CHIP = 1,
		REWARD_GUN,
		REWARD_PROP_CHIP,
		REWARD_PROP,
		REWARD_MONEY,
		REWARD_KEY,
		REWARD_VIP,
		REWARD_EXP,
		REWARD_CHEST,
		REWARD_HONOR_UP,

		REWARD_RUNE = 101,
		REWARD_DRESS_CHIP = 102,
		REWARD_DRESS = 103,
		REWARD_PRIVILEGED_CARD = 104,
	};

	class Reward
	{
	public:
		int Id = 0;
		String Title = "";
		String Name = "";
		String Image = "";
		String Desc = "";
		int Count = 0;
		String CountText = "";
		RewardType Type;
	};

	class RewardSetting
	{
	private:
		static map<int, Reward*>::type m_reward_map;

	public:
		static void	loadSetting();
		static void	unloadSetting();

		static Reward* findRewardById(int id);
		static list<Reward*>::type findRewardByIds(String ids);

	private:
		static void loadRewardSetting();
	};
}

#endif