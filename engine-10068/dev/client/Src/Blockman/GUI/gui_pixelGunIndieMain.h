#pragma once
#ifndef __GUI_PIXEL_GUN_INDIE_GAME_HEADER__
#define __GUI_PIXEL_GUN_INDIE_GAME_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIProgressBar.h"
#include "GUI/GuiActorWindow.h"
#include "GUI/RootGuiLayout.h"
#include "GUIPixelAdInfo.h"
#include "UI/GUILayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_pixelGunIndieMain : public gui_layout
	{
	private:
		i64 m_vip_time_mills = 0;
		int m_gift_bag_time = 0;

		GUIStaticText* m_player_level = nullptr;
		GUIProgressBar* m_player_exp = nullptr;
		GUIStaticText* m_player_exp_text = nullptr;
		GUIStaticText* m_player_hp_text = nullptr;
		GUIStaticText* m_player_defense_text = nullptr;

		GUIStaticImage* m_vip_time = nullptr;
		GUIStaticText* m_vip_time_text = nullptr;

		GUIStaticText* m_currency_text = nullptr;
		GUIStaticText* m_diamondGolds_text = nullptr;
		GUIStaticText* m_key_text = nullptr;

		GUIStaticImage* m_task_red_point = nullptr;
		GUIStaticText* m_task_finished_count = nullptr;

		GUIStaticImage* m_rank_icon = nullptr;
		GUIStaticText* m_rank_num = nullptr;
		GUIStaticText* m_honor_text = nullptr;

		GUIStaticText* m_gift_bag_text = nullptr; 

		GuiActorWindow* m_player_model = nullptr;
		GUIStaticImage* m_store_red_point = nullptr;
		GUIStaticImage* m_defense_red_point = nullptr;

		GUIPixelAdInfo* m_pixel_ad_info = NULL;

	public:
		gui_pixelGunIndieMain();
		~gui_pixelGunIndieMain();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:
		bool onSettingClick(const EventArgs& event);
		bool onEmailClick(const EventArgs& event);
		bool onFriendClick(const EventArgs& event);
		bool onFreeCubeClick(const EventArgs& event);
		bool onAddCubeClick(const EventArgs& event);
		bool onGunStoreClick(const EventArgs& event);
		bool onAppShopClick(const EventArgs& event);
		bool onTaskClick(const EventArgs& event);
		bool onRankClick(const EventArgs& event);
		bool onDefenseClick(const EventArgs& event);
		bool onDressClick(const EventArgs& event);
		bool onChestLotteryClick(const EventArgs& event);
		bool onMoreGameClick(const EventArgs& event);
		bool onFightingClick(const EventArgs& event);
		bool onGiftBagClick(const EventArgs& event); 
		bool onFreeVipClick(const EventArgs& event) { m_pixel_ad_info->showAdInfo(2); return false; };
		bool onFreeChestClick(const EventArgs& event) { m_pixel_ad_info->showAdInfo(1); return false; };

		bool showPixelGunHallInfo(int lv, int cur_exp, int max_exp, int yaoshi, bool is_max);
		bool showPlayerDefense(int curDefense, int maxDefense);
		bool showPlayerHonorInfo(int honorId, int rank, int honor, int endTime);
		bool onUpdatePixekGunTaskCount();
		bool onUpdatePlayerModel();
		bool onPlayerModelChange(i64 userId);
		bool onUpdateVipTime(int vip_time);
		bool onUpdateGiftRefreshTime(int time);
		bool onCubeAdVisible(bool visible);

		bool onStoreUpgradeComplete(int id) { m_store_red_point->SetVisible(true); return true; };
		bool onHideStoreUpgradeRedPoint() { m_store_red_point->SetVisible(false); return true; };
		bool onDefenseUpgradeComplete() { m_defense_red_point->SetVisible(true); return true; };

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};

}
#endif