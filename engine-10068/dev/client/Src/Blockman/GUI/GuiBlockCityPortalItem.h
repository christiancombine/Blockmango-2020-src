#pragma once
#ifndef __GUI_BLOCKCITY_PORTAL_ITEM_HEADER__
#define __GUI_BLOCKCITY_PORTAL_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "Setting/BlockCityPortalSetting.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBlockCityPortalItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		SubscriptionGuard m_subscriptionGuard;

	private:
		void removeComponents();

	public:
		GuiBlockCityPortalItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockCityPortalItem() { m_subscriptionGuard.unsubscribeAll(); }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void onSetItem(BlockCityPortalItem &item);
		void setSelect(i32 index);
		bool onClick(const EventArgs& args);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	private:
		GUIStaticText* m_name = nullptr;

		BlockCityPortalItem m_item;
	};

}
#endif
