#ifndef __WALKINGDEAD_STORE_SUPPLY_ITEM_HEADER__
#define __WALKINGDEAD_STORE_SUPPLY_ITEM_HEADER__

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
	class GUIWalkingDeadStoreSupplyItem :public CustomGuiWindow
	{
	private:
		WalkingDeadSupply* m_data = NULL;
		GUIWindow* m_window = nullptr;
		GUILayout* m_itemContent = nullptr;

		GUIStaticText* m_itemName = nullptr;
		GUIStaticImage* m_itemImage = nullptr;
		GUIButton* m_itemQuestion = nullptr;
		GUIButton* m_itemBuy = nullptr;
		GUIButton* m_itemReceive = nullptr;
		GUIButton* m_itemHasReceive = nullptr;

		GUIStaticText* m_itemBuy_text = nullptr;
		GUIStaticText* m_itemReceive_text = nullptr;
		GUIStaticText* m_itemHasReceive_text = nullptr;

		GUIStaticText* m_itemPrice = nullptr;
		GUIStaticText* m_itemDesc = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void initView();

	public:
		GUIWalkingDeadStoreSupplyItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIWalkingDeadStoreSupplyItem() = default;

		void Destroy() override;
		void UpdateSelf(ui32 nTimeElapse) override;
		void InitSupplyItems(WalkingDeadSupply*	chest);

	private:
		
		bool OnBuyButtonClick(const EventArgs & events);
		bool OnQuestionButtonClick(const EventArgs & events);
		bool OnReceiveButtonClick(const EventArgs & events);
	};
}


#endif