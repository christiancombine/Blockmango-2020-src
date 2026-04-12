#include "GuideSystemSetting.h"
#include "Util/CSVReader.h"
#include "Util/ClientEvents.h"
#include "Object/Root.h"
#include "rapidjson/document.h"
#include "Network/ClientNetwork.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Effect/EffectSystemManager.h"
#include "game.h"
#include "cWorld/Blockman.h"

namespace BLOCKMAN
{
	map<int, ConditionItem*>::type GuideSystemSetting::m_condition_map = {};
	map<int, EventItem*>::type GuideSystemSetting::m_event_map = {};
	map<int, GuideItem*>::type GuideSystemSetting::m_guide_map = {};
	list<GuideItem*>::type GuideSystemSetting::m_under_way_guide_list = {};
	map<int, SimpleEffect*>::type GuideSystemSetting::m_world_effect_map = {};
	int GuideSystemSetting::m_max_lv = 0;

	void GuideSystemSetting::loadSetting()
	{
		loadConditionSetting();
		loadEventSetting();
		loadGuideSetting();
	}

	void GuideSystemSetting::unloadSetting()
	{
		for (auto iter : m_condition_map)
		{
			delete iter.second;
		}
		m_condition_map.clear();

		for (auto iter : m_event_map)
		{
			delete iter.second;
		}
		m_event_map.clear();

		for (auto iter : m_guide_map)
		{
			delete iter.second;
		}
		m_guide_map.clear();

		m_under_way_guide_list.clear();

		for (auto iter : m_world_effect_map)
		{
			delete iter.second;
		}
		m_world_effect_map.clear();
		
	}

	void GuideSystemSetting::loadConditionSetting()
	{
		ConditionItem* pCondition = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "ConditionSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pCondition = new ConditionItem();
			pReader->GetInt("Id", pCondition->Id);
			int Type = 0;
			pReader->GetInt("Type", Type);
			pCondition->Type = (ConditionType)Type;
			String Remark = "";
			pReader->GetString("Remark", Remark);
			if (Remark != "#")
			{
				pCondition->Remark = Remark;
			}
			String PosStart = "";
			pReader->GetString("PosStart", PosStart);
			if (PosStart != "#")
			{
				pCondition->PosStart = PosStart;
			}
			String PosEnd = "";
			pReader->GetString("PosEnd", PosEnd);
			if (PosEnd != "#")
			{
				pCondition->PosEnd = PosEnd;
			}
			m_condition_map[pCondition->Id] = pCondition;
		}
		
		LordSafeDelete(pReader);
	}

	void GuideSystemSetting::loadEventSetting()
	{
		EventItem* pEvent = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "EventSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			pEvent = new EventItem();
			pReader->GetInt("Id", pEvent->Id);
			int Type = 0;
			pReader->GetInt("Type", Type);
			pEvent->Type = (EventType)Type;
			String Remark = "";
			pReader->GetString("Remark", Remark);
			if (Remark != "#")
			{
				pEvent->Remark = Remark;
			}
			String Pos = "";
			pReader->GetString("Pos", Pos);
			if (Pos != "#")
			{
				pEvent->Pos = Pos;
			}
			m_event_map[pEvent->Id] = pEvent;
		}
		
		LordSafeDelete(pReader);
	}

	void GuideSystemSetting::loadGuideSetting()
	{
		GuideItem* pGuide = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "GuideSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		LordLogDebug("GuideSystemSetting::loadGuideSetting()");
		bool isIndie = GameClient::CGame::Instance()->isEnableIndie();
		while (pReader->Read())
		{
			int IndieType = 0;
			pReader->GetInt("IndieType", IndieType);
			if (((Indie)IndieType == INDIE_INDIE && !isIndie) || ((Indie)IndieType == INDIE_PIXEL && isIndie))
			{
				continue;
			}

			pGuide = new GuideItem();
			pReader->GetInt("Id", pGuide->Id);
			int IsCompel = 0;
			pReader->GetInt("IsCompel", IsCompel);
			if (IsCompel == 1)
			{
				pGuide->IsCompel = true; 
			}
			pReader->GetInt("ResetId", pGuide->ResetId);
			pReader->GetInt("SkipId", pGuide->SkipId);
			int TriggerId = 0;
			pReader->GetInt("TriggerId", TriggerId);
			ConditionItem* Trigger = findConditionById(TriggerId);
			pGuide->Trigger = Trigger ? Trigger : pGuide->Trigger;

			int EventId = 0;
			pReader->GetInt("EventId", EventId);
			EventItem* Event = findEventById(EventId);
			pGuide->Event = Event ? Event : pGuide->Event;

			int CompleteId = 0;
			pReader->GetInt("CompleteId", CompleteId);
			ConditionItem* Complete = findConditionById(CompleteId);
			pGuide->Complete = Complete ? Complete : pGuide->Complete;
			pReader->GetInt("Lv", pGuide->Lv);

			pGuide->Status = STATUS_LOCKED;
			m_guide_map[pGuide->Id] = pGuide;
			
			m_max_lv = m_max_lv < pGuide->Lv ? pGuide->Lv : m_max_lv;
		}
		
		LordSafeDelete(pReader);
	}

	ConditionItem * GuideSystemSetting::findConditionById(int id)
	{
		auto iter = m_condition_map.find(id);
		if (iter != m_condition_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	EventItem * GuideSystemSetting::findEventById(int id)
	{
		auto iter = m_event_map.find(id);
		if (iter != m_event_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	GuideItem * GuideSystemSetting::findGuideById(int id)
	{
		auto iter = m_guide_map.find(id);
		if (iter != m_guide_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	void GuideSystemSetting::parseGuideData(const String & data)
	{
		if (data.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}

		if (doc->HasMember("done"))
		{
			auto done = doc->FindMember("done")->value.GetArray();
			if (!done.Empty())
			{
				for (size_t i = 0; i < done.Size(); ++i)
				{
					auto obj = done[i].GetObject();
					if (!obj.HasMember("Id"))
						continue;
					int Id = obj.FindMember("Id")->value.GetInt();
					updateGuideStatus(Id, STATUS_DONE);
				}
			}
		}

		bool is_have_lv_guide = false;
		if (doc->HasMember("under_way"))
		{
			auto under_way = doc->FindMember("under_way")->value.GetArray();
			if (!under_way.Empty())
			{
				for (size_t i = 0; i < under_way.Size(); ++i)
				{
					auto obj = under_way[i].GetObject();
					if (!obj.HasMember("Id"))
						continue;
					int Id = obj.FindMember("Id")->value.GetInt();
					GuideItem* guide = findGuideById(Id);
					if (!guide)
					{
						continue;
					}
					int status = obj.FindMember("Status")->value.GetInt();
					guide->Status = (GuideStatus)status ? (GuideStatus)status : guide->Status;
					if (guide->Status == STATUS_ACTIVATED)
					{
						onGuideTriggered(guide);
						updateUnderWayGuideList(guide);
					}
					is_have_lv_guide = guide->Lv > 0 ? true : is_have_lv_guide;
				}
			}
		}

		int lv = doc->HasMember("lv") ? doc->FindMember("lv")->value.GetInt() : 0;
		int cur_lv = (lv <= m_max_lv || is_have_lv_guide) ? lv : 0;
		unlockGuides(cur_lv);

		if (doc->HasMember("done"))
			onEnterGame();

		LordDelete(doc);
	}

	void GuideSystemSetting::setGuideStatus(int id, GuideStatus status)
	{
		auto iter = m_guide_map.find(id);
		if (iter != m_guide_map.end())
		{
			iter->second->Status = status;
			if (iter->second->Status != STATUS_DONE)
			{
				iter->second->Status = STATUS_ACTIVATED;
				if (iter->second->Complete && iter->second->Complete->Type == CONDITION_CLICK)
				{
					setClickConditon(iter->second);
				}
			}
			onGuideStatusChange(iter->second);
		}
	}

	void GuideSystemSetting::updateGuideStatus(int id, GuideStatus status)
	{
		auto iter = m_guide_map.find(id);
		if (iter != m_guide_map.end())
		{
			iter->second->Status = status;
		}
	}

	void GuideSystemSetting::updateUnderWayGuideList(GuideItem * guide)
	{
		bool hasFind = false;
		for (auto iter = m_under_way_guide_list.begin(); iter != m_under_way_guide_list.end();)
		{
			auto pGuide = *iter;
			if (pGuide->Id == guide->Id)
			{
				hasFind = true;
				break;
			}
			else
			{
				iter++;
			}
		}
		if (!hasFind)
		{
			m_under_way_guide_list.push_back(guide);
		}
	}

	void GuideSystemSetting::unlockGuides(int cur_lv)
	{
		for (auto iter : m_guide_map)
		{
			auto guide = iter.second;
			if (guide->Status == STATUS_LOCKED && (guide->Lv == cur_lv || guide->Lv == 0))
			{
				guide->Status = STATUS_NOT_TRIGGER;
			}
		}
	}

	void GuideSystemSetting::sendGuideToServer(GuideItem * guide)
	{
		auto Status = (int)guide->Status;
		ClientNetwork::Instance()->getSender()->sendGuideInfo(guide->Id, Status);
	}

	void GuideSystemSetting::onGuideStatusChange(GuideItem * guide)
	{
		sendGuideToServer(guide);
		String id = "";
		switch (guide->Status)
		{
		case BLOCKMAN::STATUS_NOT_TRIGGER:
			break;
		case BLOCKMAN::STATUS_ACTIVATED:
			updateUnderWayGuideList(guide);
			break;
		case BLOCKMAN::STATUS_DONE:
			id = StringUtil::ToString(guide->Id);
			onGuideCompleted(id);
			m_under_way_guide_list.remove(guide);
			removeWorldEffect(guide->Id);
			break;
		default:
			break;
		}
	}

	void GuideSystemSetting::onGuideTriggered(GuideItem * guide)
	{
		EventItem* Event = guide->Event;
		if (!Event)
		{
			return;
		}
		StringArray posArray = StringUtil::Split(Event->Pos, "#");
		SimpleEffect* pEffect = NULL;
		switch (Event->Type)
		{
		case BLOCKMAN::EVENT_POP_UI:
			ShowWindowByNameEvent::emit(Event->Remark);
			guide->Status = STATUS_DONE;
			break;
		case BLOCKMAN::EVENT_TRANSFER:
			guide->Status = STATUS_DONE;
			break;
		case BLOCKMAN::EVENT_SCREEN_EFFECT:
			SetGuidePanelShowEvent::emit(true);
			ShowGuideEffectEvent::emit(Event->Remark, posArray);
			break;
		case BLOCKMAN::EVENT_GUIDE_LINK:
 			Blockman::Instance()->m_pPlayer->addGuideArrow(Vector3(StringUtil::ParseFloat(posArray[0]), StringUtil::ParseFloat(posArray[1]), StringUtil::ParseFloat(posArray[2])));
			break;
		case BLOCKMAN::EVENT_WORLD_EFFECT:
			pEffect = WorldEffectManager::Instance()->addSimpleEffect(Event->Remark, Vector3(StringUtil::ParseFloat(posArray[0]), StringUtil::ParseFloat(posArray[1]), StringUtil::ParseFloat(posArray[2])),
				StringUtil::ParseFloat(posArray[3]), -1, 1.f, 1.f, 1.f);
			m_world_effect_map[guide->Id] = pEffect;
			break;
		case BLOCKMAN::EVENT_SHOW_TIP:
		{
			StringArray tipArray = StringUtil::Split(Event->Remark, "#");
			vector<String>::type tips = vector<String>::type();
			for (auto& tip : tipArray)
			{
				tips.push_back(tip);
			}
			ShowCommonTipEvent::emit(tips, guide->Id);
		}
			break;
		case BLOCKMAN::EVENT_CHOOSE_PAPER:
		{
			StringArray itemIds = StringUtil::Split(Event->Remark, "#");
			vector<i32>::type items = vector<i32>::type();
			for (auto& id : itemIds)
			{
				items.push_back(StringUtil::ParseI32(id));
			}
			BlockCityShowChooseItemEvent::emit(guide->Id, items);
		}
			break;
		default:
			break;
		}
		if (guide->Status != STATUS_DONE)
		{
			guide->Status = STATUS_ACTIVATED;
			if (guide->Complete && guide->Complete->Type == CONDITION_CLICK)
			{
				setClickConditon(guide);
			}
		}
		onGuideStatusChange(guide);
	}

	void GuideSystemSetting::onGuideTriggered(int guideId)
	{
		GuideItem* guide = findGuideById(guideId);
		if (!guide)
			return;

		if (guide->Status != STATUS_NOT_TRIGGER)
			return;
		guide->Status = STATUS_ACTIVATED;
		onGuideStatusChange(guide);
		onGuideTriggered(guide);
	}

	void GuideSystemSetting::onGuideCompleted(String remark)
	{
		for (auto iter : m_guide_map)
		{
			auto guide = iter.second;
			auto Trigger = guide->Trigger;
			if (!Trigger)
			{
				continue;
			}
			if (Trigger->Type == CONDITION_COMPLETE_GUIDE)
			{
				if (Trigger->Remark == remark)
				{
					onGuideTriggered(guide);
				}
			}
		}
	}

	void GuideSystemSetting::onPlayerMove(Vector3 pos)
	{
		for (auto iter : m_guide_map)
		{
			auto guide = iter.second;
			auto Trigger = guide->Trigger;
			if (guide->Status != STATUS_NOT_TRIGGER || !Trigger)
			{
				continue;
			}
			if (Trigger->Type == CONDITION_ARRIVE_AREA)
			{
				StringArray posStart = StringUtil::Split(Trigger->PosStart, "#");
				StringArray posEnd = StringUtil::Split(Trigger->PosEnd, "#");
				Vector3 start = Vector3(StringUtil::ParseFloat(posStart[0]), StringUtil::ParseFloat(posStart[1]), StringUtil::ParseFloat(posStart[2]));
				Vector3 end = Vector3(StringUtil::ParseFloat(posEnd[0]), StringUtil::ParseFloat(posEnd[1]), StringUtil::ParseFloat(posEnd[2]));
				if (isInArea(pos, start, end))
				{
					onGuideTriggered(guide);
				}
			}
		}

		for (auto iter = m_under_way_guide_list.begin(); iter != m_under_way_guide_list.end(); iter++)
		{
			auto guide = *iter;
			auto Complete = guide->Complete;
			if (!Complete)
			{
				continue;
			}
			if (Complete->Type == CONDITION_ARRIVE_AREA)
			{
				StringArray posStart = StringUtil::Split(Complete->PosStart, "#");
				StringArray posEnd = StringUtil::Split(Complete->PosEnd, "#");
				Vector3 start = Vector3(StringUtil::ParseFloat(posStart[0]), StringUtil::ParseFloat(posStart[1]), StringUtil::ParseFloat(posStart[2]));
				Vector3 end = Vector3(StringUtil::ParseFloat(posEnd[0]), StringUtil::ParseFloat(posEnd[1]), StringUtil::ParseFloat(posEnd[2]));
				if (isInArea(pos, start, end))
				{
					onGuideDone(guide->Id);
					break;
				}
			}
		}
	}

	bool GuideSystemSetting::isInArea(Vector3 pos, Vector3 startPos, Vector3 endPos)
	{
		if (pos.x >= startPos.x && pos.x <= endPos.x
			&& pos.y >= startPos.y && pos.y <= endPos.y
			&& pos.z >= startPos.z && pos.z <= endPos.z)
		{
			return true;
		}
		return false;
	}

	void GuideSystemSetting::onEnterGame()
	{
 		for (auto iter : m_guide_map)
		{
			auto guide = iter.second;
			if (guide->Status != STATUS_NOT_TRIGGER || !guide->Trigger)
			{
				continue;
			}
			auto Trigger = guide->Trigger;
			if (Trigger->Type == CONDITION_ENTER_GAME)
			{
				onGuideTriggered(guide);
			}
		}
	}

	void GuideSystemSetting::removeWorldEffect(int id)
	{
		auto iter = m_world_effect_map.find(id);
		auto guide = findGuideById(id);
		if (iter != m_world_effect_map.end())
		{
			iter->second->mDuration = 0;
		}
		if (!guide || !guide->Event)
		{
			return;
		}
		if (guide->Event->Type == EVENT_GUIDE_LINK)
		{
			StringArray posArray = StringUtil::Split(guide->Event->Pos, "#");
			Blockman::Instance()->m_pPlayer->deleteGuideArrow(Vector3(StringUtil::ParseFloat(posArray[0]), StringUtil::ParseFloat(posArray[1]), StringUtil::ParseFloat(posArray[2])));
		}
	}

	void GuideSystemSetting::setClickConditon(GuideItem * guide)
	{
		auto Complete = guide->Complete;
		if (!Complete)
		{
			return;
		}

		UVector2 uv = UVector2();
		if (StringUtil::Find(Complete->PosStart, "#"))
		{
			StringArray posString = StringUtil::Split(Complete->PosStart, "#");
			float x = StringUtil::ParseFloat(posString[0]);
			float y = StringUtil::ParseFloat(posString[1]);
			uv = { {x, 0}, {y, 0} };
		}
		else
		{
			uv = StringUtil::ParseUVector2(Complete->PosStart);
		}
		
		int skipId = guide->SkipId > 0 ? guide->SkipId : 0;
		int buttonSize = StringUtil::ParseInt(Complete->Remark);
		SetGuidePanelShowEvent::emit(true);
		ShowGuideInfoEvent::emit(guide->IsCompel, skipId, uv, guide->Id, buttonSize);
	}

	void GuideSystemSetting::onPlayerSkip(int skipId, int guideId)
	{
		if (skipId > 0)
		{
			auto skip = findGuideById(skipId);
			onGuideTriggered(skip);
			auto guide = findGuideById(guideId);
			m_under_way_guide_list.remove(guide);
		}
		else
		{
			onGuideDone(guideId);
		}
		
	}

	void GuideSystemSetting::onGuideDone(int guideId)
	{
		auto guide = findGuideById(guideId);
		if (!guide)
		{
			return;
		}
		if (guide->Status != STATUS_ACTIVATED)
			return;
		guide->Status = STATUS_DONE;
		onGuideStatusChange(guide);
	}

	void GuideSystemSetting::onWindowShown(String name)
	{
		for (auto iter : m_guide_map)
		{
			auto guide = iter.second;
			auto Trigger = guide->Trigger;
			if (guide->Status != STATUS_NOT_TRIGGER || !Trigger)
			{
				continue;
			}
			if (Trigger->Type == CONDITION_UI_POP)
			{
				if (Trigger->Remark == name)
				{
					onGuideTriggered(guide);
				}
			}
		}

		for (auto iter = m_under_way_guide_list.begin(); iter != m_under_way_guide_list.end();iter++)
		{
			auto guide = *iter;
			auto Complete = guide->Complete;
			if (!Complete)
			{
				continue;
			}
			if (Complete->Type == CONDITION_UI_POP)
			{
				if (Complete->Remark == name)
				{
					onGuideDone(guide->Id);
					break;
				}
				
			}
		}
	}
	
}


