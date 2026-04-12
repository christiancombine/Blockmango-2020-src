#pragma once
#ifndef __GUI_BLOCKCITY_RECEIVE_FLOWER_ITEM_HEADER__
#define __GUI_BLOCKCITY_RECEIVE_FLOWER_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/SubscriptionGuard.h"
#include "GUI/GuiUrlImage.h"
#include "Util/ThumbUpRecordManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBlockCityReceiveFlowerItem : public CustomGuiWindow
	{
	private:
		enum class ViewId {
			TOSS_BACK,
			VISIT,
			DEL,
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GuiUrlImage	*m_pic = nullptr;
		GUIStaticText* m_name = nullptr;
		GUIButton* m_btnTossBack = nullptr;
		GUIButton* m_btnVisit = nullptr;
		GUIButton* m_btnDelete = nullptr;

		SubscriptionGuard m_subscriptionGuard;
		ThumbUpRecord m_record;

	private:
		void removeComponents();

	public:
		GuiBlockCityReceiveFlowerItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBlockCityReceiveFlowerItem() { m_subscriptionGuard.unsubscribeAll(); }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

 		void setItem(const ThumbUpRecord& item);
		bool onClick(const EventArgs& args, ViewId viewId);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
