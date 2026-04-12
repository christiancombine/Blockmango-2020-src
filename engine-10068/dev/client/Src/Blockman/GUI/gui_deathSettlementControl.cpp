#include "gui_deathSettlementControl.h"
#include "GuiDef.h"
#include "Util/ClientEvents.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "game.h"
#include "Util/LanguageKey.h"
#include "GUI/RootGuiLayout.h"
#include "ShellInterface.h"
#include "Util/LanguageManager.h"
#include "rapidjson/document.h"
#include "Network/ClientHttpRequest.h"
#include "Util/FriendManager.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN
{
	gui_deathSettlementControl::gui_deathSettlementControl()
		: gui_layout("DeathSettlement.json")
	{
	}
	gui_deathSettlementControl::~gui_deathSettlementControl()
	{
		m_subscriptionGuard.unsubscribeAll();
	}
	void gui_deathSettlementControl::onLoad()
	{

		head = getWindow<GuiUrlImage>("DeathSettlement-Head");
		head->setChildMaterial(GUIMaterialType_CULL_BACK);

		designation = getWindow<GUIStaticText>("DeathSettlement-Designation");
		name = getWindow<GUIStaticText>("DeathSettlement-Name");

		grade = getWindow<GUIStaticImage>("DeathSettlement-Grade");

		lifetimeTitle = getWindow<GUIStaticText>("DeathSettlement-Lifetime-Title");
		lifetimeTitle->SetText(getString(LanguageKey::GUI__DEATHSETTLEMENT_LIFETIME_TITLE));
		lifetimeText = getWindow<GUIStaticText>("DeathSettlement-Lifetime-Text");
		lifetimeScore = getWindow<GUIStaticText>("DeathSettlement-Lifetime-Score");

		killDeadTitle = getWindow<GUIStaticText>("DeathSettlement-KillDead-Title");
		killDeadTitle->SetText(getString(LanguageKey::GUI__DEATHSETTLEMENT_KILLDEAD_TITLE));
		killDeadText = getWindow<GUIStaticText>("DeathSettlement-KillDead-Text");
		killDeadScore = getWindow<GUIStaticText>("DeathSettlement-KillDead-Score");

		killBossTitle = getWindow<GUIStaticText>("DeathSettlement-KillBoss-Title");
		killBossTitle->SetText(getString(LanguageKey::GUI__DEATHSETTLEMENT_KILLBOSS_TITLE));
		killBossText = getWindow<GUIStaticText>("DeathSettlement-KillBoss-Text");
		killBossScore = getWindow<GUIStaticText>("DeathSettlement-KillBoss-Score");

		totalTitle = getWindow<GUIStaticText>("DeathSettlement-Total-Title");
		totalTitle->SetText(getString(LanguageKey::GUI__DEATHSETTLEMENT_TOTAL_TITLE));
		totalText = getWindow<GUIStaticText>("DeathSettlement-Total-Text");

		restart = getWindow<GUIButton>("DeathSettlement-Restart");
		restart->subscribeEvent(EventButtonClick, std::bind(&gui_deathSettlementControl::onRestartClick, this, std::placeholders::_1));
		restartText = getWindow<GUIStaticText>("DeathSettlement-Restart-Text");
		restartText->SetText(getString(LanguageKey::GUI_CONTINUE));

	
		exit = getWindow<GUIButton>("DeathSettlement-Exit");
		exit->subscribeEvent(EventButtonClick, std::bind(&gui_deathSettlementControl::onExitClick, this, std::placeholders::_1));
		exitText= getWindow<GUIStaticText>("DeathSettlement-Exit-Text");
		exitText->SetText(getString(LanguageKey::GUI_EXIT));

		m_subscriptionGuard.add(WalkingDeadDeathSettlementEvent::subscribe(std::bind(&gui_deathSettlementControl::onReceiveDeathSettlement, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UserActorChangeEvent::subscribe(std::bind(&gui_deathSettlementControl::onGetHeadPic, this, std::placeholders::_1)));
		head->setDefaultImage(Blockman::Instance()->m_pPlayer->m_sex <= 1 ? "set:walkingdead_death_settlement.json image:ranch_boy_icon" : "set:walkingdead_death_settlement.json image:ranch_girl_icon");
		ClientHttpRequest::loadUserInfo(GameClient::CGame::Instance()->getPlatformUserId());

	}

	void gui_deathSettlementControl::onGetCommonData(String key, String data)
	{
		if (key == "DeathSettlement")
		{
			WalkingDeadDeathSettlementEvent::emit(data);
		}
	}

	void gui_deathSettlementControl::onHide()
	{
		ClientNetwork::Instance()->getSender()->sendRebirth();
	}
	bool gui_deathSettlementControl::isNeedLoad()
	{
		return checkGameType(ClientGameType::WalkingDead);
	}
	void gui_deathSettlementControl::onUpdate(ui32 nTimeElapse)
	{
		/*if (userId != myUserId)
		{
			return;
		}
		auto firend = FriendManager::findUserById(userId)
			icon->SetUrl(firend->picUrl)
		*/
	}
	bool gui_deathSettlementControl::onGetHeadPic(i64 userId)
	{
		if (GameClient::CGame::Instance()->getPlatformUserId()!=userId)
		{
			return false;
		}
		auto myClient = FriendManager::findUserActor(userId);
		if (myClient != nullptr)
		{
			//head->setDefaultImage(myClient->sex == 1 ? "set:ranch_boy_icon.json image:ranch_boy_icon" : "set:ranch_girl_icon.json image:ranch_girl_icon");
			if (myClient->picUrl != "")
			{
				head->setUrl(myClient->picUrl);
			}
			
		}
		return true;
	}
	bool gui_deathSettlementControl::onExitClick(const EventArgs & events)
	{
		GameClient::CGame::Instance()->exitGame();
		//playSoundByType(ST_Click);
		return true;
	}

	bool gui_deathSettlementControl::onRestartClick(const EventArgs & events)
	{
		getParent()->showMainControl();
		//playSoundByType(ST_Click);
		return true;
	}

	void gui_deathSettlementControl::parseUpdateDeathSettlementa(const String & str)
	{
		if (str.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(str.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}
		if (!doc->HasMember("designation") || !doc->HasMember("name")||!doc->HasMember("grade")
			||!doc->HasMember("lifetimeText")||!doc->HasMember("lifetimeScore")||!doc->HasMember("killDeadText")
			||!doc->HasMember("killDeadScore")||!doc->HasMember("killBossText")||!doc->HasMember("killBossScore")
			||!doc->HasMember("totalText"))
		{
			LordDelete(doc);
			return;
		}
		m_deathStruct->designation = doc->FindMember("designation")->value.GetString();
		m_deathStruct->name = doc->FindMember("name")->value.GetString();
		m_deathStruct->grade = doc->FindMember("grade")->value.GetString();

		m_deathStruct->lifetimeText = doc->FindMember("lifetimeText")->value.GetString();
		m_deathStruct->lifetimeScore = doc->FindMember("lifetimeScore")->value.GetString();

		m_deathStruct->killDeadText = doc->FindMember("killDeadText")->value.GetString();
		m_deathStruct->killDeadScore = doc->FindMember("killDeadScore")->value.GetString();

		m_deathStruct->killBossText = doc->FindMember("killBossText")->value.GetString();
		m_deathStruct->killBossScore = doc->FindMember("killBossScore")->value.GetString();

		m_deathStruct->totalText = doc->FindMember("totalText")->value.GetString();

		LordDelete(doc);
	}
	bool gui_deathSettlementControl::onReceiveDeathSettlement(const String & result)
	{
		parseUpdateDeathSettlementa(result);

		//GUIStaticImage * head = nullptr;

		designation->SetText(LanguageManager::Instance()->getString(m_deathStruct->designation).c_str());
		name->SetText(m_deathStruct->name.c_str());

		//GUIStaticImage * grade = nullptr;
		if (m_deathStruct->grade.compare("A") == 0)
		{
			grade->SetImage("set:walkingdead_death_settlement.json image:A");
		}
		else if (m_deathStruct->grade.compare("B") == 0)
		{
			grade->SetImage("set:walkingdead_death_settlement.json image:B");
		}
		else if (m_deathStruct->grade.compare("C") == 0)
		{
			grade->SetImage("set:walkingdead_death_settlement.json image:C");
		}
		else if (m_deathStruct->grade.compare("D") == 0)
		{
			grade->SetImage("set:walkingdead_death_settlement.json image:D");
		}
		else if (m_deathStruct->grade.compare("E") == 0)
		{
			grade->SetImage("set:walkingdead_death_settlement.json image:E");
		}
		else if (m_deathStruct->grade.compare("S") == 0)
		{
			grade->SetImage("set:walkingdead_death_settlement.json image:S");
		}
		else if (m_deathStruct->grade.compare("SS") == 0)
		{
			grade->SetImage("set:walkingdead_death_settlement.json image:SS");
		}
		else if (m_deathStruct->grade.compare("SSS") == 0)
		{
			grade->SetImage("set:walkingdead_death_settlement.json image:SSS");
		}
		//grade->SetArea({ 0, 40.0f }, { 0, 30.0f + 80.0f*i }, { 1, -80.0f }, { 0, 50 });
		grade->SetVisible(false);
		lifetimeText->SetText(m_deathStruct->lifetimeText.c_str());
		lifetimeScore->SetText(m_deathStruct->lifetimeScore.c_str());

		killDeadText->SetText(m_deathStruct->killDeadText.c_str());
		killDeadScore->SetText(m_deathStruct->killDeadScore.c_str());


		killBossText->SetText(m_deathStruct->killBossText.c_str());
		killBossScore->SetText(m_deathStruct->killBossScore.c_str());

		totalText->SetText(m_deathStruct->totalText.c_str());

		getParent()->showWalkingDeadDeathSettlement();

		return true;

	}


}
