#include "GuiBlockCityCharmRankItem.h"
#include "UI/GUIWindowManager.h"
#include "Setting/BlockCitySetting.h"
#include "Util/ClientEvents.h"
#include "GUI/GuiItemStack.h"

namespace BLOCKMAN
{
	
	void GuiBlockCityCharmRankItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockCityCharmRankItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockCityCharmRankItem::GuiBlockCityCharmRankItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockCityCharmRankItem.json"));
		renameComponents(m_window);

		m_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_ranks = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_award = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(2));

		AddChildWindow(m_window);
	}

	void GuiBlockCityCharmRankItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiBlockCityCharmRankItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBlockCityCharmRankItem::onSetItem(BlockCityWeekReward reward)
	{
		onSetRewardList(reward.items);
		switch (reward.rank)
		{
		case 1:
			m_window->SetBackImage("set:blockcity_charm_rank.json image:first_back");
			m_icon->SetVisible(true);
			m_icon->SetImage("set:blockcity_charm_rank.json image:cup_first");
			m_ranks->SetArea(UDim(0.0390625f, 70.f), UDim(0, 0), UDim(0, 100.f), UDim(1.f, 0));
			m_ranks->SetText("1");
			break;
		case 2:
			m_window->SetBackImage("set:blockcity_charm_rank.json image:second_back");
			m_icon->SetVisible(true);
			m_icon->SetImage("set:blockcity_charm_rank.json image:cup_second");
			m_ranks->SetArea(UDim(0.0390625f, 70.f), UDim(0, 0), UDim(0, 100.f), UDim(1.f, 0));
			m_ranks->SetText("2");
			break;
		case 3:
			m_window->SetBackImage("set:blockcity_charm_rank.json image:third_back");
			m_icon->SetVisible(true);
			m_icon->SetImage("set:blockcity_charm_rank.json image:cup_third");
			m_ranks->SetArea(UDim(0.0390625f, 70.f), UDim(0, 0), UDim(0, 100.f), UDim(1.f, 0));
			m_ranks->SetText("3");
			break;
		case 4:
			m_window->SetBackImage("set:blockcity_charm_rank.json image:other_back");
			m_icon->SetVisible(false);
			m_ranks->SetArea(UDim(0.0390625f, 0), UDim(0, 0), UDim(0, 100.f), UDim(1.f, 0));
			m_ranks->SetText("4-10");
			break;
		case 5:
			m_window->SetBackImage("set:blockcity_charm_rank.json image:other_back");
			m_icon->SetVisible(false);
			m_ranks->SetArea(UDim(0.0390625f, 0), UDim(0, 0), UDim(0, 100.f), UDim(1.f, 0));
			m_ranks->SetText("11-30");
			break;
		default:
			break;
		}
	}

	void GuiBlockCityCharmRankItem::onSetRewardList(std::vector<BlockCityWeekRewardItem> items)
	{
		static i32 count = 0;
		m_award->CleanupChildren();
		for (size_t i = 0; i < items.size(); i++)
		{
			String iterLayout = StringUtil::Format("BlockCityCharmRankItem-Award-Items-%d", count++).c_str();
			GUIStaticImage* itemView = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, iterLayout.c_str());
			itemView->SetArea(UDim(0, 91.f * (2-i)), UDim(0, 0), UDim(0, 91.f), UDim(1.f, 0));
			if (items[i].meta >= 0)
			{
				itemView->SetSize(UVector2(UDim(0, 67.f), UDim(0, 67.f)));
				itemView->SetVerticalAlignment(VA_CENTRE);
				GuiItemStack::showItemIcon(itemView,items[i].id, items[i].meta);
			}
			else
			{
				itemView->SetImage(BlockCitySetting::getItemIcon(items[i].id).c_str());
			}
			m_award->AddChildWindow(itemView);
		}
	}
}