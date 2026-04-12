#pragma once
#ifndef __GUI_BLOCKCITY_CHOOSE_PAPER_ITEM_HEADER__
#define __GUI_BLOCKCITY_CHOOSE_PAPER_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Game/BlockCity.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBlockCityChoosePaperItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_volume = nullptr;
		GUIStaticText* m_score = nullptr;
		GUIStaticText* m_volumeText = nullptr;
		GUIStaticText* m_scoreText = nullptr;
		GUIStaticImage* m_frameUp = nullptr;
		GUIStaticImage* m_frameDown = nullptr;
		GUILayout* m_chooseFrame = nullptr;
		BlockCityStoreItem  m_item;
		SubscriptionGuard m_subscriptionGuard;

	private:
		void removeComponents();

	public:
		GuiBlockCityChoosePaperItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockCityChoosePaperItem() { m_subscriptionGuard.unsubscribeAll(); }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		bool onClick(const EventArgs& args);
		void setItem(const BlockCityStoreItem& item);
		bool onItemClick(i32 itemId);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
