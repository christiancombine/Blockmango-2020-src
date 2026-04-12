#include "GuiBlockCityChoosePaperItem.h"
#include "UI/GUIWindowManager.h"
#include "Setting/BlockCitySetting.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"

namespace BLOCKMAN
{
	
	void GuiBlockCityChoosePaperItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockCityChoosePaperItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockCityChoosePaperItem::GuiBlockCityChoosePaperItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockCityChoosePaperItem.json"));
		renameComponents(m_window);

		m_chooseFrame = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0));
		m_frameUp = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_frameDown = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3)->GetChildByIndex(0));
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4)->GetChildByIndex(0));
		m_volume = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5)->GetChildByIndex(1));
		m_score = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(6)->GetChildByIndex(1));
		m_volumeText = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5)->GetChildByIndex(0));
		m_scoreText = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(6)->GetChildByIndex(0));
		

		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBlockCityChoosePaperItem::onClick, this, std::placeholders::_1));
		m_subscriptionGuard.add(BlockCityChooseItemClickEvent::subscribe(std::bind(&GuiBlockCityChoosePaperItem::onItemClick, this, std::placeholders::_1)));
		AddChildWindow(m_window);
	}

	void GuiBlockCityChoosePaperItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiBlockCityChoosePaperItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}


	bool GuiBlockCityChoosePaperItem::onClick(const EventArgs& args)
	{
		if (m_item.id)
		{
			BlockCityChooseItemClickEvent::emit(m_item.id);
		}
		return true;
	}

	void GuiBlockCityChoosePaperItem::setItem(const BlockCityStoreItem & item)
	{
		m_volumeText->SetText(LanguageManager::Instance()->getString("gui_blockcity_store_vol").c_str());
		m_scoreText->SetText(LanguageManager::Instance()->getString("gui_blockcity_store_score").c_str());
		m_name->SetText(getString(BlockCitySetting::getItemName(item.id)));
		m_image->SetImage(BlockCitySetting::getItemIcon(item.id).c_str());
		m_score->SetText(StringUtil::ToString( item.score).c_str());
		m_volume->SetText(item.volume.c_str());
		m_item = item;
	}

	bool GuiBlockCityChoosePaperItem::onItemClick(i32 itemId)
	{
		m_chooseFrame->SetVisible(m_item.id == itemId);
		m_frameUp->SetVisible(m_item.id != itemId);
		m_frameDown->SetVisible(m_item.id != itemId);
		return true;
	}
}