#pragma once
#ifndef __GUI_BIRD_TOOL_HEADER__
#define __GUI_BIRD_TOOL_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "Util/SubscriptionGuard.h"
#include "Game/BirdSimulator.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiBirdTool : public CustomGuiWindow
	{
	private:
		enum class ViewId {
			PACK,
			GATHER,
			WING,
			CAP,
			CLOTHES,
			USE
		};

	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;

		GUIStaticText* m_name = nullptr;
		GUIStaticImage* m_image = nullptr;
		GUIStaticText* m_des = nullptr;
		GUIButton* m_use = nullptr;
		GUIStaticImage* m_collect = nullptr;
		GUIStaticImage* m_pack = nullptr;
		GUIStaticImage* m_wing = nullptr;

		SubscriptionGuard m_subscriptionGuard;

	private:
		void removeComponents();

	private:
		BirdStoreItem m_tool;
		BirdStoreItem m_curCollect;
		BirdStoreItem m_curPack;
		BirdStoreItem m_curWing;

	public:
		GuiBirdTool(const GUIWindowType& nType, const GUIString& strName);
		~GuiBirdTool() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		bool onClick(const EventArgs& args, ViewId viewId);

		void setToolInfo(BirdStoreItem tool);
		void setCurUse(BirdStoreItem tool);

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
