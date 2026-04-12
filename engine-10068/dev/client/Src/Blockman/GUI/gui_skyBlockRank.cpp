#include "gui_skyBlockRank.h"
#include "Util/LanguageKey.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "GUISkyBlockRankItem.h"


namespace BLOCKMAN
{
	gui_skyBlockRank::gui_skyBlockRank()
		: gui_layout("SkyBlockRank.json")
	{
		m_selfRank = NULL;
	}

	gui_skyBlockRank::~gui_skyBlockRank()
	{
		clearData();
	}

	void gui_skyBlockRank::onLoad()
	{
		getWindow<GUIButton>("SkyBlockRank-Title-BtnClose")->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockRank::onExitClick, this, std::placeholders::_1));

		getWindow<GUIStaticText>("SkyBlockRank-Title-Text")->SetText(getString(LanguageKey::GUI_RANK_TITLE));
		getWindow<GUIStaticText>("SkyBlockRank-Content-Title-SkyBlockRank")->SetText(getString(LanguageKey::GUI_RANK_RANKS));
		getWindow<GUIStaticText>("SkyBlockRank-Content-Title-Name")->SetText(getString(LanguageKey::GUI_RANK_NAME));
		getWindow<GUIStaticText>("SkyBlockRank-Content-Title-Score")->SetText(getString(LanguageKey::GUI_RANK_LEVEL));
		m_selfRankText = getWindow<GUIStaticText>("SkyBlockRank-Bottom-SelfRankText");
		m_selfNameText = getWindow<GUIStaticText>("SkyBlockRank-Bottom-SelfRankName");
		m_selfScoreText = getWindow<GUIStaticText>("SkyBlockRank-Bottom-SelfScore");

		m_rankListBox = getWindow<GUIListBox>("SkyBlockRank-Content-List");

		m_subscriptionGuard.add(updateSkyBlockRankDataEvent::subscribe(std::bind(&gui_skyBlockRank::receiveRankResult, this, std::placeholders::_1)));
		m_selfRank = NULL;
	}

	bool gui_skyBlockRank::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain);
	}

	bool gui_skyBlockRank::onExitClick(const EventArgs & events)
	{
		getParent()->hideRank();
		return true;
	}

	void gui_skyBlockRank::onShow()
	{
		RefreshUI();
	}

	void gui_skyBlockRank::RefreshUI()
	{
		refreshSelfRank();
		refreshContent(m_rankListBox, m_rankList);
	}

	void gui_skyBlockRank::refreshSelfRank()
	{
		int dayRank = 0;
		int dayScore = 0;
		String selfname = "";
		if (m_selfRank != NULL)
		{
			dayRank = m_selfRank->rank;
			dayScore = m_selfRank->score;
			selfname = m_selfRank->name;

		}

		m_selfScoreText->SetText(StringUtil::ToString(dayScore).c_str());
		m_selfRankText->SetText(StringUtil::ToString(dayRank).c_str());
		m_selfNameText->SetText(selfname.c_str());
	}

	void gui_skyBlockRank::refreshContent(GUIListBox* list, std::list<SKyBlockRankData*>& data)
	{
		if (list == NULL)
			return;

		list->ClearAllItem();
		list->SetItemHeight(65.0f);
		static int i = 1;
		for (auto iter = data.begin(); iter != data.end(); iter++, i++)
		{
			GUISkyBlockRankItem* pRankItem = dynamic_cast<GUISkyBlockRankItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_RANK_ITEM, GUIString("RankItem") + StringUtil::ToString(i).c_str()));
			pRankItem->RefreshItem(*iter);
			pRankItem->SetPosition(UVector2(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f)));
			pRankItem->SetSize(UVector2(UDim(1.0f, 0.0f), UDim(0.0f, 53.0f)));
			list->AddItem(pRankItem, true);
		}

		list->SetAllowScroll(true);
		list->SetTouchable(true);
	}

	bool gui_skyBlockRank::receiveRankResult(const String& rankResult)
	{
		bool bRet = false;
		clearData();
		bRet = parseRankResult(rankResult);
		return bRet;
	}

	SKyBlockRankData* gui_skyBlockRank::getRankData(rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>> &data)
	{
		if (!data.HasMember("rank") || !data.HasMember("score") ||
			!data.HasMember("name") || !data.HasMember("userId"))
		{
			LordLogError("The rank result content missed some field.");
			return nullptr;
		}

		String name = data.FindMember("name")->value.GetString();
		ui64 userId = data.FindMember("userId")->value.GetInt();
		int rank = data.FindMember("rank")->value.GetInt();
		int score = data.FindMember("score")->value.GetInt();

		return new SKyBlockRankData(name, userId, score, rank);
	}

	void gui_skyBlockRank::clearData()
	{
		if (m_selfRank != NULL)
		{
			delete m_selfRank;
			m_selfRank = NULL;
		}

		for (auto iter = m_rankList.begin(); iter != m_rankList.end(); iter++)
			delete *iter;
		m_rankList.clear();
	}

	bool gui_skyBlockRank::parseRankResult(const String& rankResult)
	{
		if (rankResult.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(rankResult.c_str());

		if (doc->HasParseError())
		{
			LordLogError("The rank result content  HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("own") || !doc->HasMember("ranks"))
		{
			LordLogError("The rank result content missed some field.");
			LordDelete(doc);
			return false;
		}

		auto ownData = doc->FindMember("own")->value.GetObject();
		m_selfRank = getRankData(ownData);
		if (m_selfRank == NULL)
		{
			LordDelete(doc);
			return false;
		}

		rapidjson::Value ranksData = doc->FindMember("ranks")->value.GetArray();
		for (size_t i = 0; i < ranksData.Size(); ++i)
		{
			auto obj = ranksData[i].GetObject();
			SKyBlockRankData* pData = getRankData(obj);
			if (pData == NULL)
			{
				LordDelete(doc);
				return false;
			}
			m_rankList.push_back(pData);
		}
		LordDelete(doc);
		return true;
	}
}
