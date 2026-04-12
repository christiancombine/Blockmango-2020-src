#include "gui_blockCityChoosePaper.h"
#include "GUI/GuiBlockCityChoosePaperItem.h"
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
	gui_blockCityChoosePaper::gui_blockCityChoosePaper()
		: gui_layout("BlockCityChoosePaper.json")
	{
	}

	gui_blockCityChoosePaper::~gui_blockCityChoosePaper()
	{
	}

	void gui_blockCityChoosePaper::onLoad()
	{
		setText("BlockCityChoosePaper-Sure", "gui_blockcity_tip_sure");
		setText("BlockCityChoosePaper-Title", "gui_blockcity_choose_paper");
		m_items = getWindow<GUILayout>("BlockCityChoosePaper-Items");
		getWindow("BlockCityChoosePaper-Sure")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityChoosePaper::onSure, this, std::placeholders::_1));
		m_subscriptionGuard.add(BlockCityShowChooseItemEvent::subscribe(std::bind(&gui_blockCityChoosePaper::open, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BlockCityChooseItemClickEvent::subscribe(std::bind(&gui_blockCityChoosePaper::onItemClick, this, std::placeholders::_1)));
	}

	void gui_blockCityChoosePaper::onShow()
	{
	
	}

	void gui_blockCityChoosePaper::onHide()
	{
		if (m_guideId > 0 && m_propId > 0 && ClientNetwork::Instance()->isAlive())
		{
			ClientNetwork::Instance()->getCommonSender()->sendBlockCityChooseProp(m_guideId, m_propId);
			GuideSystemSetting::setGuideStatus(m_guideId, STATUS_DONE);
			m_guideId = 0;
			m_propId = 0;
		}
	}

	bool gui_blockCityChoosePaper::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCityChoosePaper::onSure(const EventArgs& events)
	{
		playSoundByType(ST_Click);
		RootGuiLayout::Instance()->showMainControl();
		return true;
	}

	bool gui_blockCityChoosePaper::open(i32 guideId, const vector<i32>::type& items)
	{
		if (UICommon::checkBlockCityParam())
		{
			m_guideId = guideId;
			vector<BlockCityStoreItem>::type storeItems = vector<BlockCityStoreItem>::type();
			auto store = Blockman::Instance()->m_pPlayer->m_blockCity->getStore();
			for (const auto& tab : store)
			{
				for (const auto& item : tab.items)
				{
					for (i32 id : items)
					{
						if (item.id == id)
						{
							storeItems.push_back(item);
							break;
						}
					}
				}
			}
			initItems(storeItems);
			RootGuiLayout::Instance()->showBlockCityChooseView();
		}
		return true;
	}

	void gui_blockCityChoosePaper::initItems(const vector<BlockCityStoreItem>::type& items)
	{
		static i32 count = 0;
		m_items->CleanupChildren();
		float itemX = (m_items->GetPixelSize().x - 3 * 20.f) / 4;
		for (size_t i = 0; i < items.size(); i++)
		{
			if (i == 0)
			{
				m_propId = items[i].id;
			}
			String iterLayout = StringUtil::Format("BlockCityChoosePaper-Items-%d", count++).c_str();
			GuiBlockCityChoosePaperItem* itemView = (GuiBlockCityChoosePaperItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_CHOOSE_PAPER_ITEM, iterLayout.c_str());
			itemView->SetArea(UDim(0, i * (itemX + 20.f)), UDim(0, 0), UDim(0.0f, itemX), UDim(1.0f, 0.f));
			itemView->setItem(items[i]);
			itemView->onItemClick(m_propId);
			m_items->AddChildWindow(itemView);
		}
	}


	bool gui_blockCityChoosePaper::onItemClick(i32 propId)
	{
		m_propId = propId;
		return true;
	}
}