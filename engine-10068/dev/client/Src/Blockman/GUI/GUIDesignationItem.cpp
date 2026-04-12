#include "GUIDesignationItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Network/ClientNetwork.h"
namespace BLOCKMAN
{
	GUIDesignationItem::GUIDesignationItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("DesignationItem.json"));
		renameComponents(m_window);
		initView();
		AddChildWindow(m_window);
	}

	void GUIDesignationItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIDesignationItem::initView()
	{
		m_left_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_left_image_text = dynamic_cast<GUIStaticText*>(m_left_image->GetChildByIndex(0));
		m_center_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_center_image_text = dynamic_cast<GUIStaticText*>(m_center_image->GetChildByIndex(0));
		m_right_button = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
	}

	void GUIDesignationItem::UpdateSelf(ui32 nTimeElapse)
	{

	}

	void GUIDesignationItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIDesignationItem::InitDesignationItem(DesignationItem * item)
	{
		m_data = item;
		m_left_image_text->SetText(LanguageManager::Instance()->getString(item->Name).c_str());

		String key = LanguageManager::Instance()->getString(item->ConditionName);
		DesignationTotal* total = DesignationSetting::findDesignationTotalById(item->Type);
		if (total == nullptr)
		{
			return;
		}
		key += StringUtil::ToString(total->num >= item->Condition ? item->Condition : total->num);
		key += "/";
		key += StringUtil::ToString(item->Condition);
		m_center_image_text->SetText(key.c_str());

		if (item->Status == NOT_HAS)
		{
			m_right_button->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_DESIGNATION_ITEM_NOT_HAS).c_str());
			m_right_button->SetTextColor(Color(177.0f / 255.0f, 177.0f / 255.0f, 177.0f / 255.0f));
			m_right_button->SetNormalImage("set:walking_dead_designation.json image:xiaoanniu3");
			m_right_button->SetPushedImage("set:walking_dead_designation.json image:xiaoanniu3");
		}
		else if (item->Status == HAS)
		{
			m_right_button->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_DESIGNATION_ITEM_HAS).c_str());
			m_right_button->subscribeEvent(EventButtonClick, std::bind(&GUIDesignationItem::OnItemChick, this, std::placeholders::_1));
			m_right_button->SetTextColor(Color(227.0f / 255.0f, 206.0f / 255.0f, 162.0f / 255.0f));
			m_right_button->SetNormalImage("set:walking_dead_designation.json image:xiaoanniu1");
			m_right_button->SetPushedImage("set:walking_dead_designation.json image:xiaoanniu1");
		}
		else if (item->Status == USE)
		{
			m_right_button->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_DESIGNATION_ITEM_USE).c_str());
			m_right_button->subscribeEvent(EventButtonClick, std::bind(&GUIDesignationItem::OnItemChick, this, std::placeholders::_1));
			m_right_button->SetTextColor(Color(72.0f / 255.0f, 64.0f / 255.0f, 44.0f / 255.0f));
			m_right_button->SetNormalImage("set:walking_dead_designation.json image:xiaoanniu2");
			m_right_button->SetPushedImage("set:walking_dead_designation.json image:xiaoanniu2");

		}
	}

	bool GUIDesignationItem::OnItemChick(const EventArgs & events)
	{
		if (m_data == NULL)
			return false;
		if (m_data->Status == HAS)
		{
			ClientNetwork::Instance()->getCommonSender()->sendWalkingDeadDesignationChange(m_data->Id, m_data->Status);
		}
		else if (m_data->Status == USE)
		{
			ClientNetwork::Instance()->getCommonSender()->sendWalkingDeadDesignationChange(m_data->Id, m_data->Status);
		}
		return true;
	}

}