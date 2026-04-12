#include "gui_blockCityElevator.h"
#include "GUI/GuiBlockCityElevatorItem.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/RootGuiLayout.h"
#include "Game/BlockCity.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "Setting/GuideSystemSetting.h"
#include "GuiDef.h"
#include "UI/GUILayout.h"

namespace BLOCKMAN
{
	gui_blockCityElevator::gui_blockCityElevator()
		: gui_layout("BlockCityElevator.json")
	{
	}

	gui_blockCityElevator::~gui_blockCityElevator()
	{
	}

	void gui_blockCityElevator::onLoad()
	{
		setText("BlockCityElevator-Title-Text", "gui_blockcity_elevator");
		setText("BlockCityElevator-Choose-Floor-Text", "gui_blockcity_elevator_choose_floor");
		setText("BlockCityElevator-Content-Cancel", "gui_blockcity_elevator_close");

		m_firstGroup = getWindow<GUILayout>("BlockCityElevator-Content-First-Group");
		m_secondGroup = getWindow<GUILayout>("BlockCityElevator-Content-Second-Group");

		getWindow("BlockCityElevator-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityElevator::onClose, this, std::placeholders::_1));
		getWindow("BlockCityElevator-Content-Cancel")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityElevator::onClose, this, std::placeholders::_1));
		m_subscriptionGuard.add(ShowBlockCityElevatorEvent::subscribe(std::bind(&gui_blockCityElevator::openElevator, this, std::placeholders::_1)));
		m_subscriptionGuard.add(setRaceVehileStatusEvent::subscribe(std::bind(&gui_blockCityElevator::isInviteRaceVehile, this, std::placeholders::_1)));
	}

	void gui_blockCityElevator::onShow()
	{
	
	}

	void gui_blockCityElevator::onUpdate(ui32 nTimeElapse)
	{
		if (!UICommon::checkBlockCityParam())
			return;

		Vector3 position = Blockman::Instance()->m_pPlayer->position;
		i32 floorId = Blockman::Instance()->m_pPlayer->m_blockCity->getElevatorFloorByPos(position);
		if (floorId != 0 && !m_isShowUI && !m_inviteRaceVehicle)
		{
			ShowBlockCityElevatorEvent::emit(floorId);
			m_isShowUI = true;
		}

		if (floorId == 0)
		{
			RootGuiLayout::Instance()->hideBlockCityElevator();
			m_isShowUI = false;
		}	
	}

	bool gui_blockCityElevator::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCityElevator::openElevator(i32 floorId)
	{
		if (!UICommon::checkBlockCityParam())
		{
			return false;
		}
		RootGuiLayout::Instance()->showBlockCityElevator();
		auto elevatorIcons = Blockman::Instance()->m_pPlayer->m_blockCity->getElevatorFloorIcons();
		static int count = 0;
		float itemY = (m_firstGroup->GetPixelSize().y - 5 * 3.f) / 6;

		if (m_firstGroup->GetChildCount() + m_secondGroup->GetChildCount() != 12)
		{
			for (size_t i = 0; i < 12; i++)
			{
				GUIString strItemName = StringUtil::Format("BlockCityElevator-Content-Item-%d", count++).c_str();
				GuiBlockCityElevatorItem* itemView = (GuiBlockCityElevatorItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_ELEVATOR_ITEM, strItemName.c_str());
				if (i < 6)
				{
					itemView->SetArea(UDim(0, 0), UDim(0, i * (itemY + 3.f)), UDim(1.0f, 0.f), UDim(0.0f, itemY));
					if (elevatorIcons.size() >= (i + 1))
					{
						auto iconMap = elevatorIcons.find(i + 1);
						if (iconMap != elevatorIcons.end())
						{
							itemView->setItem(iconMap->second, i + 1, floorId);
						}
					}
					else
					{
						itemView->setItem("", i + 1, floorId);
					}
					m_firstGroup->AddChildWindow(itemView);
				}
				else
				{
					itemView->SetArea(UDim(0, 0), UDim(0, (i - 6) * (itemY + 3.f)), UDim(1.0f, 0.f), UDim(0.0f, itemY));
					if (elevatorIcons.size() >= (i + 1))
					{
						auto iconMap = elevatorIcons.find(i + 1);
						if (iconMap != elevatorIcons.end())
						{
							itemView->setItem(iconMap->second, i + 1, floorId);
						}
					}
					else
					{
						itemView->setItem("", i + 1, floorId);
					}
					m_secondGroup->AddChildWindow(itemView);
				}
			}
		}
		else
		{
			for (size_t i = 0; i < m_firstGroup->GetChildCount(); i++)
 			{
 				auto itemView = dynamic_cast<GuiBlockCityElevatorItem*>(m_firstGroup->GetChildByIndex(i));
				if (elevatorIcons.size() >= (i + 1))
				{
					auto iconMap = elevatorIcons.find(i + 1);
					if (iconMap != elevatorIcons.end())
					{
						itemView->setItem(iconMap->second, i + 1, floorId);
					}
				}
				else
				{
					itemView->setItem("", i + 1, floorId);
				}
 			}
			for (size_t i = 0; i < m_secondGroup->GetChildCount(); i++)
			{
				auto itemView = dynamic_cast<GuiBlockCityElevatorItem*>(m_secondGroup->GetChildByIndex(i));
				if (elevatorIcons.size() >= (m_firstGroup->GetChildCount() + i + 1))
				{
					auto iconMap = elevatorIcons.find(m_firstGroup->GetChildCount() + i);
					if (iconMap != elevatorIcons.end())
					{
						itemView->setItem(iconMap->second, m_firstGroup->GetChildCount() + i + 1, floorId);
					}
				}
				else
				{
					itemView->setItem("", m_firstGroup->GetChildCount() + i + 1, floorId);
				}
			}
		}
		return true;
	}

	bool gui_blockCityElevator::onClose(const EventArgs& events)
	{
		RootGuiLayout::Instance()->showMainControl();
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_blockCityElevator::isInviteRaceVehile(bool invite)
	{
		m_inviteRaceVehicle = invite;
		return true;
	}
}