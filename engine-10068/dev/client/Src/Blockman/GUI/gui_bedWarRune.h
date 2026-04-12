#ifndef __GUI_BED_WAR_RUNE_HEADER__
#define __GUI_BED_WAR_RUNE_HEADER__

#include "gui_layout.h"
#include <vector>

using namespace LORD;
namespace LORD {
	class GUIButton;
	class GUIListBox;
	class GUIRadioButton;
	class GUILayout;
	class GUIGridView;
	class GUIStaticImage;
	class GUIStaticText;
	class GUIListHorizontalBox;
}
namespace BLOCKMAN
{
	class RootGuiLayout;
	
	class gui_bedwar_rune : public gui_layout
	{
	public:
		gui_bedwar_rune();
		~gui_bedwar_rune();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

		static const int MAX_RUNE_TAB_NUM = 2;
		static const int MAX_RUNE_RECALIM_TAB_NUM = 5;
		static const int REFRESH_RUNE_LIST_TIME = 100;
		static const int EFFECT_TIME = 4000;
		static const int EFFECT_START_TIME = 1000;

		enum AssembleOpenType
		{
			AOT_DEFAULT = 0,
			AOT_PRO = 1,
			AOT_RUNE_LIST = 2,
		};

		enum RUNE_TAB
		{
			RT_ASSEMBLE = 0,
			RT_RECLAIM = 1,
		};

		enum RuneType
		{
			RT_INVALID = 0,
			RT_ATTACK = 1,
			RT_DEFENCE = 2,
			RT_SKILL = 3,
			RT_FUNC = 4,
			RT_COUNT = 5,
		};

		struct RuneTroughItem
		{
			int rune_trough_id = 0;
			int rune_id = 0;
			bool rune_lock = true;
			bool need_show_effect = false;
		};

		struct RuneItem
		{
			int rune_id = 0;
			int rune_num = 0;
		};

	private:
		bool onCloseClick(const EventArgs & events);
		bool onLeftImpl();
		bool onRightImpl();
		void moveRuneTroughType(bool left);
		bool onBatchReclaimClick(const EventArgs & events);
		bool onProSumChangeClick(const EventArgs & events);
		bool onOneKeyUnloadImpl();
		bool onOneKeyAssembleImpl();
		bool onRuneChangeClick(const EventArgs & events);
		bool refreshAll(const String& data);
		void refreshGridView(int type);
		bool getAllInfo(const String& data, std::vector<RuneTroughItem> &rune_trough_list, std::vector<RuneItem> &rune_list);
		void refreshRuneTypeList();
		void refreshRuneList();
		void refreshRuneProList(int select_index = 0);
		bool onRadioChange(const EventArgs & events, int index);
		bool onReclaimRadioChange(const EventArgs & events, int index);
		void initRuneTrough();
		bool troughSelectChange(int rune_trough_id, int rune_id);
		bool runeClick(int rune_id);
		bool runeReclaimClick(int rune_id);
		bool onShowRuneListImpl();
		bool onShowProSumImpl();
		void clearRuneTroughSelect();
		void setRuneTroughMove(bool sign);
		void calEquipRuneLevel();
		bool nextSelectTrough();

	private:
		RootGuiLayout * getParent();

		GUIListHorizontalBox* m_runeTypeList = nullptr;
		GUIListBox* m_runeListBox = nullptr;
		GUIListBox* m_runeProListBox = nullptr;
		GUIRadioButton* m_tab_radio_btn[MAX_RUNE_TAB_NUM];
		GUIStaticText* m_tab_txt[MAX_RUNE_TAB_NUM];
		GUIRadioButton* m_reclaim_tab_radio_btn[MAX_RUNE_RECALIM_TAB_NUM];
		GUIStaticText* m_reclaim_tab_txt[MAX_RUNE_RECALIM_TAB_NUM];
		GUILayout* m_rune_assemble_layout = nullptr;
		GUILayout* m_rune_reclaim_layout = nullptr;
		GUILayout* m_reclaim_grid_layout = nullptr;
		GUIGridView* m_reclaim_grid_view = nullptr;
		GUIButton* m_reclaim_batch_btn = nullptr;
		GUIStaticImage* m_rune_list_layout = nullptr;
		GUIStaticImage* m_pro_sum_layout = nullptr;
		GUIStaticText* m_rune_trough_pro_sum_level_txt = nullptr;
		GUILayout* m_effect_layout = nullptr;
		GUIStaticImage* m_effect_bg = nullptr;
		const int TIME_TICK_ALL = 200;
		int m_left_time_tick = 0;
		float m_left_start_pos = 0.f;
		float m_rune_trough_list_end_pos = 0.f;
		int m_right_time_tick = 0;
		float m_right_start_pos = 0.f;
		int m_current_tab_index = RT_ASSEMBLE;
		int m_current_reclaim_tab_index = RT_INVALID;
		int m_player_level = 0;
		int m_unlock_price = 0;
		int m_current_select_rune_trough_id = 0;
		int m_current_select_rune_trough_rune_id = 0;
		int m_rune_trough_select_index = -1;
		int m_rune_trough_select_type = 0;
		int m_rune_trough_sum_level = 0;
		bool m_unlock_effect = false;
		std::vector<int> m_unlock_id_list;
		int m_first_unlock_type = RT_ATTACK;
		int m_refreshRuneTypeListTime = 0;
		int m_effect_time = 0;
		int m_effect_start_time = 0;
		int m_assemble_open_type = AOT_DEFAULT;
		bool m_moving_trough = false;

		std::map<int, std::vector<RuneTroughItem>> m_rune_trough_map;
		std::vector<RuneItem> m_rune_list;
	};
}

#endif

