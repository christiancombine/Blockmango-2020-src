#include "GUISkyBlockRankItem.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "RootGuiLayout.h"
#include "game.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "UI/GUISystem.h"
#include "Util/FriendManager.h"

using namespace LORD;
namespace BLOCKMAN
{
	vector<GUIWindow*>::type GUISkyBlockRankItem::getPrivateChildren()
	{
		return { m_window };
	}

	GUISkyBlockRankItem::GUISkyBlockRankItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = GUIWindowManager::Instance()->LoadWindowFromJSON("SkyBlockRankItem.json");
		renameComponents(m_window);

		m_rankIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_rank = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_score = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		m_btn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4));
		m_btn->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockRankItem::onClick, this));


		m_btn->SetText(getString(LanguageKey::GUI_RANK_ACCESS));
		

		AddChildWindow(m_window);
	}

	void GUISkyBlockRankItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUISkyBlockRankItem::Destroy()
	{
		BaseClass::Destroy();
	}

	void GUISkyBlockRankItem::RefreshItem(SKyBlockRankData* rankData)
	{
		String rankIcon = "";
		switch (rankData->rank)
		{
		case  1:
			rankIcon = "set:skyblock_rank.json image:first";
			break;
		case  2:
			rankIcon = "set:skyblock_rank.json image:second";
			break;
		case  3:
			rankIcon = "set:skyblock_rank.json image:third";
			break;
		default:
			rankIcon = "set:skyblock_rank.json image:rank_num_bg";
			break;
		}
		m_userId = rankData->userId;
		m_rank->SetText(StringUtil::ToString(rankData->rank).c_str());
		m_name->SetText(rankData->name.c_str());
		m_score->SetText(StringUtil::ToString(rankData->score).c_str());
		m_rankIcon->SetImage(rankIcon.c_str());
	}

	bool GUISkyBlockRankItem::onClick()
	{
		ClientNetwork::Instance()->getSender()->sendCallOnManor(m_userId, 3, FriendManager::findFriendById(m_userId) ? true : false, 0);
		return true;
	}
}
