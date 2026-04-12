#include "gui_blockCitySaveFile.h"
#include "GUI/GuiBlockCitySaveFileItem.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/RootGuiLayout.h"
#include "Game/BlockCity.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "Util/TipType.h"
#include "GuiDef.h"
#include "UI/GUIListBox.h"

namespace BLOCKMAN
{
	gui_blockCitySaveFile::gui_blockCitySaveFile()
		: gui_layout("BlockCitySaveFile.json")
	{
	}

	gui_blockCitySaveFile::~gui_blockCitySaveFile()
	{
		m_mapItem.clear();
	}

	void gui_blockCitySaveFile::onLoad()
	{
		setText("BlockCitySaveFile-Title-Text", "gui_blockcity_save_file");
		setText("BlockCitySaveFile-Delete", "gui_blockcity_save_file_delete");
		setText("BlockCitySaveFile-Save", "gui_blockcity_save_file_save");
		setText("BlockCitySaveFile-Read", "gui_blockcity_save_file_read");

		m_list = getWindow<GUIListBox>("BlockCitySaveFile-List");

		getWindow("BlockCitySaveFile-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCitySaveFile::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("BlockCitySaveFile-Delete")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCitySaveFile::onClick, this, std::placeholders::_1, ViewId::DEL));
		getWindow("BlockCitySaveFile-Save")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCitySaveFile::onClick, this, std::placeholders::_1, ViewId::SAVE));
		getWindow("BlockCitySaveFile-Read")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCitySaveFile::onClick, this, std::placeholders::_1, ViewId::READ));
		m_subscriptionGuard.add(UpdateBlockCityArchiveEvent::subscribe(std::bind(&gui_blockCitySaveFile::onDataChange, this)));
		m_subscriptionGuard.add(BlockCityArchiveItemClickEvent::subscribe(std::bind(&gui_blockCitySaveFile::onItemClick, this, std::placeholders::_1)));
		m_subscriptionGuard.add(BlockCityArchiveSureEvent::subscribe(std::bind(&gui_blockCitySaveFile::tipSure, this, std::placeholders::_1)));
	}

	void gui_blockCitySaveFile::onShow()
	{
		if (m_dataChange)
		{
			onSetDatas();
			m_dataChange = false;
		}
	}

	void gui_blockCitySaveFile::onHide()
	{
	}

	bool gui_blockCitySaveFile::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCitySaveFile::onClick(const EventArgs& events, ViewId viewId)
	{
		switch (viewId)
		{
		case BLOCKMAN::gui_blockCitySaveFile::ViewId::CLOSE:
			RootGuiLayout::Instance()->setShowBlockCitySaveFile(false);
			break;
		case BLOCKMAN::gui_blockCitySaveFile::ViewId::DEL:
			if (m_mapItem[m_curItemIndex].updateAt == "" || m_mapItem[m_curItemIndex].updateAt == "0")
			{
				break;
			}
			else
			{
				if (m_isSureDelete)
				{
					m_isSureDelete = false;
					ClientNetwork::Instance()->getCommonSender()->sendBlockCityOperateArchiveData(m_curItemIndex, 1);
				}
				else
				{
					ShowBlockCityShowCommonTipEvent::emit(vector<BlockCityPayment>::type(), (i32)BlockCityTipType::BLOCKCITY_DELETE_ARCHIVE, "gui_blockcity_save_file_sure_delete");
				}
			}
			break;
		case BLOCKMAN::gui_blockCitySaveFile::ViewId::SAVE:
			if (m_mapItem[m_curItemIndex].updateAt == "0" || m_isSureSave)
			{
				m_isSureSave = false;
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityOperateArchiveData(m_curItemIndex, 2);
				break;
			}
			else if (m_mapItem[m_curItemIndex].updateAt == "")
			{
				if (m_curNum < m_maxNum)
				{
					vector<BlockCityPayment>::type pays;
					BlockCityPayment pay;
					pay.num = 1;
					pay.price = m_nextPrice;
					pay.moneyType = 1;
					pays.push_back(pay);
					ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_ARCHIVE_BUY_SAVE, "gui_blockcity_save_file_buy_tip");
				}
				else
				{
					ShowBlockCityTipEvent::emit("gui_blockcity_max_num");
				}
			}
			else
			{
				ShowBlockCityShowCommonTipEvent::emit(vector<BlockCityPayment>::type(), (i32)BlockCityTipType::BLOCKCITY_ARCHIVE_COVER, "gui_blockcity_save_file_cover");
			}
			break;
		case BLOCKMAN::gui_blockCitySaveFile::ViewId::READ:
			if (m_mapItem[m_curItemIndex].updateAt == "" || m_mapItem[m_curItemIndex].updateAt == "0")
			{
				break;
			}
			else
			{
				if (m_isSureRead)
				{
					m_isSureRead = false;
					ClientNetwork::Instance()->getCommonSender()->sendBlockCityOperateArchiveData(m_curItemIndex, 3);
					RootGuiLayout::Instance()->showMainControl();
				}
				else
				{
					ShowBlockCityShowCommonTipEvent::emit(vector<BlockCityPayment>::type(), (i32)BlockCityTipType::BLOCKCITY_ARCHIVE_READ, "gui_blockcity_save_file_if_read");
				}
			}
			break;
		default:
			break;
		}
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_blockCitySaveFile::onDataChange()
	{
		m_dataChange = true;
		if (isShown())
		{
			onShow();
		}
		return true;
	}

	void gui_blockCitySaveFile::onSetDatas()
	{
		if (!UICommon::checkBlockCityParam())
		{
			return;
		}
		auto archive = Blockman::Instance()->m_pPlayer->m_blockCity->getArchive();

		if (archive)
		{
			m_nextPrice = archive->nextPrice;
			m_maxNum = archive->maxNum;
			m_curNum = archive->items.size();
			static i32 count = 0;
			m_list->ClearAllItem();
			m_list->SetInterval(2.f);
			m_mapItem.clear();
			float itemY = (m_list->GetPixelSize().y - 2 * 2.f) / 3;
			for (size_t i = 0; i < archive->items.size() + 1; i++)
			{
				if (i < archive->items.size())
				{
					String iterLayout = StringUtil::Format("BlockCitySaveFile-Items-%d", count++).c_str();
					GuiBlockCitySaveFileItem* itemView = (GuiBlockCitySaveFileItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_SAVE_FILE_ITEM, iterLayout.c_str());
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0), UDim(0.0f, itemY));
					itemView->onSetItem(archive->items[i], archive->nextPrice, archive->maxNum, i);
					m_mapItem.insert({ i, archive->items[i] });
					m_list->AddItem(itemView, false);
				}
				else
				{
					if ((i32)archive->items.size() < m_maxNum)
					{
						String iterLayout = StringUtil::Format("BlockCitySaveFile-Items-%d", count++).c_str();
						GuiBlockCitySaveFileItem* itemView = (GuiBlockCitySaveFileItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_SAVE_FILE_ITEM, iterLayout.c_str());
						itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0), UDim(0.0f, itemY));
						itemView->onSetItem(BlockCityArchiveItem(), archive->nextPrice, archive->maxNum, i);
						itemView->setCurNum(archive->items.size());
						m_mapItem.insert({ i, BlockCityArchiveItem() });
						m_list->AddItem(itemView, false);
					}
				}
			}

			i32  defaultItemIndex = -1;

			for (size_t i = 0; i < archive->items.size() + 1; i++)
			{
				if (i == 0)
				{
					defaultItemIndex = i;
				}
				if (i == m_curItemIndex)
				{
					defaultItemIndex = i;
					break;
				}

			}
			if (defaultItemIndex >= 0)
			{
				m_curItemIndex = defaultItemIndex;
				BlockCityArchiveItemClickEvent::emit(m_curItemIndex);
			}
		}
		m_list->UpdateSelf(0);
		/*m_list->ResetChildVisible();*/
	}

	bool gui_blockCitySaveFile::onItemClick(i32 index)
	{
		m_curItemIndex = index;
		for (size_t i = 0; i < m_list->getContainerWindow()->GetChildCount(); i++)
		{
			auto itemView = dynamic_cast<GuiBlockCitySaveFileItem*>(m_list->getContainerWindow()->GetChildByIndex(i));
			itemView->setSelect(m_curItemIndex);
		}
		if (m_mapItem[m_curItemIndex].updateAt == "" || m_mapItem[m_curItemIndex].updateAt == "0")
		{
			getWindow("BlockCitySaveFile-Read")->SetEnabled(false);
			getWindow("BlockCitySaveFile-Delete")->SetEnabled(false);
		}
		else
		{
			getWindow("BlockCitySaveFile-Read")->SetEnabled(true);
			getWindow("BlockCitySaveFile-Delete")->SetEnabled(true);
		}
		return true;
	}

	bool gui_blockCitySaveFile::tipSure(i32 type)
	{
		switch (type)
		{
		case 1:
			m_isSureDelete = true;
			onClick(EventArgs(), ViewId::DEL);
			break;
		case 2:
			m_isSureSave = true;
			onClick(EventArgs(), ViewId::SAVE);
			break;
		case 3:
			m_isSureRead = true;
			onClick(EventArgs(), ViewId::READ);
			break;
		default:
			break;
		}
		return true;
	}
}