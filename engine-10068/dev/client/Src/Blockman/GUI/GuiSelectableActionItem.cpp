#include "GuiSelectableActionItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cEntity/EntityPlayerSP.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "game.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiItemStack.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "GUI/RootGuiLayout.h"
#include "Object/Root.h"
#include "ShellInterface.h"
#include "Util/ClientDataReport.h"
#include "Plugins/GameClientEvents.h"

using namespace LORD;

namespace BLOCKMAN
{
	GuiSelectableActionItem::GuiSelectableActionItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SelectableActionItem.json"));
		renameComponents(m_window);
		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_mask = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_window->subscribeEvent(EventWindowTouchMove, std::bind(&GuiSelectableActionItem::onItemTouchMove, this, std::placeholders::_1));
		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiSelectableActionItem::onItemTouchUp, this, std::placeholders::_1));
		m_window->subscribeEvent(EventMotionRelease, std::bind(&GuiSelectableActionItem::onItemTouchRelease, this, std::placeholders::_1));
		AddChildWindow(m_window);
	}

	void GuiSelectableActionItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiSelectableActionItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	bool GuiSelectableActionItem::onItemTouchMove(const EventArgs & events)
	{
		m_isMoved = true;
		return true;
	}

	bool GuiSelectableActionItem::onItemTouchUp(const EventArgs & events)
	{
		if (!m_isMoved)
		{
			EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
			if (player && !player->isOnVehicle() && !player->isOnAircraft() && m_actionId > 0)
			{
				if (m_isLocked)
				{
					RootGuiLayout::Instance()->showUnlockSelectableActionTip(m_actionId);
				}
				else
				{
					SCRIPT_EVENT::ClickSelectableActionItemEvent::invoke(true);
					int currTime = Root::Instance()->getCurrentTime();
					if (currTime - player->m_lastPlaySelectableActionTime > 3000)
					{
						player->m_lastPlaySelectableActionTime = currTime;

						player->m_playActionName = StringUtil::Format("selectable_action_%d", m_actionId);
						player->m_needUpdateUpperAction = true;
						player->setInsertUpperAction(ActionState::AS_SELECTABLE_ACTION, -1);

						ClientNetwork::Instance()->getSender()->sendPlayingSelectableAction(m_actionId, StringUtil::Format("gui_selectable_action_%d", m_actionId));
						String msg = StringUtil::Format(getString(StringUtil::Format("gui_selectable_action_%d_for_me", m_actionId)).c_str());
						ChatMessageReceiveEvent::emit(msg,false);

						Blockman::Instance()->setPersonView(1);
						RootGuiLayout::Instance()->updateSwitchSerpece();
						GameClient::CGame::Instance()->dataReportNotType(BLOCKMAN::ClientDataReport::SEND_ACT_SUCCESS, StringUtil::ToString(m_actionId));
					}
					else
					{
						String msg = StringUtil::Format(getString(LanguageKey::GUI_SELECTABLE_ACTION_OPERATE_FREQUENTLY).c_str());
						ChatMessageReceiveEvent::emit(msg,false);
					}
				}
			}
			RootGuiLayout::Instance()->hideSelectableAction();
		}
		m_isMoved = false;
		return true;
	}

	bool GuiSelectableActionItem::onItemTouchRelease(const EventArgs & events)
	{
		m_isMoved = false;
		return true;
	}

	void GuiSelectableActionItem::setAction(int actionId, bool isLocked)
	{
		m_actionId = actionId;
		m_isLocked = isLocked;
		updateIcon();
	}

	vector<GUIWindow*>::type GuiSelectableActionItem::getPrivateChildren()
	{
		return { m_window };
	}

	void GuiSelectableActionItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GuiSelectableActionItem::updateIcon()
	{
		m_mask->SetVisible(m_isLocked);
		m_icon->SetImage(StringUtil::Format("set:selectable_action_img.json image:action_img_%d", m_actionId).c_str());
	}
}