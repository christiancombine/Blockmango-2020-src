#include "GuiBlockCitySaveFileItem.h"
#include "UI/GUIWindowManager.h"
#include "Setting/BlockCitySetting.h"
#include "Util/ClientEvents.h"
#include "GUI/GuiItemStack.h"
#include "Util/TipType.h"
#include "Network/ClientNetwork.h"
#include "Util/LanguageManager.h"

namespace BLOCKMAN
{
	
	void GuiBlockCitySaveFileItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockCitySaveFileItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockCitySaveFileItem::GuiBlockCitySaveFileItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockCitySaveFileItem.json"));
		renameComponents(m_window);

		m_num = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_info = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(1));
		m_record = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));
		m_recordName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(0)->GetChildByIndex(0));
		m_recordDate = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(0)->GetChildByIndex(1));
		m_nullRecord = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(1));
		m_button = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(2));
		m_mod_name = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2)->GetChildByIndex(0));
		m_add = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2)->GetChildByIndex(1));
		m_edit_name = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(3));
		m_edit = dynamic_cast<GUIEditBox*>(m_edit_name->GetChildByIndex(0));
		m_sure_change_name = dynamic_cast<GUIButton*>(m_edit_name->GetChildByIndex(1));
		m_sure_change_name->SetText(LanguageManager::Instance()->getString("gui_blockcity_tip_sure").c_str());

		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBlockCitySaveFileItem::onClick, this, std::placeholders::_1, ViewId::MAIN));
		m_mod_name->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCitySaveFileItem::onClick, this, std::placeholders::_1, ViewId::CHANGE_NAME));
		m_add->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCitySaveFileItem::onClick, this, std::placeholders::_1, ViewId::ADD));
		m_sure_change_name->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCitySaveFileItem::onClick, this, std::placeholders::_1, ViewId::SURE_CHANGE_NAME));
		m_edit->subscribeEvent(EventWindowTextChanged, std::bind(&GuiBlockCitySaveFileItem::onInputChanged, this, std::placeholders::_1));

		AddChildWindow(m_window);
	}

	void GuiBlockCitySaveFileItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);

		m_edit_name->SetVisible(m_isShowEdit);
	}

	
	void GuiBlockCitySaveFileItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBlockCitySaveFileItem::onSetItem(BlockCityArchiveItem item, i32 nextPrice, i32 maxNum, i32 index)
	{
		m_num->SetText(StringUtil::ToString(index + 1).c_str());
		m_index = index;
		m_nextPrice = nextPrice;
		m_maxNum = maxNum;
		if (item.updateAt == "")
		{
			m_info->SetVisible(false);
			m_button->SetVisible(true);
			m_add->SetVisible(true);
			m_mod_name->SetVisible(false);
		}
		else if (item.updateAt == "0")
		{
			m_nullRecord->SetVisible(false);
			m_record->SetVisible(true);
			//m_recordName->SetText(item.name.c_str());
			m_recordName->SetText(LanguageManager::Instance()->getString("gui_blockcity_save_file_name_null").c_str());
			m_recordDate->SetText(LanguageManager::Instance()->getString("gui_blockcity_save_file_time_null").c_str());
			m_button->SetVisible(true);
			m_mod_name->SetVisible(false);
			m_add->SetVisible(false);
		}
		else
		{
			m_nullRecord->SetVisible(false);
			m_record->SetVisible(true);
			m_recordName->SetText(item.name.c_str());
			m_recordDate->SetText(item.updateAt.c_str());
			m_button->SetVisible(true);
			m_mod_name->SetVisible(true);
			m_add->SetVisible(false);
		}
	}

	void GuiBlockCitySaveFileItem::setCurNum(i32 num)
	{
		m_curNum = num;
	}

	bool GuiBlockCitySaveFileItem::onInputChanged(const EventArgs & events)
	{
		std::string text = m_edit->GetText().c_str();
		m_edit->SetText(text.c_str());
		return true;
	}

	bool GuiBlockCitySaveFileItem::onClick(const EventArgs& args, ViewId viewId)
	{
		switch (viewId)
		{
		case BLOCKMAN::GuiBlockCitySaveFileItem::ViewId::MAIN:
			if (m_index >= 0)
			{
				BlockCityArchiveItemClickEvent::emit(m_index);
			}
			break;
		case BLOCKMAN::GuiBlockCitySaveFileItem::ViewId::CHANGE_NAME:
			m_isShowEdit = true;
			m_button->SetVisible(false);
			break;
		case BLOCKMAN::GuiBlockCitySaveFileItem::ViewId::ADD:
			if (m_curNum < m_maxNum)
			{
				vector<BlockCityPayment>::type pays;
				BlockCityPayment pay;
				pay.num = 1;
				pay.price = m_nextPrice;
				pay.moneyType = 1;
				pays.push_back(pay);
				ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_ARCHIVE_BUY, "gui_blockcity_save_file_buy_tip");
			}
			else
			{
				ShowBlockCityTipEvent::emit("gui_blockcity_max_num");
			}
			break;
		case BLOCKMAN::GuiBlockCitySaveFileItem::ViewId::SURE_CHANGE_NAME:
			if (m_edit->GetText().length() > 0)
			{
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityChangeArchiveName(m_index, m_edit->GetText().c_str());
			}
			else
			{
				ShowBlockCityTipEvent::emit("gui_blockcity_save_file_change_name_fuse");
			}
			m_isShowEdit = false;
			m_button->SetVisible(true);
			m_edit->SetText("");
			break;
		default:
			break;
		}
		return false;
	}

	void GuiBlockCitySaveFileItem::setSelect(i32 index)
	{
		m_isShowEdit = false;
		m_button->SetVisible(true);
		m_edit->SetText("");
		m_isSelect = (index > -1 && index == m_index);
		m_window->SetBackImage(m_isSelect ? "set:blockcity_save_file.json image:save_file_choose" : "set:blockcity_save_file.json image:save_file_other");
	}
}