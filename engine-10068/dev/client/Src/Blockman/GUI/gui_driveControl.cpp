#include "gui_driveControl.h"
#include "Blockman/cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Entity/EntityVehicle.h"
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "game.h"
#include "Util/ClientEvents.h"

using namespace LORD;

namespace BLOCKMAN
{
	auto gui_driveControl::GetButtonSettingTable() -> const ButtonSettingTable &
	{
		auto createSettingTable = []() -> ButtonSettingTable {
			ButtonSettingTable table;
			table[kButtonForward]	= { kButtonForward,	"forward",	"forward_pressed",	"DriveControl-Forward" };
			table[kButtonBack]		= { kButtonBack,	"back",		"back_pressed",		"DriveControl-Back" };
			table[kButtonLeft]		= { kButtonLeft,	"left",		"left_pressed",		"DriveControl-Left" };
			table[kButtonRight]		= { kButtonRight,	"right",	"right_pressed",	"DriveControl-Right" };
			table[kButtonHorn]		= { kButtonHorn,	"horn",		"horn_pressed",		"DriveControl-Horn" };
			table[kButtonBrake]		= { kButtonBrake,	"brake",	"brake_pressed",	"DriveControl-Brake" };
			table[kButtonNos]		= { kButtonNos,		"nos",		"nos_pressed",		"DriveControl-NOS" };
			table[kButtonDebark]	= { kButtonDebark,	"debark",	"debark_pressed",	"DriveControl-Debark" };

			for (auto&& setting : table)
			{
				static constexpr const char* prefix = "set:drive.json image:";
				setting.normalImage = prefix + setting.normalImage;
				setting.pressedImage = prefix + setting.pressedImage;
			}

			return table;
		};
		static ButtonSettingTable settingTable = createSettingTable();
		return settingTable;
	}

	void gui_driveControl::onLoad()
	{
		using namespace std::placeholders;
		for (const auto& setting : GetButtonSettingTable())
		{
			auto window = getWindow<GUIStaticImage>(setting.windowName);
			m_buttons[setting.id] = window;
			(void)window->subscribeEvent(EventWindowTouchDown, std::bind(&gui_driveControl::onButtonPressed, this, _1, setting.id));
			(void)window->subscribeEvent(EventWindowTouchUp, std::bind(&gui_driveControl::onButtonReleased, this, _1, setting.id));
			(void)window->subscribeEvent(EventMotionRelease, std::bind(&gui_driveControl::onButtonReleased, this, _1, setting.id));
		}
		m_lockSwitch = getWindow<GUIStaticImage>("DriveControl-Lock");
		(void)m_lockSwitch->subscribeEvent(EventWindowTouchDown, std::bind(&gui_driveControl::onLockToggle, this, _1));
		m_subscriptionGuard.add(VehicleLockEvent::subscribe(std::bind(&gui_driveControl::onLockStateChanged, this, _1)));
		m_subscriptionGuard.add(setRaceVehileCountdownEvent::subscribe(std::bind(&gui_driveControl::onSetLockBtns, this, _1)));

		m_leftSection = getWindow("DriveControl-LeftSection");
		m_rightSection = getWindow("DriveControl-RightSection");
		m_leftSectionOriginalSize = m_leftSection->GetPixelSize();
		m_rightSectionOriginalSize = m_rightSection->GetPixelSize();
	}

	void gui_driveControl::onShow()
	{
		onLockStateChanged(false);
	}

	void gui_driveControl::onUnload()
	{
		m_buttons.fill(nullptr);
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_driveControl::onUpdate(ui32 nTimeElapse)
	{
		for (const auto& setting : GetButtonSettingTable())
		{
			if (m_buttonPressed[setting.id])
			{
				m_buttons[setting.id]->SetImage(setting.pressedImage);
			}
			else
			{
				m_buttons[setting.id]->SetImage(setting.normalImage);
			}
		}
		float scale = Blockman::Instance()->m_gameSettings->playerActivityGuiSize;
		m_leftSection->SetSize({ {0, m_leftSectionOriginalSize.x * scale}, {0, m_leftSectionOriginalSize.y * scale} });
		m_rightSection->SetSize({ {0, m_rightSectionOriginalSize.x * scale}, {0, m_rightSectionOriginalSize.y * scale} });
	}

	bool gui_driveControl::onButtonPressed(const EventArgs& events, ButtonId id)
	{
		m_buttonPressed[id] = true;
		auto player = Blockman::Instance()->m_pPlayer;
		if (!player)
			return true;
		auto vehicle = player->getVehicle();
		if (!vehicle)
			return true;

		switch (id)
		{
		case kButtonForward:
			vehicle->setThrottle(1);
			break;
		case kButtonBack:
			vehicle->setThrottle(-1);
			break;
		case kButtonLeft:
			vehicle->setTurning(1);
			break;
		case kButtonRight:
			vehicle->setTurning(-1);
			break;
		case kButtonHorn:
			vehicle->playHornSound();
			break;
		case kButtonBrake:
			vehicle->setBraking(true);
			break;
		case kButtonNos:
			break;
		case kButtonDebark:
			Blockman::Instance()->m_pPlayer->askGetOffVehicle();
			break;
		default:
			break;
		}
		return true;
	}

	bool gui_driveControl::onButtonReleased(const EventArgs& events, ButtonId id)
	{
		m_buttonPressed[id] = false;
		auto player = Blockman::Instance()->m_pPlayer;
		if (!player)
			return true;
		auto vehicle = player->getVehicle();
		if (!vehicle)
			return true;

		switch (id)
		{
		case kButtonForward:
		case kButtonBack:
			vehicle->setThrottle(0);
			break;
		case kButtonLeft:
		case kButtonRight:
			vehicle->setTurning(0);
			break;
		case kButtonHorn:
			break;
		case kButtonBrake:
			vehicle->setBraking(false);
			break;
		case kButtonNos:
			break;
		case kButtonDebark:
			break;
		default:
			break;
		}
		return true;
	}

	bool gui_driveControl::onLockToggle(const EventArgs& events)
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (!pPlayer || !pPlayer->isDriver())
			return true;
		auto vehicle = pPlayer->getVehicle();
		if (!vehicle)
			return true;
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendLockVehicle(!vehicle->isLocked(), vehicle->entityId);
		return true;
	}

	bool gui_driveControl::onLockStateChanged(bool locked)
	{
		static const GUIString lockedImage = "set:drive.json image:locked";
		static const GUIString unlockedImage = "set:drive.json image:unlocked";
		m_lockSwitch->SetImage(locked ? lockedImage : unlockedImage);
		return true;
	}

	bool gui_driveControl::onSetLockBtns(bool locked)
	{
		m_buttons[kButtonLeft]->SetEnabled(!locked);
		m_buttons[kButtonRight]->SetEnabled(!locked);
		m_buttons[kButtonForward]->SetEnabled(!locked);
		m_buttons[kButtonBack]->SetEnabled(!locked);
		m_buttons[kButtonDebark]->SetEnabled(!locked);
		return true;
	}
}