#pragma once
#include "gui_layout.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "GUI/GuiActorWindow.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;
using namespace GameClient;

namespace BLOCKMAN
{
	class gui_pixelgungameresult : public gui_layout
	{
	private:
		int m_show_cup_time = 0;
		int m_show_title_time = 0;
		int m_show_content_time = 0;
		int m_move_score_time = 0;
		int m_hide_cup_time = 0;

		Vector2 m_score_start_pos = { 0.0f, 0.0f };
		Vector2 m_score_end_pos = { 0.0f, 0.0f };

	public:

		gui_pixelgungameresult();
		virtual ~gui_pixelgungameresult();
		void onLoad() override;
		bool isNeedLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onGetCommonData(String key, String data);

		enum ResultType
		{
			RT_1V1 = 0,
			RT_PERSON = 1,
			RT_TEAM = 2,
		};

		enum TeamType
		{
			TT_BLUE = 1,
			TT_RED = 2,
		};

		bool refreshAll(const String& data, int result_type);
		bool get1v1Info(const String& data);
		bool getPersonInfo(const String& data);
		bool getTeamInfo(const String& data);
		bool refreshSpecialInfo(bool btnRevengeEnable);

		static const int MAX_REWARD_NUM = 4;
		static const int MAX_DATA_ITEM_NUM = 16;
		static const int MAX_COLLUME_NUM = 8;

		struct DataCommon
		{
			int reward_gold_num = 0;
			String reward_img[MAX_REWARD_NUM];
			int reward_num[MAX_REWARD_NUM];
			bool show_ad = false;
			bool is_win = false;
		};

		struct Data1v1
		{
			String blue_duan_img = "";
			int blue_lv = 0;
			String blue_name = "";
			int blue_kills = 0;
			String red_duan_img = "";
			int red_lv = 0;
			String red_name = "";
			int red_kills = 0;
			int self_team = 0;
			int win_team = 0;
			bool revenge_btn_enable = true;
			int min_money = 0;
		};

		struct DataPerson
		{
			int person_rank[MAX_DATA_ITEM_NUM];
			String person_duan_img[MAX_DATA_ITEM_NUM];
			int person_lv[MAX_DATA_ITEM_NUM];
			String person_name[MAX_DATA_ITEM_NUM];
			int person_kills[MAX_DATA_ITEM_NUM];
			int self_rank = 0;
		};

		struct DataTeam
		{
			int blue_rank[MAX_COLLUME_NUM];
			String blue_duan_img[MAX_COLLUME_NUM];
			int blue_lv[MAX_COLLUME_NUM];
			String blue_name[MAX_COLLUME_NUM];
			int blue_kills[MAX_COLLUME_NUM];
			int red_rank[MAX_COLLUME_NUM];
			String red_duan_img[MAX_COLLUME_NUM];
			int red_lv[MAX_COLLUME_NUM];
			String red_name[MAX_COLLUME_NUM];
			int red_kills[MAX_COLLUME_NUM];
			int self_team = 0;
			int self_rank = 0;
			int blue_team_score = 0;
			int red_team_score = 0;
			int win_team = 0;
		};

	private:

		SubscriptionGuard							m_subscriptionGuard;

		bool onNextClick(const EventArgs & events);
		bool onRevbengeClick(const EventArgs & events);
		bool onBackClick(const EventArgs & events);
		bool onShowAdClick(const EventArgs & events);

		void showResultCup();
		void showTitleAndScore();
		void showResultContent();
		void moveScore();
		void hideResultCup();

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}

		DataCommon m_data_common;
		Data1v1 m_data_1v1;
		DataPerson m_data_person;
		DataTeam m_data_team;

		GuiActorWindow* m_pixel_actor = nullptr;
		GUIStaticImage* m_pixel_score_bg = nullptr;
		GUIStaticText* m_pixel_score_text = nullptr;
		GUIStaticText* m_pixel_score_move = nullptr;

		GUILayout* m_pixel_content = nullptr;
		GUIStaticText* m_pixel_title = nullptr;
		GUIStaticText* m_pixel_blue_team_score = nullptr;
		GUIStaticText* m_pixel_red_team_score = nullptr;
		GUIStaticText* m_pixel_self_gold_txt = nullptr;
		GUIStaticImage* m_pixel_reward_img[MAX_REWARD_NUM];
		GUIStaticText* m_pixel_reward_num[MAX_REWARD_NUM];
		GUIButton* m_pixel_back_btn = nullptr;
		GUIButton* m_pixel_next_btn = nullptr;
		GUIButton* m_pixel_revenge_btn = nullptr;
		GUIButton* m_pixel_show_ad = nullptr;
		GUIStaticText* m_pixel_blue_team_title = nullptr;
		GUIStaticText* m_pixel_red_team_title = nullptr;
		GUIStaticImage* m_pixel_blue_team_title_bg = nullptr;
		GUIStaticImage* m_pixel_red_team_title_bg = nullptr;
		GUIStaticImage* m_pixel_blue_icon = nullptr;
		GUIStaticImage* m_pixel_red_icon = nullptr;
		GUIStaticImage* m_pixel_data_item_bg[MAX_DATA_ITEM_NUM];
		GUIStaticText* m_pixel_data_item_rank_num[MAX_DATA_ITEM_NUM];
		GUIStaticImage* m_pixel_data_item_duan[MAX_DATA_ITEM_NUM];
		GUIStaticText* m_pixel_data_item_lv[MAX_DATA_ITEM_NUM];
		GUIStaticText* m_pixel_data_item_name[MAX_DATA_ITEM_NUM];
		GUIStaticText* m_pixel_data_item_kill[MAX_DATA_ITEM_NUM];

		bool8 m_isNextServer = true;
	};
}

