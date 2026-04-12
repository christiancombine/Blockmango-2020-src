#pragma once

#ifndef __ADVERT_SETTING_HEADER__
#define __ADVERT_SETTING_HEADER__

#include "Core.h"

using namespace LORD;;

namespace BLOCKMAN
{
	class AdvertSetting : public Singleton<AdvertSetting>, public ObjectAlloc
	{
	private:
		String m_gameType = "g1000";
		map<String, String>::type m_advert_id_map;

		void loadAdsId();

	public:
		AdvertSetting();
		~AdvertSetting();

		void setGameType(String gameType) {
			m_gameType = gameType;
		}
		String getAdsId();
	};
}
#endif