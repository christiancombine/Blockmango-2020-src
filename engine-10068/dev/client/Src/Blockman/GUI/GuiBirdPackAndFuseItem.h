#pragma once
#ifndef __GUI_BIRD_PACK_AND_FUSE_ITEM_HEADER__
#define __GUI_BIRD_PACK_AND_FUSE_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIRadioButton.h"
#include "Game/BirdSimulator.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBirdPackAndFuseItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticImage* m_image = nullptr;
		GUIStaticImage* m_greenDot = nullptr;
		GUIStaticText* m_level = nullptr;
		GUIStaticImage* m_tick = nullptr;
		i32 m_showType = 1;
		bool m_isSelect = false;

		BirdInfo m_bird;
		BirdStoreItem m_tool;
		SubscriptionGuard m_subscriptionGuard;

	private:
		void removeComponents();

	public:
		GuiBirdPackAndFuseItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdPackAndFuseItem();

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setBirdInfo(BirdInfo data);
		void setSelect(i64 id);
		void unSelect(i64 birdId);
		void setSelect(bool isSelect) { m_isSelect = isSelect; }
		void setShowType(i32 type) { m_showType = type; }
		bool onClick(const EventArgs& args);
		void setToolInfo(BirdStoreItem data);

	private:
		void changeImage();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
