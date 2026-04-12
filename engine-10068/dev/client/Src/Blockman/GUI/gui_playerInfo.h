#pragma once
#ifndef __GUI_PLAYER_INFO_HEADER__
#define __GUI_PLAYER_INFO_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIListBox.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIStaticText.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUIListHorizontalBox.h"
#include <list>
#include <map>
#include <atomic>


using namespace LORD;

namespace BLOCKMAN
{
	struct buffItem
	{
		int buffName{ 0 };
		int duration{ 0 };
		ui32 lasttime{ 0 };
	};

	class BirdBuff;
	class gui_playerInfo : public  gui_layout
	{
	public:
		gui_playerInfo();
		virtual ~gui_playerInfo();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void resetMaxHealth(float health) {}
		bool updateHeartBar(int hp, int maxHp);
		bool updateDefenseUI(int defense, int maxDefense);
		bool updateAttackUI(int attack, int maxAttack);
		bool updateAppleUI(int cur, int max);
		bool onBirdBuffChange();
		bool changeWDBuffItem(int buffId, int duration);
		void onGetCommonData(String key, String data);
		void setShowEffectLimit(int time) { if (time < 0) return; m_showEffectLimit = time; };

	private:
		bool openEffectInfo(const EventArgs & events);
		bool openWalkingDeathBuffInfo(const EventArgs & events);
		void updateEffectUI();
		void updateHealthUI();
		void updateFoodStatUI();
		void updateArmorUI();
		void updateAirUI();
		void updateAppleFreshUI(ui32 nTimeElapse);
		void updateWaterUI();
		void updateBuffUI(ui32 nTimeElapse);
		bool showAppleFresh(int time);
		void initBirdBuff(const vector<BirdBuff>::type& birdBuffs);
		void showBuffItem(GUIString itemName, int& showCount, int time);
		void showWDBuffItem(GUIString itemName, int& showCount, int time);
		GUIStaticImage* initBuffItem(String itemName,String itemRes);
		GUIStaticImage* initWDBuffItem(String itemName,int buffId, String itemRes);

	private:
		
		SubscriptionGuard m_subscriptionGuard;

		int m_oldAir = 10;
		int m_airFps = 0;
		int m_appleFreshTime = 0;
		
		int m_showEffectLimit = 10000;

		GUILayout* m_airLayout = nullptr;
		GUILayout* m_armorLayout = nullptr;
		GUILayout* m_buffLayout = nullptr;
		GUILayout* m_heartBar = nullptr;
		GUILayout* m_defenseLayout = nullptr;
		GUILayout* m_attackLayout = nullptr;
		GUILayout* m_appleLayout = nullptr;
		GUILayout* m_waterLayout = nullptr;
		GUILayout* m_walkingDeathBuffLayout = nullptr;

		GUIProgressBar* m_health_value = nullptr;
		GUIProgressBar* m_foodstat_value = nullptr;
		GUIProgressBar* m_defense_value = nullptr;
		GUIProgressBar* m_attack_value = nullptr;
		GUIProgressBar* m_apple_value = nullptr;
		GUIProgressBar* m_water_value = nullptr;

		GUIStaticText* m_health_number = nullptr;
		GUIStaticText* m_foodstat_number = nullptr;
		GUIStaticText* m_defense_number = nullptr;
		GUIStaticText* m_attack_number = nullptr;
		GUIStaticText* m_apple_number = nullptr;
		GUIStaticText* m_water_number = nullptr;
		GUIListHorizontalBox* m_birdBuffList = nullptr;

		list<GUIStaticImage*>::type m_listAir;
		list<GUIStaticImage*>::type m_listArmor;
		list<GUIStaticImage*>::type m_listBuff;
		list<GUIStaticImage*>::type m_listWalkingDeathBuff;
		
		static map<int, buffItem>::type m_mapWalkingDeathBuff;
	};
}
#endif