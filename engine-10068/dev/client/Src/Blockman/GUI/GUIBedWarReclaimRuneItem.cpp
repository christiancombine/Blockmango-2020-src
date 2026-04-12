#include "GUIBedWarReclaimRuneItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Setting/RuneSetting.h"

namespace BLOCKMAN
{

	void GuiBedWarReclaimRuneItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}


	vector<GUIWindow*>::type GuiBedWarReclaimRuneItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBedWarReclaimRuneItem::GuiBedWarReclaimRuneItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = GUIWindowManager::Instance()->LoadWindowFromJSON("BedWarReclaimRuneItem.json");
		renameComponents(m_window);


		m_rune_img = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_rune_num_txt = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0)->GetChildByIndex(0));
		m_rune_title = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_rune_des = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_rune_btn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_rune_btn->subscribeEvent(EventButtonClick, std::bind(&GuiBedWarReclaimRuneItem::onRuneClick, this, std::placeholders::_1));


		AddChildWindow(m_window);
		subscribeToBubbleUpMouseEvents();
	}

	void GuiBedWarReclaimRuneItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}


	void GuiBedWarReclaimRuneItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBedWarReclaimRuneItem::RefreshItem(int id, int num)
	{
		m_rune_id = id;
		m_rune_num = num;

		RuneSettingItem* rune_item = RuneSetting::findRuneItemById(m_rune_id);
		if (!rune_item) return;

		m_rune_num_txt->SetText(StringUtil::Format("X%d", m_rune_num).c_str());

		if (rune_item->Image.length() > 0)
		{
			m_rune_img->SetImage(rune_item->Image.c_str());
			m_rune_img->SetVisible(true);
		}
		else
		{
			m_rune_img->SetVisible(false);
		}

		if (rune_item->Name.length() > 0)
		{
			m_rune_title->SetText(getString(rune_item->Name.c_str()).c_str());
			m_rune_title->SetVisible(true);
		}
		else
		{
			m_rune_title->SetVisible(false);
		}

		if (rune_item->Des.length() > 0)
		{
			m_rune_des->SetText(getString(rune_item->Des.c_str()).c_str());
			m_rune_des->SetVisible(true);
		}
		else
		{
			m_rune_des->SetVisible(false);
		}
	}

	bool GuiBedWarReclaimRuneItem::onRuneClick(const EventArgs & events)
	{
		BedWarRuneReclaimClickEvent::emit(m_rune_id);
		return true;
	}
}