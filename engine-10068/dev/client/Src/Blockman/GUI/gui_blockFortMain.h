#ifndef __GUI_BLOCKFORT_MAIN_HEADER__
#define __GUI_BLOCKFORT_MAIN_HEADER__

#include "GUI/gui_layout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUICheckBox.h"
#include "GUI/GuiUrlImage.h"

namespace BLOCKMAN
{
	class gui_blockFortMain : public gui_layout
	{
	private:
		enum class ViewId {
			MANOR_UPGRADE,
			STORE,
			FRIEND,
			ATTACK,
		};

	public:
		gui_blockFortMain();
		~gui_blockFortMain();

		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		bool onDataChange();

	private:
		GUILayout* m_transfer = nullptr;
		GuiUrlImage* m_head = nullptr;
		GUIStaticText* m_level = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIProgressBar* m_exp = nullptr;
		GUIStaticText* m_expValue = nullptr;
		GUICheckBox* m_transferCb = nullptr;

		i32 m_transferBtnsLoadTime = 0;
		i32 m_transferBtnsTime = 0;
		bool m_isShowTransferBtns = false;
		bool m_isOpenTransferTip = false;
		bool m_dataChange = false;

	private:
		void onSetPlayerInfo();
		bool onTransferBtnsCheckChange(const EventArgs &args);
		bool onClickTransfer(const EventArgs &args, i32 place);
		bool onClickBtn(const EventArgs &args, ViewId viewId);
		bool updateIsShowTransferTip(bool isHave);
		bool sureTransfer();
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
