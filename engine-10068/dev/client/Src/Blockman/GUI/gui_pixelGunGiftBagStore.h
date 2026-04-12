#pragma once
#ifndef __GUI_PIXEL_GUN_GIFT_BAG_STORE_HEADER__
#define __GUI_PIXEL_GUN_GIFT_BAG_STORE_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/PixelGunGiftBagSetting.h"
#include "GUIPixelGunGiftBagItem.h"
#include "UI/GUIGridView.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_pixelGunGiftBagStore : public gui_layout
	{
	private:
		GUIGridView* m_list = nullptr;
		int m_last_time = 0;
		int m_giftAdTimes = -1;
		int m_giftAdTime = -1;
		list<GUIPixelGunGiftBagItem*>::type m_gifts;
	public:
		gui_pixelGunGiftBagStore();
		virtual ~gui_pixelGunGiftBagStore();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isForceUpdate() { return true; }
		bool isNeedLoad() override;
		void onGetCommonData(String key, String data);

	private:
		bool onCloseClick(const EventArgs & events);
		bool onRefreshClick(const EventArgs & events);
		void initGiftBags();
		void showGiftBagStore(list<GiftBag*>::type gifts);
		void updateGifts(list<GiftBag*>::type gifts);
		void createGiftBagItem(GiftBag* gift);
		bool onUpdateGiftBagData(const String & data, bool isRefresh, bool isShow);
		bool onUpdateGiftRefreshTime(int time);
		void setRefreshTime(int time);
		bool onSyncGiftAdInfo(int times, int seconds);

		GUIPixelGunGiftBagItem* getGiftByIndex(int index);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}
#endif 
