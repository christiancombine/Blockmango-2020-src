#include "gui_furnacePanel.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "game.h"
#include "cItem/cItem.h"
#include "UI/GUISystem.h"
#include "External/cjson.h"
#include "Render/TextureAtlas.h"
#include "Inventory/InventoryPlayer.h"
#include "GuiDef.h"
#include "TileEntity/TileEntityInventory.h"

using namespace LORD;

namespace BLOCKMAN
{

	gui_furnacePanel::gui_furnacePanel()
		: gui_layout("FurnacePanel.json")
	{
		m_furnaceInventory = nullptr;
		m_selectStackPtr = nullptr;
	}

	gui_furnacePanel::~gui_furnacePanel()
	{
		m_subscriptionGuard.unsubscribeAll();
		m_furnaceInventory = nullptr;
		m_selectStackPtr = nullptr;
	}

	void gui_furnacePanel::onLoad()
	{
 		m_closeBtn = getWindow<GUIButton>("FurnacePanel-Btn-Close");
		m_burnTimeTxt = getWindow<GUIStaticText>("FurnacePanel-BurnTime"); 
		m_burnTimeTxt->SetVisible(false);

		m_guiMeterialScrollableInventory = getWindow<GuiScrollableInventory>("FurnacePanel-MaterialInventory");
		m_guiMeterialScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_TOUCH_UP.c_str(), SubscriberSlot(&gui_furnacePanel::placeMeterial, this));
		m_guiMeterialScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_START.c_str(), SubscriberSlot(&gui_furnacePanel::onMaterialItemStackLongTouchStart, this));
		m_guiMeterialScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_END.c_str(), SubscriberSlot(&gui_furnacePanel::onMaterialItemStackLongTouchEnd, this));
		m_guiMeterialScrollableInventory->setStyle(GuiItemSlot::Style::SKYBLOCK2);
		m_guiMeterialScrollableInventory->SetProperty(GWP_TABLE_HORIZONTAL_INTERVAL, StringUtil::Format("%d", 5).c_str());
		m_guiMeterialScrollableInventory->SetProperty(GWP_TABLE_VERTICAL_INTERVAL, StringUtil::Format("%d", 5).c_str());

		m_guiFuelScrollableInventory = getWindow<GuiScrollableInventory>("FurnacePanel-FuelInventory");
		m_guiFuelScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_TOUCH_UP.c_str(), SubscriberSlot(&gui_furnacePanel::placeFuel, this));
		m_guiFuelScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_START.c_str(), SubscriberSlot(&gui_furnacePanel::onFuelItemStackLongTouchStart, this));
		m_guiFuelScrollableInventory->subscribeEvent(GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_END.c_str(), SubscriberSlot(&gui_furnacePanel::onFuelItemStackLongTouchEnd, this));
		m_guiFuelScrollableInventory->setStyle(GuiItemSlot::Style::SKYBLOCK2);
		m_guiFuelScrollableInventory->SetProperty(GWP_TABLE_HORIZONTAL_INTERVAL, StringUtil::Format("%d", 5).c_str());
		m_guiFuelScrollableInventory->SetProperty(GWP_TABLE_VERTICAL_INTERVAL, StringUtil::Format("%d", 5).c_str());

		m_fire_img = getWindow<GUIStaticImage>("FurnacePanel-Fire");
		m_fire_img->SetImage("set:craftableImg.json image:fire_gray");
		m_fire_progress = getWindow<GUIProgressBar>("FurnacePanel-Process");
		m_fire_progress->SetVisible(false);

		m_materialSlot = getWindow<GuiItemSlot>("FurnacePanel-MeterialSlot");
		m_materialSlot->setStyle(GuiItemSlot::SKYBLOCK2);
		m_fuelSlot = getWindow<GuiItemSlot>("FurnacePanel-FuelSlot");
		m_fuelSlot->setStyle(GuiItemSlot::SKYBLOCK2);
		m_resultSlot = getWindow<GuiItemSlot>("FurnacePanel-ResultSlot");
		m_resultSlot->setStyle(GuiItemSlot::SKYBLOCK2);

		m_materialSlot->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_furnacePanel::onMaterialClick, this));
		m_materialSlot->subscribeEvent("ItemStackLongTouchStart", SubscriberSlot(&gui_furnacePanel::onMaterialLongClickStart, this));
		m_materialSlot->subscribeEvent("ItemStackLongTouchEnd", SubscriberSlot(&gui_furnacePanel::onMaterialLongClickEnd, this));

		m_fuelSlot->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_furnacePanel::onFuelClick, this));
		m_fuelSlot->subscribeEvent("ItemStackLongTouchStart", SubscriberSlot(&gui_furnacePanel::onFuelLongClickStart, this));
		m_fuelSlot->subscribeEvent("ItemStackLongTouchEnd", SubscriberSlot(&gui_furnacePanel::onFuelLongClickEnd, this));

		m_resultSlot->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_furnacePanel::onResultClick, this));
		m_resultSlot->subscribeEvent("ItemStackLongTouchStart", SubscriberSlot(&gui_furnacePanel::onResultLongClickStart, this));
		m_resultSlot->subscribeEvent("ItemStackLongTouchEnd", SubscriberSlot(&gui_furnacePanel::onResultLongClickEnd, this));

 		m_closeBtn->subscribeEvent(EventButtonClick, std::bind(&gui_furnacePanel::onCloseClick, this, std::placeholders::_1));

 		getWindow<GUIStaticText>("FurnacePanel-Title")->SetText(getString(LanguageKey::GUI_FURNACE_TITLE));
 		getWindow<GUIStaticText>("FurnacePanel-MaterialTitle")->SetText(getString(LanguageKey::GUI_FURNACE_MATERIAL_TITLE));
		getWindow<GUIStaticText>("FurnacePanel-FuelTitle")->SetText(getString(LanguageKey::GUI_FURNACE_FUEL_TITLE));
		getWindow<GUIStaticText>("FurnacePanel-MeterialTxt")->SetText(getString(LanguageKey::GUI_FURNACE_RIGHT_MATERIAL_TITLE));
		getWindow<GUIStaticText>("FurnacePanel-FuelTxt")->SetText(getString(LanguageKey::GUI_FURNACE_RIGHT_FUEL_TITLE));
		getWindow<GUIStaticText>("FurnacePanel-ResultTxt")->SetText(getString(LanguageKey::GUI_FURNACE_RIGHT_RESULT_TITLE));
	}

	void gui_furnacePanel::onShow()
	{

	}

	void gui_furnacePanel::onUpdate(ui32 nTimeElapse)
	{
		if (m_furnaceInventory == nullptr) return;

		if (m_selectStackPtr)
		{
			m_touchTime += nTimeElapse;
		}

		if (m_burnTime > 0)
		{
			// String time = StringUtil::Format("%d", int(m_burnTime / 1000)).c_str();
			// m_burnTimeTxt->SetText(time.c_str());
			// m_burnTimeTxt->SetVisible(true);
			if (m_burnMaxTime > 0 && m_burnMaxTime - m_burnTime > 0)
			{
				m_fire_progress->SetVisible(true);
				m_fire_progress->SetProgress((float)((m_burnMaxTime - m_burnTime) * 1.0 / m_burnMaxTime));
			}
			else
			{
				m_fire_progress->SetVisible(false);
				m_fire_progress->SetProgress(0.f);
			}
			
			m_burnTime -= nTimeElapse;
			m_fire_img->SetImage("set:craftableImg.json image:fire_normal");
			if (m_burnTime <= 0)
			{
				// m_burnTimeTxt->SetVisible(false);
				m_fire_progress->SetVisible(false);
				m_fire_progress->SetProgress(0.f);
				m_fire_img->SetImage("set:craftableImg.json image:fire_gray");
			}
		}
		else
		{
			// m_burnTimeTxt->SetVisible(false);
			m_fire_progress->SetVisible(false);
			m_fire_progress->SetProgress(0.f);
			m_fire_img->SetImage("set:craftableImg.json image:fire_gray");
		}

		if (isShown())
		{
			Vector3 m_pPlayer_pos = Blockman::Instance()->m_pPlayer->position;
			float dis = m_pPlayer_pos.distanceTo(m_blockPos);
			if (dis > m_closeFurnaceDis)
			{
				m_furnaceInventory = nullptr;
				getParent()->showMainControl();
				ClientNetwork::Instance()->getSender()->sendCloseContainer(BlockPos::ONE);
			}
		}

		updateData(nTimeElapse);
	}

	bool gui_furnacePanel::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockMineCave) || checkGameType(ClientGameType::SkyBlockProduct);
	}

	bool gui_furnacePanel::onCloseClick(const EventArgs & events)
	{
		m_furnaceInventory = nullptr;
		m_selectStackPtr = nullptr;
		getParent()->showMainControl();
		ClientNetwork::Instance()->getSender()->sendCloseContainer(BlockPos::ONE);
		return true;
	}

	bool gui_furnacePanel::onMaterialClick(const EventArgs & events)
	{
		exchangeItem(false, 0, 0);
		return true;
	}

	bool gui_furnacePanel::onMaterialLongClickStart(const EventArgs & events)
	{
		m_touchTime = 75;
		m_selectStackPtr = m_furnaceInventory->getStackInSlot(0);
		return true;
	}

	bool gui_furnacePanel::onMaterialLongClickEnd(const EventArgs & events)
	{
		exchangeItem(false, m_touchTime, 0);
		return true;
	}

	bool gui_furnacePanel::onFuelClick(const EventArgs & events)
	{
		exchangeItem(false, 0, 1);
		return true;
	}

	bool gui_furnacePanel::onFuelLongClickStart(const EventArgs & events)
	{
		m_touchTime = 75;
		m_selectStackPtr = m_furnaceInventory->getStackInSlot(1);
		return true;
	}

	bool gui_furnacePanel::onFuelLongClickEnd(const EventArgs & events)
	{
		exchangeItem(false, m_touchTime, 1);
		return true;
	}

	bool gui_furnacePanel::onResultClick(const EventArgs & events)
	{
		exchangeItem(false, 0, 2);
		return true;
	}

	bool gui_furnacePanel::onResultLongClickStart(const EventArgs & events)
	{
		m_touchTime = 75;
		m_selectStackPtr = m_furnaceInventory->getStackInSlot(2);
		return true;
	}

	bool gui_furnacePanel::onResultLongClickEnd(const EventArgs & events)
	{
		exchangeItem(false, m_touchTime, 2);
		return true;
	}

	bool gui_furnacePanel::placeMeterial(const EventArgs & events)
	{
		int index = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(events).index;
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		if (inventory == nullptr) return true;

		IInventory *invMaterial = inventory->getMaterialFilteredInventory();
		ItemStackPtr itemMaterial = invMaterial->getStackInSlot(index);

		if (itemMaterial == nullptr) return true;

		index = inventory->findItemStack(itemMaterial);

		if (index == -1) return true;

		exchangeItem(true, 0, index, 0);
		return true;
	}

	bool gui_furnacePanel::onMaterialItemStackLongTouchStart(const EventArgs & events)
	{
		m_touchTime = 75;
		auto playerInventory = Blockman::Instance()->m_pPlayer->inventory;
		m_selectStackPtr = getClickItem(playerInventory, events);
		return false;
	}

	bool gui_furnacePanel::onMaterialItemStackLongTouchEnd(const EventArgs & events)
	{
		int index = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(events).index;
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		if (inventory == nullptr) return true;

		IInventory *invMaterial = inventory->getMaterialFilteredInventory();
		ItemStackPtr itemMaterial = invMaterial->getStackInSlot(index);

		if (itemMaterial == nullptr) return true;

		index = inventory->findItemStack(itemMaterial);

		if (index == -1) return true;

		exchangeItem(true, m_touchTime, index, 0);
		return false;
	}

	bool gui_furnacePanel::placeFuel(const EventArgs & events)
	{
		int index = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(events).index;

		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		if (inventory == nullptr) return true;

		IInventory *invFuel = inventory->getFuelFilteredInventory();
		ItemStackPtr itemFuel = invFuel->getStackInSlot(index);

		if (itemFuel == nullptr) return true;

		index = inventory->findItemStack(itemFuel);

		if (index == -1) return true;

		exchangeItem(true, 0, index, 1);
		return true;
	}

	bool gui_furnacePanel::onFuelItemStackLongTouchStart(const EventArgs & events)
	{
		m_touchTime = 75;
		auto playerInventory = Blockman::Instance()->m_pPlayer->inventory;
		m_selectStackPtr = getClickItem(playerInventory, events);
		return false;
	}

	bool gui_furnacePanel::onFuelItemStackLongTouchEnd(const EventArgs & events)
	{
		int index = dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(events).index;

		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		if (inventory == nullptr) return true;

		IInventory *invFuel = inventory->getFuelFilteredInventory();
		ItemStackPtr itemFuel = invFuel->getStackInSlot(index);

		if (itemFuel == nullptr) return true;

		index = inventory->findItemStack(itemFuel);

		if (index == -1) return true;

		exchangeItem(true, m_touchTime, index, 1);
		return true;
	}

	void gui_furnacePanel::updateData(ui32 nTimeElapse)
	{
		auto inventory = Blockman::Instance()->m_pPlayer->inventory;
		if (inventory == nullptr)
			return;

		if (dynamic_cast<TileEntityFurnace*>(m_furnaceInventory) == nullptr)
			return;

		m_guiMeterialScrollableInventory->setInventory(inventory->getMaterialFilteredInventory());
		m_guiFuelScrollableInventory->setInventory(inventory->getFuelFilteredInventory());

		m_materialSlot->setItemStack(dynamic_cast<TileEntityFurnace*>(m_furnaceInventory)->getStackInSlot(0));
		m_fuelSlot->setItemStack(dynamic_cast<TileEntityFurnace*>(m_furnaceInventory)->getStackInSlot(1));
		m_resultSlot->setItemStack(dynamic_cast<TileEntityFurnace*>(m_furnaceInventory)->getStackInSlot(2));
	}

	void gui_furnacePanel::setInventory(IInventory * inventory)
	{
		this->m_furnaceInventory = inventory;
	}

	void gui_furnacePanel::setInventory(IInventory * inventory, const Vector3i & blockPos, int face, const Vector3 & hisPos)
	{
		this->m_furnaceInventory = inventory;
		this->m_face = face;
		this->m_hisPos = hisPos;
		this->m_blockPos = blockPos;
	}

	void gui_furnacePanel::exchangeItem(bool isPlayerInventoryToChest, ui32 touchTime, int index, int targetIndex)
	{
		m_touchTime = 0;
		m_selectStackPtr = nullptr;
		ClientNetwork::Instance()->getSender()->sendExchangeItemStack(isPlayerInventoryToChest, touchTime, index, m_blockPos, m_face, m_hisPos, targetIndex, false);
	}

	ItemStackPtr gui_furnacePanel::getClickItem(IInventory* inventory, const EventArgs & event)
	{
		return inventory ? inventory->getStackInSlot(dynamic_cast<const GuiScrollableInventory::ItemStackTouchEventArgs&>(event).index) : nullptr;
	}

}
