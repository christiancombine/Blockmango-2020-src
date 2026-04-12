#include "AdvertSetting.h"

namespace BLOCKMAN
{
	AdvertSetting::AdvertSetting()
	{
		m_advert_id_map = {};
		loadAdsId();
	}

	AdvertSetting::~AdvertSetting()
	{
		m_advert_id_map.clear();
	}

	void AdvertSetting::loadAdsId()
	{
		//Common Test Id
		m_advert_id_map["g1000"] = "test";
		//饥饿游戏广告位g1001
		m_advert_id_map["g1001"] = "SurvivalGames";
		//起床广告位g1008 g1046
		m_advert_id_map["g1008"] = "BedWar";
		m_advert_id_map["g1046"] = "BedWar";
		//警匪广告位g1014
		m_advert_id_map["g1014"] = "JailBreak";
		//大亨广告位g1025
		m_advert_id_map["g1025"] = "HeroTycoon";
		//空岛乱斗广告位g1027
		m_advert_id_map["g1027"] = "SkyRoyale";
		//躲猫猫2广告位 g1037 g1038 g1039
		m_advert_id_map["g1037"] = "HideAndSeek2";
		m_advert_id_map["g1038"] = "HideAndSeek2";
		m_advert_id_map["g1039"] = "HideAndSeek2";
		//小鸟广告位g1041
		m_advert_id_map["g1041"] = "BirdSimulator";
		//像素射击大厅g1042
		m_advert_id_map["g1042"] = "PixelGunHall";
		//像素射击游戏g1043,g1044,g1045,g1053
		m_advert_id_map["g1043"] = "PixelGunGame";
		m_advert_id_map["g1044"] = "PixelGunGame";
		m_advert_id_map["g1045"] = "PixelGunGame";
		m_advert_id_map["g1053"] = "PixelGunGame";
		//家园广告位g1047
		m_advert_id_map["g1047"] = "RealmCity";
		//空岛生存广告位g1048,g1049,g1050
		m_advert_id_map["g1048"] = "SkyBlock";
		m_advert_id_map["g1049"] = "SkyBlock";
		m_advert_id_map["g1050"] = "SkyBlock";
		//健身游戏广告位g1055
		m_advert_id_map["g1055"] = "LiftingSimulator";
	}

	String AdvertSetting::getAdsId()
	{
		auto AdsId = m_advert_id_map.find(m_gameType);
		if (AdsId != m_advert_id_map.end())
		{
			return AdsId->second;
		}
		return "test";
	}
}

