#pragma once
#ifndef __GUI_BLOCKCITY_SAVE_FILE_ITEM_HEADER__
#define __GUI_BLOCKCITY_SAVE_FILE_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIListBox.h"
#include "UI/GUIButton.h"
#include "UI/GUIEditBox.h"
#include "Game/BlockCity.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBlockCitySaveFileItem : public CustomGuiWindow
	{
	private:
		enum class ViewId {
			MAIN,
			CHANGE_NAME,
			ADD,
			SURE_CHANGE_NAME,
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticText* m_num = nullptr;
		GUILayout* m_info = nullptr;
		GUILayout* m_record = nullptr;
		GUIStaticText* m_recordName = nullptr;
		GUIStaticText* m_recordDate = nullptr;
		GUIStaticText* m_nullRecord = nullptr;
		GUILayout* m_button = nullptr;
		GUIButton* m_mod_name = nullptr;
		GUIButton* m_add = nullptr;
		GUILayout* m_edit_name = nullptr;
		GUIEditBox* m_edit = nullptr;
		GUIButton* m_sure_change_name = nullptr;

		SubscriptionGuard m_subscriptionGuard;

	private:
		void removeComponents();

	public:
		GuiBlockCitySaveFileItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockCitySaveFileItem() { m_subscriptionGuard.unsubscribeAll(); }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

 		void onSetItem(BlockCityArchiveItem item, i32 nextPrice, i32 maxNum, i32 index);
		void setSelect(i32 index);
		void setCurNum(i32 num);
		bool onClick(const EventArgs& args, ViewId viewId);
		bool onInputChanged(const EventArgs & events);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	private:
		i32 m_index = -1;
		i32 m_nextPrice = 0;
		i32 m_maxNum = 0;
		i32 m_curNum = 0;
		bool m_isSelect = false;
		bool m_isShowEdit = false;
	};

}
#endif
