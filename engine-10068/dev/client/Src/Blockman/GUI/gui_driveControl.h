#pragma once

#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIStaticImage.h"
#include <bitset>

namespace BLOCKMAN
{
	class gui_driveControl : public  gui_layout
	{
	private:
		enum ButtonId
		{
			kButtonForward = 0,
			kButtonBack,
			kButtonLeft,
			kButtonRight,
			kButtonHorn,
			kButtonBrake,
			kButtonNos,
			kButtonDebark,
			kButtonCount
		};

		struct ButtonSetting
		{
			ButtonId id;
			LORD::GUIString normalImage;
			LORD::GUIString pressedImage;
			LORD::GUIString windowName;
		};

		using ButtonSettingTable = std::array<ButtonSetting, kButtonCount>;
		static const ButtonSettingTable& GetButtonSettingTable();

	private:
		SubscriptionGuard m_subscriptionGuard;
		std::array<LORD::GUIStaticImage*, kButtonCount> m_buttons = {};
		std::bitset<kButtonCount> m_buttonPressed = {};
		GUIStaticImage* m_lockSwitch = nullptr;
		GUIWindow* m_leftSection = nullptr;
		GUIWindow* m_rightSection = nullptr;
		Vector2 m_leftSectionOriginalSize = Vector2::ZERO;
		Vector2 m_rightSectionOriginalSize = Vector2::ZERO;

	public:
		gui_driveControl()
			: gui_layout("DriveControl.json") {}
		void onLoad() override;
		void onShow() override;
		void onUnload() override;
		void onUpdate(ui32 nTimeElapse) override;

		bool onButtonPressed(const EventArgs& events, ButtonId id);
		bool onButtonReleased(const EventArgs& events, ButtonId id);
		bool onLockToggle(const EventArgs& events);
		bool onLockStateChanged(bool locked);
		bool onSetLockBtns(bool locked);
	};

}