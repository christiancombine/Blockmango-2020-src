#ifndef __WALKINGDEAD_STORE_ITEM_HEADER__
#define __WALKINGDEAD_STORE_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/WalkingDeadStoreSetting.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIWalkingDeadStoreItem : public CustomGuiWindow
	{
	private:
		int m_goods_id = 0;
		WalkingDeadStoreGoods* m_goods = nullptr;
		GUIWindow* m_window = nullptr;

		GUILayout* m_itemcontent=nullptr;
		GUILayout* m_prop=nullptr;
		GUIStaticImage* m_itemIcon ;
		GUIStaticText* m_itemName ;
		GUIStaticImage* m_itemImagebg ;
		GUIStaticText* m_itemNum;
		GUIButton* m_btnBuy;

		GUIStaticImage* m_itemMoneyImage;
		GUIStaticText* m_Price_text;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIWalkingDeadStoreItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIWalkingDeadStoreItem() = default;

		void Destroy() override;
		void UpdateSelf(ui32 nTimeElapse) override;

		void InitStoreItems(WalkingDeadStoreGoods* goods);
		int GetTabId() { return m_goods_id; }


		bool onItemClick(const EventArgs & events);

	private:
		void findViews();
		bool onButtonClick(const EventArgs & events);

	};
}

#endif