#pragma once

#ifndef __CUSTOM_ARMOR_SETTINF_HEADER__
#define __CUSTOM_ARMOR_SETTINF_HEADER__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{
	struct CustomArmorSettingInfo
	{
		int uniqueId = 0;
		int itemId = 0;
		int attack = 0;
		int defence = 0;
		int hp = 0;
	};

	class CustomArmorSetting : public Singleton<CustomArmorSetting>, public ObjectAlloc
	{
	private:
		map<int, CustomArmorSettingInfo>::type m_armorInfoMap;

	public:
		CustomArmorSetting();
		~CustomArmorSetting();

		bool getArmorSettingInfo(int itemId, CustomArmorSettingInfo& armorSettingInfo);
		bool isCustomArmorByItemId(int itemId);

		bool loadSetting(bool isClient);
		void unloadSetting();
	};
}

#endif // __CUSTOM_ARMOR_SETTINF_HEADER__
