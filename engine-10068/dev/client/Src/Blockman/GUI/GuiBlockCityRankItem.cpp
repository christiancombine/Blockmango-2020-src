#include "GuiBlockCityRankItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/UICommon.h"
#include "GUI/GuiItemStack.h"
#include "Util/ClientEvents.h"
#include "game.h"
#include "Network/ClientHttpRequest.h"
#include "ShellInterface.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "Audio/SoundSystem.h"
#include "Audio/SoundDef.h"

namespace BLOCKMAN
{
	
	void GuiBlockCityRankItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockCityRankItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockCityRankItem::GuiBlockCityRankItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockCityRankItem.json"));
		renameComponents(m_window);

		m_btn = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(4));
		m_ownInfo = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(5));
		m_btnAddFriend = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4)->GetChildByIndex(0));
		m_btnVisit = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4)->GetChildByIndex(1));
		m_rankNo = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_pic = dynamic_cast<GuiUrlImage*>(m_window->GetChildByIndex(1));
		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_score = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3)->GetChildByIndex(0));
		m_myRank = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5)->GetChildByIndex(1)->GetChildByIndex(0));
		m_window->GetChildByIndex(5)->GetChildByIndex(0)->SetText(getString("dead.summary.rank"));
		m_btnAddFriend->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityRankItem::onClick, this, std::placeholders::_1, ViewId::ADD_FRIEND));
		m_btnVisit->subscribeEvent(EventButtonClick, std::bind(&GuiBlockCityRankItem::onClick, this, std::placeholders::_1, ViewId::VISIT));

		m_pic->setDefaultImage("set:blockcity_rank.json image:head_icon");
		m_pic->setChildMaterial(GUIMaterialType_CULL_BACK);
		if(Root::Instance()->isEditorMode())
		{
			AddWindowToDrawList(*m_window);
		}
		else
		{
			AddChildWindow(m_window);
		}
	}

	void GuiBlockCityRankItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		if (Root::Instance()->isEditorMode())
		{
			m_window->SetPosition(getAbsolutePositionFromRoot());
		}
		BaseClass::UpdateSelf(nTimeElapse);

		if (m_addFriendTime > 0)
		{
			m_addFriendTime -= nTimeElapse;
			m_addFriendTime = m_addFriendTime < 0 ? 0 : m_addFriendTime;
		}
		m_btnAddFriend->SetEnabled(m_addFriendTime == 0);
	}

	
	void GuiBlockCityRankItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}


	bool GuiBlockCityRankItem::onClick(const EventArgs& args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::ADD_FRIEND:
			GameClient::CGame::Instance()->getShellInterface()->onFriendOperation(2, m_rank.userId);
			m_addFriendTime = 30 * 1000;
			break;
		case ViewId::VISIT:
			ClientNetwork::Instance()->getSender()->sendCallOnManor(m_rank.userId, 1, false, m_rank.score);
			break;
		}
		SoundSystem::Instance()->playEffectByType(ST_Click);
		return true;
	}

	void GuiBlockCityRankItem::setRank(const UserRank &rank, i32 type)
	{
		switch (type)
		{
		case RankType::GlobalRank:
			m_rank = rank;
			m_pic->setUrl(rank.picUrl);
			m_name->SetText(rank.userName.c_str());
			m_rankNo->SetText(StringUtil::ToString(rank.rank).c_str());
			m_score->SetText(StringUtil::ToString(rank.score).c_str());
			m_ownInfo->SetVisible(false);
			if (Blockman::Instance()->m_pPlayer->getPlatformUserId() == rank.userId)
			{
				m_btnAddFriend->SetVisible(false);
			}
			break;

		case RankType::FriendRank:
			m_rank = rank;
			m_pic->setUrl(rank.picUrl);
			m_name->SetText(rank.userName.c_str());
			m_rankNo->SetText(StringUtil::ToString(rank.rank).c_str());
			m_score->SetText(StringUtil::ToString(rank.score).c_str());
			m_ownInfo->SetVisible(false);
			m_btnAddFriend->SetVisible(false);
			break;

		case RankType::MyRank:
			m_rank = rank;
			m_pic->setUrl(rank.picUrl);
			m_name->SetText(rank.userName.c_str());
			m_myRank->SetText(StringUtil::ToString(rank.rank).c_str());
			m_score->SetText(StringUtil::ToString(rank.score).c_str());
			m_window->SetBackImage("set:blockcity_rank.json image:my_back");
			m_rankNo->SetVisible(false);
			m_btnAddFriend->SetVisible(false);
			m_btnVisit->SetVisible(false);
			break;
		}
	}
}