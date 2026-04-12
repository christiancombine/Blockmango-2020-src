#include "gui_blockCityReceiveFlower.h"
#include "GUI/GuiBlockCityReceiveFlowerItem.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "Network/ClientHttpRequest.h"
#include "cWorld/Blockman.h"
#include "GuiDef.h"
#include "UI/GUIListBox.h"

namespace BLOCKMAN
{
	gui_blockCityReceiveFlower::gui_blockCityReceiveFlower()
		: gui_layout("BlockCityReceiveFlower.json")
	{
	}

	gui_blockCityReceiveFlower::~gui_blockCityReceiveFlower()
	{
	}

	void gui_blockCityReceiveFlower::onLoad()
	{
		setText("BlockCityReceiveFlower-Title", "gui_blockcity_receive_flower");

		m_list = getWindow<GUIListBox>("BlockCityReceiveFlower-List");
		m_list->SetInterval(2.f);

		getWindow("BlockCityReceiveFlower-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityReceiveFlower::onClose, this, std::placeholders::_1));
		m_subscriptionGuard.add(UpdateBlockCityThumbUpRecordEvent::subscribe(std::bind(&gui_blockCityReceiveFlower::onUpdateThumbUpRecord, this, std::placeholders::_1)));
		m_subscriptionGuard.add(DeleteBlockCityThumbUpRecordEvent::subscribe(std::bind(&gui_blockCityReceiveFlower::onSuccessDeleteThumbUpRecord, this)));
	}

	void gui_blockCityReceiveFlower::onShow()
	{
		if (m_list)
		{
			ClientHttpRequest::getThumbUpRecord(0, 20);
		}
	}

	bool gui_blockCityReceiveFlower::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCityReceiveFlower::onClose(const EventArgs& events)
	{
		playSoundByType(ST_Click);
		RootGuiLayout::Instance()->setShowBlockCityReceiveFlower(false);
		return true;
	}

	bool gui_blockCityReceiveFlower::onUpdateThumbUpRecord(const vector<ThumbUpRecord>::type& records)
	{
		static i32 count = 0;
		m_list->ClearAllItem();
 		for (auto& record : records)
 		{
			String itemLayout = StringUtil::Format("BlockCityReceiveFlower-List-%d", count++).c_str();
			GuiBlockCityReceiveFlowerItem *itemView = (GuiBlockCityReceiveFlowerItem *)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_RECEIVE_FLOWER_ITEM, itemLayout.c_str());
			itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.f), UDim(0.0f, 73.f));
			itemView->setItem(record);
			m_list->AddItem(itemView, false);
 		}
		return true;
	}

	bool gui_blockCityReceiveFlower::onSuccessDeleteThumbUpRecord()
	{
		ClientHttpRequest::getThumbUpRecord(0, 20);
		return true;
	}
}