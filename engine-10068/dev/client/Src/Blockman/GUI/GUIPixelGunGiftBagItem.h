#ifndef __GUI_PIXEL_GUN_GIFT_BAG_ITEM_HEADER__
#define __GUI_PIXEL_GUN_GIFT_BAG_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/PixelGunGiftBagSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIPixelGunGiftBagItem : public CustomGuiWindow
	{

	private:
		int m_gift_id = 0;
		GUIWindow* m_window = nullptr;
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIPixelGunGiftBagItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIPixelGunGiftBagItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void InitGift(GiftBag* gift);
		void InitGiftItem(GiftBag* gift);
		bool OnBuyButtonClick(const EventArgs & events);
		bool OnQuestionButtonClick(const EventArgs & events);

	public:
		void Destroy() override;
	};
}
#endif 
