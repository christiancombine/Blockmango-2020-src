#ifndef __GUI_SKY_BLOCK_MAIN_HEADER__
#define __GUI_SKY_BLOCK_MAIN_HEADER__

#include "gui_layout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISkyBlockMain : public gui_layout
	{
	public:
		GUISkyBlockMain();
		~GUISkyBlockMain();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;

		static const int SKY_BLOCK_LEVEL_MAX = 5;

		void onShowSkyBlockLoadingRewards(bool loading);

	private:
		GUIStaticText* m_player_level = nullptr;
		GUIProgressBar* m_player_exp = nullptr;
		GUIStaticText* m_player_exp_text = nullptr;
		GUIStaticText* m_player_area_text = nullptr;
		GUIStaticImage* m_task_red = nullptr;
		GUIStaticImage* m_mian_red = nullptr;
		GUILayout* m_areaLayout = nullptr;
		GUILayout* m_homeLayout = nullptr;
		GUILayout* m_listLayout = nullptr;
		GUIButton* m_sky_back_btn = nullptr;
		GUIButton* m_btn = nullptr;
		GUIButton* m_sky_task_btn = nullptr;
		GUIButton* m_sky_rank_btn = nullptr;
		GUIStaticImage* m_option_bg = nullptr;
		GUIStaticImage* m_level_img[SKY_BLOCK_LEVEL_MAX];
		GUIButton*      m_jiantou = nullptr;
		GUILayout* m_option = nullptr;
		GUILayout* m_effect_layout = nullptr;

		
	private:
		
		bool OnClickResourceIsland(const EventArgs & events);
		bool onAreaClick(const EventArgs & events);
		bool onIslandClick(const EventArgs & events);
		bool OnClickArrow(const EventArgs & events);
		bool showSkyBlockInfo(int lv, i64 cur_exp, i64 max_exp, int island_lv, String area, bool is_max, bool isMaster);
	
		bool onUpdateSkyBlockTaskCount();
		bool OnClickOption();
		bool OnClickRankIcon(const EventArgs & events);
		bool OnClickTaskIcon(const EventArgs & events);
		bool OnClickAddHome();
		bool OnClickBackHome();
		
		bool OnOpenAppShop();
		bool OnOpenPrivilegeShop();
		bool onBackClick(const EventArgs & events);
		bool onOpenParty(const EventArgs & events);


	};
}

#endif
