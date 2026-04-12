#include "gui_blockFortBuildingInfo.h"
#include "RootGuiLayout.h"
#include "GuiBlockFortItemInfo.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	gui_blockFortBuildingInfo::gui_blockFortBuildingInfo() :
		gui_layout("BlockFortBuilding.json")
	{
	}

	gui_blockFortBuildingInfo::~gui_blockFortBuildingInfo()
	{
	}

	void gui_blockFortBuildingInfo::onLoad()
	{
		m_descTurretAndWallList = getWindow<GUIListBox>("BlockFortBuilding-Desc-Turret_Wall-List");
		m_descFortLayout = getWindow<GUILayout>("BlockFortBuilding-Desc-Fort");

		m_mainIcon = getWindow<GUIStaticImage>("BlockFortBuilding-Icon");
		m_mainTitleText = getWindow<GUIStaticText>("BlockFortBuilding-Title-String");

		m_upgradeBtn = getWindow<GUIButton>("BlockFortBuilding-Btn-Upgrade");
		m_upgradeBtnIcon = getWindow<GUIStaticImage>("BlockFortBuilding-Btn-Upgrade-Icon");

		m_subscriptionGuard.add(ShowBlockFortBuildingInfoEvent::subscribe(std::bind(&gui_blockFortBuildingInfo::openBuildingInfoByEvent, this, std::placeholders::_1, std::placeholders::_2)));

		getWindow<GUIButton>("BlockFortBuilding-Btn-Remove")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortBuildingInfo::onClick, this, std::placeholders::_1, ViewId::REMOVE));
		getWindow<GUIButton>("BlockFortBuilding-Btn-Upgrade")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortBuildingInfo::onClick, this, std::placeholders::_1, ViewId::UPGRADE));
		getWindow<GUIButton>("BlockFortBuilding-Btn-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockFortBuildingInfo::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("BlockFortBuilding-Info-Close")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_blockFortBuildingInfo::onClick, this, std::placeholders::_1, ViewId::CLOSE));
	}

	bool gui_blockFortBuildingInfo::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockFort);
	}

	bool gui_blockFortBuildingInfo::onClick(const EventArgs& args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE :
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::REMOVE :
			ClientNetwork::Instance()->getCommonSender()->sendBlockFortOperation(i32(OperationType::REMOVE), m_id, m_Index);
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::UPGRADE :
			RootGuiLayout::Instance()->showMainControl();
			ShowBlockFortCommonTipEvent::emit("gui_blockcity_not_open_fun");
			break;
		default:
			break;
		}
		return true;
	}

	bool gui_blockFortBuildingInfo::openBuildingInfoByEvent(i32 id, i32 index)
	{
		m_Index = index;
		m_id = id;
		openBuildingInfo(id, false);
		return true;
	}

	bool gui_blockFortBuildingInfo::openBuildingInfo(i32 id, bool isUpdate)
	{
		if (auto itemInfos = BlockFortItemSetting::Instance()->findBuilding(id))
		{
			changeUI(*itemInfos);
			if (!isUpdate)
			{
				RootGuiLayout::Instance()->showBlockFortBuildingInfo(true);
			}
			return true;
		}
		
		return false;
	}

	void gui_blockFortBuildingInfo::changeUI(const BlockFortBuilding& itemInfos)
	{
		static i32 count = 0;
		String title = BlockFortItemSetting::Instance()->getItemName(itemInfos.id);
		String icon = BlockFortItemSetting::Instance()->getItemIcon(itemInfos.id);
		i32 type = BlockFortItemSetting::Instance()->getItemType(itemInfos.id);

		m_mainTitleText->SetText(getString(title).c_str());
		m_mainIcon->SetImage(icon.c_str());
		
		m_upgradeBtnIcon->SetImage(UICommon::getCurrencyIconByType(itemInfos.moneyType).c_str());
		setText("BlockFortBuilding-Btn-Remove-Text", "gui_blockfort_remove");

		switch (BuildingType(type))
		{
		case BuildingType::TURRET:
		case BuildingType::WALL:
		{
			bool haveSplit = false;
			String price = "";
			switch (CurrencyType(itemInfos.moneyType))
			{
			case CurrencyType::PLATFORM_MONEY:
				price = itemInfos.cube;
				break;
			case CurrencyType::PLATFORM_GOLD:
				price = itemInfos.price;
				break;
			default:break;
			}

			if (haveSplit = (StringUtil::Find(price, "#") && StringUtil::Find(price, "-")))
			{
				StringArray valArr = StringUtil::Split(price, "#");
				StringArray val = StringUtil::Split(valArr[1], "-");//BlockFort Version 1: valArr[ONE] only for display, acturaly ONE is the building level
				m_upgradeBtn->GetChildByIndex(1)->SetText(val[1].c_str());
			}

			m_upgradeBtn->SetVisible(haveSplit);
			m_descFortLayout->SetVisible(false);
			m_descTurretAndWallList->SetVisible(true);

			vector<BlockFortItemInfo>::type ItemInfos = itemInfos.infos;

			if (!m_descTurretAndWallList->getContainerWindow() || m_descTurretAndWallList->getContainerWindow()->GetChildCount() != ItemInfos.size())
			{
				m_descTurretAndWallList->ClearAllItem();
				for (const auto& item : ItemInfos)
				{
					String iterLayout = StringUtil::Format("BlockFortBuildingInfo-Items-%d", count++).c_str();
					GuiBlockFortItemInfo* itemView = (GuiBlockFortItemInfo*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKFORT_ITEM_INFO, iterLayout.c_str());
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 27.f));
					itemView->setItemInfo(item, BFItemEntrance::AttackBuilding, true);
					m_descTurretAndWallList->AddItem(itemView, false);
				}
			}
			else
			{
				for (size_t i = 0; i < m_descTurretAndWallList->getContainerWindow()->GetChildCount(); i++)
				{
					auto itemView = dynamic_cast<GuiBlockFortItemInfo*>(m_descTurretAndWallList->getContainerWindow()->GetChildByIndex(i));
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 27.f));
					itemView->setItemInfo(ItemInfos[i], BFItemEntrance::AttackBuilding, true);
				}
			}
		}
			break;
		case BuildingType::FORT:
		{
			m_descTurretAndWallList->SetVisible(false);
			m_descFortLayout->SetVisible(true);
			m_upgradeBtn->SetVisible(false);
			String desc = BlockFortItemSetting::Instance()->getItemDesc(itemInfos.id);
			setText("BlockFortBuilding-Desc-Fort-Title", "gui_blockfort_desc_tile");
			setText("BlockFortBuilding-Desc-Fort-Content", desc);
		}
			break;
		default:
			break;
		}
	}

}