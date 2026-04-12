#include "gui_blockCityTigerLottery.h"
#include "rapidjson/document.h"
#include "Util/Random.h"
#include "Util/LanguageKey.h"
#include "Entity/EntitySessionNpc.h"
#include "game.h"
#include "GuiDef.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/BlockCitySetting.h"
#include "Network/protocol/CommonDataBuilder.h"

#include "Object/Root.h"
#include "Plugins/GameClientEvents.h"

namespace BLOCKMAN
{
	gui_blockCityTigerLottery::gui_blockCityTigerLottery() :
		gui_layout("BlockCityTigerLottery.json")
	{
	}

	gui_blockCityTigerLottery::~gui_blockCityTigerLottery()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_blockCityTigerLottery::onLoad()
	{
		m_item1_scroll = getWindow<GUILayout>("BlockCityTigerLottery-Item1-Scroll");
		m_item1_name = getWindow<GUIStaticText>("BlockCityTigerLottery-Item1-Name");
		m_item1_mask = getWindow<GUIStaticImage>("BlockCityTigerLottery-Item1-Mask");
		m_item1_lock = getWindow<GUIStaticImage>("BlockCityTigerLottery-Item1-Lock");
		m_item1_choose = getWindow<GUIButton>("BlockCityTigerLottery-Item1-Choose");
		m_item1_question_mark = getWindow<GUIStaticImage>("BlockCityTigerLottery-Item1-Question");

		m_item2_scroll = getWindow<GUILayout>("BlockCityTigerLottery-Item2-Scroll");
		m_item2_name = getWindow<GUIStaticText>("BlockCityTigerLottery-Item2-Name");
		m_item2_mask = getWindow<GUIStaticImage>("BlockCityTigerLottery-Item2-Mask");
		m_item2_lock = getWindow<GUIStaticImage>("BlockCityTigerLottery-Item2-Lock");
		m_item2_choose = getWindow<GUIButton>("BlockCityTigerLottery-Item2-Choose");
		m_item2_question_mark = getWindow<GUIStaticImage>("BlockCityTigerLottery-Item2-Question");

		m_item3_scroll = getWindow<GUILayout>("BlockCityTigerLottery-Item3-Scroll");
		m_item3_name = getWindow<GUIStaticText>("BlockCityTigerLottery-Item3-Name");
		m_item3_mask = getWindow<GUIStaticImage>("BlockCityTigerLottery-Item3-Mask");
		m_item3_lock = getWindow<GUIStaticImage>("BlockCityTigerLottery-Item3-Lock");
		m_item3_choose = getWindow<GUIButton>("BlockCityTigerLottery-Item3-Choose");
		m_item3_vip_icon = getWindow<GUIStaticImage>("BlockCityTigerLottery-Item3-Vip");
		m_item3_question_mark = getWindow<GUIStaticImage>("BlockCityTigerLottery-Item3-Question");

		m_lottery_begin = getWindow<GUIButton>("BlockCityTigerLottery-Begin");
		m_countdown = getWindow<GUIStaticText>("BlockCityTigerLottery-Countdown");

		m_up = getWindow<GUIStaticImage>("BlockCityTigerLottery-Up");
		m_down = getWindow<GUIStaticImage>("BlockCityTigerLottery-Down");
		m_up->SetVisible(false);
		m_down->SetVisible(false);

		m_subscriptionGuard.add(TigerLotteryResultEvent::subscribe(std::bind(&gui_blockCityTigerLottery::onLotteryResult, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(SetBlockCityLotteryByNpcEvent::subscribe(std::bind(&gui_blockCityTigerLottery::setLotteryDataByEntityId, this, std::placeholders::_1)));

		m_item1_choose->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTigerLottery::onSelectFirst, this, std::placeholders::_1));
		m_item2_choose->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTigerLottery::onSelectSecond, this, std::placeholders::_1));
		m_item3_choose->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTigerLottery::onSelectThird, this, std::placeholders::_1));
		m_lottery_begin->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTigerLottery::onLotteryBegin, this, std::placeholders::_1));

		//m_countdown->SetText(getString(LanguageKey::GUI_BLOCKCITY_TIGER).c_str());
		m_item1_choose->SetText(getString(LanguageKey::GUI_BLOCKCITY_TIGER_CHOOSE).c_str());
		m_item2_choose->SetText(getString(LanguageKey::GUI_BLOCKCITY_TIGER_CHOOSE).c_str());
		m_item3_choose->SetText(getString(LanguageKey::GUI_BLOCKCITY_TIGER_CHOOSE).c_str());
		getWindow<GUIStaticText>("BlockCityTigerLottery-Begin-Text")->SetText(getString(LanguageKey::GUI_BLOCKCITY_TIGER_BEGIN).c_str());


		auto btnProbability = getWindow<GUIButton>("BlockCityTigerLottery-Probability");
		if (Root::Instance()->isChina())
		{
			btnProbability->SetVisible(true);
			btnProbability->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityTigerLottery::onProbabilityClick, this, std::placeholders::_1));
		}
		else
		{
			btnProbability->SetVisible(false);
		}
		
	}

	bool gui_blockCityTigerLottery::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	void gui_blockCityTigerLottery::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
			return;

		bool isStartHasScroll = m_firstScroll || m_secondScroll || m_thirdScroll;

		if (m_firstScroll)
			m_firstScroll = onUpdateChild(m_firstId, m_firstThroughTimes, m_firstMoveSpeed, m_item1_childs, m_firstTopPosY, ViewId::FIRST);
		if (m_secondScroll)
			m_secondScroll = onUpdateChild(m_secondId, m_secondThroughTimes, m_secondMoveSpeed, m_item2_childs, m_secondTopPosY, ViewId::SECOND);
		if (m_thirdScroll)
			m_thirdScroll = onUpdateChild(m_thirdId, m_thirdThroughTimes, m_thirdMoveSpeed, m_item3_childs, m_thirdTopPosY, ViewId::THIRD);

		bool isEndHasScroll = m_firstScroll || m_secondScroll || m_thirdScroll;

		m_item1_name->SetVisible(!(m_firstScroll || m_firstLock));
		m_item2_name->SetVisible(!(m_secondScroll || m_secondLock));
		m_item3_name->SetVisible(!(m_thirdScroll || m_thirdLock));
		m_item1_scroll->SetVisible(!m_firstLock);
		m_item2_scroll->SetVisible(!m_secondLock);
		m_item3_scroll->SetVisible(!m_thirdLock);
		m_item1_question_mark->SetVisible(m_firstLock);
		m_item2_question_mark->SetVisible(m_secondLock);
		m_item3_question_mark->SetVisible(m_thirdLock);
		m_item1_choose->SetEnabled(!(m_firstLock || m_firstScroll));
		m_item2_choose->SetEnabled(!(m_secondLock || m_secondScroll));
		m_item3_choose->SetEnabled(!(m_thirdLock || m_thirdScroll) && Blockman::Instance()->m_pPlayer->m_vip > 0);
		m_item3_lock->SetVisible(!(m_thirdLock || m_thirdScroll) && Blockman::Instance()->m_pPlayer->m_vip == 0);
		m_item3_vip_icon->SetVisible(!(m_thirdLock || m_thirdScroll) && Blockman::Instance()->m_pPlayer->m_vip == 0);
		m_item3_mask->SetVisible(!(m_thirdLock || m_thirdScroll) && Blockman::Instance()->m_pPlayer->m_vip == 0);

		if (isStartHasScroll != isEndHasScroll)
		{
			m_countdownTime = m_countdownCD * 1000;
		}

		if (m_countdownTime > 0)
		{
			m_countdownTime -= nTimeElapse;
			if (m_countdownTime == 0)
				m_countdownTime--;
			if(m_countdownTime > 0)
				m_countdown->SetText(StringUtil::ToString(m_countdownTime / 1000).c_str());
		}
		else
		{
			m_countdown->SetText(getString(LanguageKey::GUI_BLOCKCITY_TIGER).c_str());
		}

		if (m_countdownTime < 0)
		{
			m_countdownTime = 0;
			onSelectFirst(EventArgs());
		}
	}

	bool gui_blockCityTigerLottery::onUpdateChild(String selectId, int& throughTimes, float& moveSpeed, map<String, GuiBlockCityTigerLotteryItem*>::type childs, float topPosY, ViewId viewId)
	{
		bool needSubSpeed = false;
		bool finded = false;
		float restoreY = 0.0f;
		int round = 40;
		for (auto iter = childs.begin(); iter != childs.end(); iter++)
		{
			auto child = iter->second;
			child->SetYPosition({ 0, child->GetYPosition().d_offset + moveSpeed });

			if (child->GetYPosition().d_offset > 337)
			{
				if (throughTimes >= round)
				{
					needSubSpeed = true;
				}
				child->SetYPosition({ 0, topPosY + (child->GetYPosition().d_offset - 337) });
				throughTimes++;
			}

			if (iter->first == selectId)
			{
				if (child->GetYPosition().d_offset > 59)
				{
					switch (viewId)
					{
					case BLOCKMAN::gui_blockCityTigerLottery::ViewId::FIRST: 
						restoreY = child->GetYPosition().d_offset - 59;
						finded = true;
						m_firstStop = true;
						break;
					case BLOCKMAN::gui_blockCityTigerLottery::ViewId::SECOND:
						if (m_firstStop)
						{
							restoreY = child->GetYPosition().d_offset - 59;
							finded = true;
							m_secondStop = true;
						}
						break;
					case BLOCKMAN::gui_blockCityTigerLottery::ViewId::THIRD:
						if (m_firstStop && m_secondStop)
						{
							restoreY = child->GetYPosition().d_offset - 59;
							finded = true;
						}
						break;
					default:
						break;
					}
				}
			}
			child->SetVisible(child->GetYPosition().d_offset >= -100);
		}

		if (finded)
		{
			moveSpeed = 0;
		}

		if (needSubSpeed && moveSpeed > 0)
		{
			moveSpeed -= m_subMoveSpeed;
			moveSpeed = Math::Max(40 * 0.2f, moveSpeed);
		}

		if (finded)
		{
			for (auto iter = childs.begin(); iter != childs.end(); iter++)
			{
				auto child = iter->second;
				child->SetYPosition({ 0, child->GetYPosition().d_offset - restoreY });
				if (child->GetYPosition().d_offset < topPosY)
				{
					child->SetYPosition({ 0, 337 - (topPosY - child->GetYPosition().d_offset) });
				}
			}
			return false;
		}
		else
		{
			return true;
		}
	}

	bool gui_blockCityTigerLottery::onLotteryData(const String & data)
	{
		m_items.clear();
		m_firstIds.clear();
		m_secondIds.clear();
		m_thirdIds.clear();

		if (data.length() == 0)
			return false;
		rapidjson::Document doc = rapidjson::Document();
		doc.Parse(data.c_str());

		if (doc.HasParseError())
		{
			LordLogError("The lottery data content HasParseError.");
			return false;
		}

		if (doc.HasMember("items"))
		{
			auto doc_items = doc.FindMember("items");
			if (doc_items->value.IsArray())
			{
				auto items = doc_items->value.GetArray();
				for (auto& item : items)
				{
					if (!item.HasMember("id") || !item.HasMember("type") || !item.HasMember("itemId") || !item.HasMember("itemIdMate"))
					{
						continue;
					}
					String id = item.FindMember("id")->value.GetString();
					int type = item.FindMember("type")->value.GetInt();
					int itemId = item.FindMember("itemId")->value.GetInt();
					int itemIdMate = item.FindMember("itemIdMate")->value.GetInt();
					m_items.insert({ id, CityLotteryItem(id, type, itemId, itemIdMate) });
				}
			}
		}

		if (doc.HasMember("firstIds"))
		{
			auto doc_firstIds = doc.FindMember("firstIds");
			if (doc_firstIds->value.IsArray())
			{
				auto firstIds = doc_firstIds->value.GetArray();
				for (auto& id : firstIds)
				{
					if (!id.IsString())
					{
						continue;
					}
					m_firstIds.push_back(id.GetString());
				}
			}
		}

		if (doc.HasMember("secondIds"))
		{
			auto doc_secondIds = doc.FindMember("secondIds");
			if (doc_secondIds->value.IsArray())
			{
				auto secondIds = doc_secondIds->value.GetArray();
				for (auto& id : secondIds)
				{
					if (!id.IsString())
					{
						continue;
					}
					m_secondIds.push_back(id.GetString());
				}
			}
		}

		if (doc.HasMember("thirdIds"))
		{
			auto doc_thirdIds = doc.FindMember("thirdIds");
			if (doc_thirdIds->value.IsArray())
			{
				auto thirdIds = doc_thirdIds->value.GetArray();
				for (auto& id : thirdIds)
				{
					if (!id.IsString())
					{
						continue;
					}
					m_thirdIds.push_back(id.GetString());
				}
			}
		}

		if (doc.HasMember("countdownCD"))
		{
			auto doc_countdownCD = doc.FindMember("countdownCD");
			m_countdownCD = doc_countdownCD->value.GetInt();
		}
		initAllChilds();
		return true;
	}

	void gui_blockCityTigerLottery::initAllChilds()
	{
		m_firstTopPosY = initItemChilds(m_item1_scroll, m_item1_childs, m_firstIds);
		m_secondTopPosY = initItemChilds(m_item2_scroll, m_item2_childs, m_secondIds);
		m_thirdTopPosY = initItemChilds(m_item3_scroll, m_item3_childs, m_thirdIds);
	}

	float gui_blockCityTigerLottery::initItemChilds(GUILayout* parent, map<String, GuiBlockCityTigerLotteryItem*>::type& childs, list<String>::type ids)
	{
		parent->CleanupChildren();
		childs.clear();
		static int index = 0;
		float positionY = 337;
		for (auto id : ids)
		{
			auto iter = m_items.find(id);
			if (iter == m_items.end())
				continue;
			index++;
			positionY -= 278;
			CityLotteryItem item = iter->second;
			String vName = StringUtil::Format("Lottery-Item-Child-%d", index);
			GuiBlockCityTigerLotteryItem* child = (GuiBlockCityTigerLotteryItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_TIGER_LOTTERY_ITEM, vName.c_str());
			child->SetTouchable(false);
			child->SetArea({ 0, 0 }, { 0, positionY }, { 0, 106.0f }, { 0, 110.0f });
			child->SetHorizontalAlignment(HA_CENTRE);
			if (item.m_type == 1)
			{
				child->setBlockLotteryItem(item.m_itemId, item.m_itemIdMate);
			}
			else
			{
				child->setLotteryItem(item.m_itemId);
			}
			child->SetVisible(positionY >= -100);
			child->setShowType(1);
			parent->AddChildWindow(child);
			childs.insert({ item.m_uniqueId, child });
		}
		return positionY;
	}

	bool gui_blockCityTigerLottery::onLotteryResult(const String& firstId, const String& secondId, const String& thirdId)
	{	
		m_firstId = firstId;
		m_secondId = secondId;
		m_thirdId = thirdId;
		
		m_firstThroughTimes = 0;
		m_secondThroughTimes = 0;
		m_thirdThroughTimes = 0;

		if (!m_items.empty())
		{
			m_subMoveSpeed = 100.0f / m_items.size() - 0.2f;
			if (m_subMoveSpeed <= 0.0f)
			{
				m_subMoveSpeed = 0.1f;
			}
		}

		m_firstMoveSpeed = 90 * m_subMoveSpeed;
		m_secondMoveSpeed = 90 * m_subMoveSpeed;
		m_thirdMoveSpeed = 90 * m_subMoveSpeed;

		m_firstScroll = m_firstMoveSpeed > m_subMoveSpeed;
		m_secondScroll = m_secondMoveSpeed > m_subMoveSpeed;
		m_thirdScroll = m_thirdMoveSpeed > m_subMoveSpeed;

		bool isFirstLock = m_items.find(m_firstId) == m_items.end();
		bool isSecondLock = m_items.find(m_secondId) == m_items.end();
		bool isThirdLock = m_items.find(m_thirdId) == m_items.end();

		m_firstScroll = m_firstScroll && !isFirstLock;
		m_secondScroll = m_secondScroll && !isSecondLock;
		m_thirdScroll = m_thirdScroll && !isThirdLock;

		m_up->SetVisible(!(m_firstScroll || m_secondScroll || m_thirdScroll));
		m_down->SetVisible(m_firstScroll || m_secondScroll || m_thirdScroll);

		m_firstLock = !m_firstScroll;
		m_secondLock = !m_secondScroll;
		m_thirdLock = !m_thirdScroll;

		setLotteryResultName(m_firstId, m_firstIds, m_item1_name);
		setLotteryResultName(m_secondId, m_secondIds, m_item2_name);
		setLotteryResultName(m_thirdId, m_thirdIds, m_item3_name);

		m_countdownTime = 0;
		m_countdown->SetText("");

		return true;
	}

	void gui_blockCityTigerLottery::setLotteryResultName(String uniqueId, list<String>::type ids, GUIStaticText* item)
	{
		auto iter = m_items.find(uniqueId);
		if (iter == m_items.end())
		{
			if (!ids.empty())
			{
				iter = m_items.find(*ids.begin());
				if (iter != m_items.end())
				{
					CityLotteryItem lotteryItem = iter->second;
					if (lotteryItem.m_type == 1)
					{
						ItemStackPtr itemStack = LORD::make_shared<ItemStack>(lotteryItem.m_itemId, 1, lotteryItem.m_itemIdMate);
						item->SetText(LanguageManager::Instance()->getItemName(itemStack->getItemName()).c_str());
					}
					else
					{
						item->SetText(getString(BlockCitySetting::getItemName(lotteryItem.m_itemId).c_str()));
					}
				}
				else
				{
					item->SetText("");
				}
			}
			else
			{
				item->SetText("");
			}
		}
		else
		{
			CityLotteryItem lotteryItem = iter->second;
			if (lotteryItem.m_type == 1)
			{
				ItemStackPtr itemStack = LORD::make_shared<ItemStack>(lotteryItem.m_itemId, 1, lotteryItem.m_itemIdMate);
				item->SetText(LanguageManager::Instance()->getItemName(itemStack->getItemName()).c_str());
			}
			else
			{
				item->SetText(getString(BlockCitySetting::getItemName(lotteryItem.m_itemId).c_str()));
			}
		}
	}

	bool gui_blockCityTigerLottery::onSelectFirst(const EventArgs & events)
	{
		if (m_firstId != "")
		{
			selectItem(m_firstId);
			getParent()->hideBlockCityTigerLottery();
		}
		return true;
	}

	bool gui_blockCityTigerLottery::onSelectSecond(const EventArgs & events)
	{
		if (m_secondId != "")
		{
			selectItem(m_secondId);
			getParent()->hideBlockCityTigerLottery();
		}
		return true;
	}

	bool gui_blockCityTigerLottery::onSelectThird(const EventArgs & events)
	{
		if (m_thirdId != "")
		{
			selectItem(m_thirdId);
			getParent()->hideBlockCityTigerLottery();
		}
		return true;
	}

	bool gui_blockCityTigerLottery::onLotteryBegin(const EventArgs & events)
	{
		if (m_firstScroll || m_secondScroll || m_thirdScroll)
			return false;
		playSoundByType(ST_tiger_machine);
		ClientNetwork::Instance()->getCommonSender()->sendBlockCityOpenTigerLottery(m_entityId);
		m_lottery_begin->SetEnabled(false);
		return true;
	}

	bool gui_blockCityTigerLottery::onProbabilityClick(const EventArgs& events)
	{
		SCRIPT_EVENT::CallPluginFunctionEvent::invoke("ProbabilityDialog", "g1047_1");
		return true;
	}

	void gui_blockCityTigerLottery::selectItem(String uniqueId)
	{
		if (m_items.find(uniqueId) != m_items.end())
			GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendBlockCitySelectTigerLottery(uniqueId);
		m_firstId = "";
		m_secondId = "";
		m_thirdId = "";
		m_firstScroll = false;
		m_secondScroll = false;
		m_thirdScroll = false;
		m_firstStop = false;
		m_secondStop = false;
	}

	bool gui_blockCityTigerLottery::setLotteryDataByEntityId(i32 entityId)
	{
		auto entity = dynamic_cast<EntitySessionNpc*>(Blockman::Instance()->m_pWorld->getEntity(entityId));
		if (!entity)
		{
			m_entityId = -1;
			return false;
		}
		m_entityId = entityId;

		onLotteryData(entity->getSessionContent());
		initLotteryUiByEntityId(entity->getActorBodyId() == "1");
		getParent()->showBlockCityTigerLottery();

		return true;
	}

	void gui_blockCityTigerLottery::initLotteryUiByEntityId(bool isVipTiger)
	{
		getWindow<GUILayout>("BlockCityTigerLottery")->SetBackImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_back" : "");
		getWindow<GUILayout>("BlockCityTigerLottery-Frame")->SetBackImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_content" :
			"set:blockcity_tiger_lottery.json image:general_content");
		getWindow<GUILayout>("BlockCityTigerLottery-Frame-Top")->SetBackImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_top" :
			"set:blockcity_tiger_lottery.json image:general_top");
		getWindow<GUILayout>("BlockCityTigerLottery-Frame-Title")->SetBackImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_title" :
			"set:blockcity_tiger_lottery.json image:general_title");
		getWindow<GUILayout>("BlockCityTigerLottery-Frame-Bottom")->SetBackImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_bottom" :
			"set:blockcity_tiger_lottery.json image:general_bottom");
		getWindow<GUILayout>("BlockCityTigerLottery-Frame-Left")->SetBackImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_left" :
			"set:blockcity_tiger_lottery.json image:general_left");
		getWindow<GUILayout>("BlockCityTigerLottery-Frame-Right")->SetBackImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_right" :
			"set:blockcity_tiger_lottery.json image:general_right");
		getWindow<GUIStaticImage>("BlockCityTigerLottery-Item1-Bg")->SetImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_content_left" :
			"set:blockcity_tiger_lottery.json image:general_content_left");
		getWindow<GUIStaticImage>("BlockCityTigerLottery-Item2-Bg")->SetImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_content_middle" :
			"set:blockcity_tiger_lottery.json image:general_content_middle");
		getWindow<GUIStaticImage>("BlockCityTigerLottery-Item3-Bg")->SetImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_content_right" :
			"set:blockcity_tiger_lottery.json image:general_content_right");
		m_up->SetImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_up" : "set:blockcity_tiger_lottery.json image:general_up");
		m_down->SetImage(isVipTiger ? "set:blockcity_tiger_lottery.json image:vip_down" : "set:blockcity_tiger_lottery.json image:general_down");
		m_countdownTime = 0;
		m_lottery_begin->SetEnabled(true);
		m_up->SetVisible(true);
		m_down->SetVisible(false);

		m_firstLock = true;
		m_item1_lock->SetVisible(false);
		m_item1_mask->SetVisible(false);

		m_secondLock = true;
		m_item2_lock->SetVisible(false);
		m_item2_mask->SetVisible(false);

		m_thirdLock = true;
		m_item3_lock->SetVisible(false);
		m_item3_vip_icon->SetVisible(false);
		m_item3_mask->SetVisible(false);
	}

	void gui_blockCityTigerLottery::onGetCommonData(String key, String data)
	{
		auto builder = CommonDataBuilder().fromData(data);
		i32 entityId = builder.getIntParam("entityId");
		setLotteryDataByEntityId(ClientNetwork::Instance()->getDataCache()->getClientId(entityId));
	}

}