#include "GUIBedWarRuneProItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Setting/RuneSetting.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIFont.h"

namespace BLOCKMAN
{

	void GuiBedWarRuneProItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}


	vector<GUIWindow*>::type GuiBedWarRuneProItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBedWarRuneProItem::GuiBedWarRuneProItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = GUIWindowManager::Instance()->LoadWindowFromJSON("BedWarRuneProItem.json");
		renameComponents(m_window);

		m_rune_img_bg = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_rune_pro_txt = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));

		AddChildWindow(m_window);
		subscribeToBubbleUpMouseEvents();
	}

	void GuiBedWarRuneProItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}


	void GuiBedWarRuneProItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBedWarRuneProItem::RefreshItem(int index, int rune_id, bool select)
	{
		m_rune_id = rune_id;

		RuneSettingItem* rune_item = RuneSetting::findRuneItemById(m_rune_id);
		if (!rune_item) return;

		m_rune_pro_txt->SetText(getString(rune_item->DesPro.c_str()).c_str());
		m_rune_pro_txt->SetFont(GUIFontManager::Instance()->GetFont("HT14"));
		m_rune_img_bg->SetImage("");
		m_rune_img_bg->SetBackgroundColor(index % 2 == 0 ? Color(183.f / 255.f, 123.f / 255.f, 72.f / 255.f, 1.f)
			: Color(194.f / 255.f, 138.f / 255.f, 85.f / 255.f, 1.f));
		if (select)
		{
			m_rune_img_bg->SetImage("set:rune.json image:rune_des_bg3");
			// m_rune_pro_txt->SetFont(GUIFontManager::Instance()->GetFont("HT16"));
		}
	}
}