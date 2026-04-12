#pragma once
#ifndef __GUI_BLOCK_FORT_STORE_ITEM_HEADER__
#define __GUI_BLOCK_FORT_STORE_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/SubscriptionGuard.h"
#include "Game/BlockFort.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBlockFortStoreItem : public CustomGuiWindow
	{

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_icon = nullptr;
		GUIStaticImage* m_ivLock = nullptr;
		GUIButton* m_ivLimit = nullptr;
		BlockFortStoreItem m_item;
		i32 m_selectId = -1;

	private:
		void removeComponents();

	public:
		GuiBlockFortStoreItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockFortStoreItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setItem(const BlockFortStoreItem& item);
		void setSelectId(i32 id);
		bool isSelected() { return m_item.id == m_selectId; }

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
