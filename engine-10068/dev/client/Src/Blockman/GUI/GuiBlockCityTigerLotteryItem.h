#pragma once
#ifndef __GUI_BLOCKCITY_TIGER_LOTTERY_ITEM_HEADER__
#define __GUI_BLOCKCITY_TIGER_LOTTERY_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "Game/BlockCity.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBlockCityTigerLotteryItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticImage* m_lock = nullptr;
		GUIStaticText* m_text = nullptr;
		GUIStaticImage* m_new = nullptr;
		GUIStaticImage* m_hot = nullptr;
		GUIStaticImage* m_clock = nullptr;

		i32 m_showType = 1;
		BlockCityStoreItem m_storeItem;
		bool m_isSelect = false;
		BlockCityHouseShopItem m_packItem;

	private:
		void removeComponents();

	public:
		GuiBlockCityTigerLotteryItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockCityTigerLotteryItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setLotteryItem(const int& itemId);
		void setBlockLotteryItem(const int& itemId, const int& itemIdMate);
		void setStoreItem(const BlockCityStoreItem& item);
		void setStoreMaterialItem(const BlockCityMaterial& item);

		void setShowType(i32 type) { m_showType = type; }
		bool onClick(const EventArgs& args);
		void setSelectId(i32 id);
		bool isSelected() { return m_isSelect; }

		void setLackItem(const BlockCityLackItem& item);
		void setPackItem(const BlockCityHouseShopItem& item);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
