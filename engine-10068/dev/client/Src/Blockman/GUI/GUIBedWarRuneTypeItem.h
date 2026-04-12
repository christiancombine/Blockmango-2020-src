#pragma once
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBedWarRuneTypeItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUIWindow* m_window = nullptr;
		void removeComponents();

	public:
		struct RuneTroughItem
		{
			int rune_trough_id = 0;
			int rune_id = 0;
			bool rune_lock = false;
			bool need_show_effect = false;
		};

		static const int MAX_RUNE_TROUGH_NUM = 7;
		static const int MAX_RUNE_TYPE = 4;


	public:
		GuiBedWarRuneTypeItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBedWarRuneTypeItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void RefreshItem(std::vector<RuneTroughItem>& rune_trough_vec, int trough_type, int player_lv, int unlock_price, std::vector<int>& unlock_list);
		bool onRuneClick(const EventArgs & events, int index);
		bool onLeftClick(const EventArgs & events);
		bool onRightClick(const EventArgs & events);
 		bool onOneKeyUnloadClick(const EventArgs & events);
 		bool onOneKeyAssembleClick(const EventArgs & events);
 		bool onChangeBtnClick(const EventArgs & events);
		bool onBackBtnClick(const EventArgs & events);
		void refreshSelect(int index);
		void refreshSelectFromOutSide(int trough_id);
		void clearSelect();
		void setMoving(bool sign);
		int getTroughType() { return m_trough_type; }

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
		std::vector<RuneTroughItem> m_rune_trough_vec;
		int m_current_select_index = -1;
		int m_trough_type = -1;
		int m_player_lv = 0;
		int m_unlock_price = 0;
		bool m_moving = false;
		
		GUIStaticImage* m_trough_bg = nullptr;
		GUIButton* m_left_btn = nullptr;
		GUIButton* m_right_btn = nullptr;
		GUIButton* m_one_key_unload = nullptr;
		GUIButton* m_one_key_assemble = nullptr;
		GUIButton* m_change_btn = nullptr;
		GUIButton* m_back_btn = nullptr;
		GUIStaticImage* m_type_tab_img = nullptr;
		GUIStaticText* m_type_tab_txt = nullptr;
		GUIStaticImage* m_trough_kuang[MAX_RUNE_TROUGH_NUM];
		GUIStaticImage* m_trough_select[MAX_RUNE_TROUGH_NUM];
		GUIStaticImage* m_trough_lock_img[MAX_RUNE_TROUGH_NUM];
		GUIStaticText* m_trough_lock_txt[MAX_RUNE_TROUGH_NUM];
		GUIStaticImage* m_trough_img[MAX_RUNE_TROUGH_NUM];
		GUIStaticImage* m_trough_big_lock[MAX_RUNE_TROUGH_NUM];
		GUIStaticImage* m_rune_type_circle[MAX_RUNE_TYPE];
		GUILayout* m_trough_effect_layout[MAX_RUNE_TROUGH_NUM];
	};

}

