#include "GuiBlockCityElevatorItem.h"
#include "UI/GUIWindowManager.h"
#include "Setting/BlockCitySetting.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"
#include "GUI/RootGuiLayout.h"
#include "Network/ClientNetwork.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"

namespace BLOCKMAN
{
	
	void GuiBlockCityElevatorItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockCityElevatorItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockCityElevatorItem::GuiBlockCityElevatorItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockCityElevatorItem.json"));
		renameComponents(m_window);

		m_floorIdText = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));

		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBlockCityElevatorItem::onClick, this, std::placeholders::_1));
		AddChildWindow(m_window);
	}

	void GuiBlockCityElevatorItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiBlockCityElevatorItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}


	bool GuiBlockCityElevatorItem::onClick(const EventArgs& args)
	{
		RootGuiLayout::Instance()->showMainControl();

		if (m_curFloorId == m_floorId)
		{
			return false;
		}

		if (Blockman::Instance()->m_pPlayer->isGlide())
		{
			ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_call_on_in_glide_use_elevator");
			return false;
		}

		if (!Blockman::Instance()->m_pPlayer->onGround)
		{
			ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_on_ground_can_use_elevator");
			return false;
		}

		ClientNetwork::Instance()->getCommonSender()->sendMoveElevator(m_floorId);
		return true;
	}

	void GuiBlockCityElevatorItem::setItem(const String& itemImage, i32 index, i32 curFloorId)
	{
		m_floorId = index;
		m_curFloorId = curFloorId;
		m_floorIdText->SetText(StringUtil::ToString(index).c_str());
		m_image->SetImage(itemImage == "" ? "set:blockcity_elevator.json image:constructing" : itemImage.c_str());
		m_window->SetBackImage(index == curFloorId ? "set:blockcity_elevator.json image:cur_floor" : "set:blockcity_elevator.json image:normal");
	}
}