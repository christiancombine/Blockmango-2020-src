#include "gui_playerInfo.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "Block/BM_Material.h"
#include "Util/StringUtil.h"
#include "Util/ClientEvents.h"
#include "Item/Potion.h"
#include <list>
#include <iostream>
#include <string.h>
#include <math.h>
#include "game.h"
#include "Util/UICommon.h"
#include "Game/BirdSimulator.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "UI/GUIFontManager.h"
#include "Setting/CustomArmorSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	map<int, buffItem>::type gui_playerInfo::m_mapWalkingDeathBuff = {};
	gui_playerInfo::gui_playerInfo() :
		gui_layout("PlayerInfo.json"),
		m_oldAir(10),
		m_airFps(0)
	{

	}

	gui_playerInfo::~gui_playerInfo()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_playerInfo::onLoad()
	{
		m_rootWindow->SetTouchable(false);
		
		m_airLayout = getWindow<GUILayout>("PlayerInfo-Air");
		m_armorLayout = getWindow<GUILayout>("PlayerInfo-Armor");
		m_buffLayout = getWindow<GUILayout>("PlayerInfo-Effect-Layout");
		m_heartBar = getWindow<GUILayout>("PlayerInfo-HeartBar");
		m_defenseLayout = getWindow<GUILayout>("PlayerInfo-Defense");
		m_attackLayout = getWindow<GUILayout>("PlayerInfo-Attack");
		m_appleLayout = getWindow<GUILayout>("PlayerInfo-Apple");
		m_waterLayout = getWindow<GUILayout>("PlayerInfo-Water");
		m_birdBuffList = getWindow<GUIListHorizontalBox>("PlayerInfo-BuffList");
		m_walkingDeathBuffLayout = getWindow<GUILayout>("PlayerInfo-Buff");
		
		m_airLayout->SetVisible(false);
		m_defenseLayout->SetVisible(false);
		m_attackLayout->SetVisible(false);
		m_appleLayout->SetVisible(false);
		m_waterLayout->SetVisible(false);

		m_health_value = getWindow<GUIProgressBar>("PlayerInfo-Health-Value");
		m_foodstat_value = getWindow<GUIProgressBar>("PlayerInfo-FoodSaturation-Value");
		m_defense_value = getWindow<GUIProgressBar>("PlayerInfo-Defense-Value");
		m_attack_value = getWindow<GUIProgressBar>("PlayerInfo-Attack-Value");
		m_apple_value = getWindow<GUIProgressBar>("PlayerInfo-Apple-Value");
		m_water_value = getWindow<GUIProgressBar>("PlayerInfo-Water-Value");

		m_health_number = getWindow<GUIStaticText>("PlayerInfo-Health-Number");
		m_foodstat_number = getWindow<GUIStaticText>("PlayerInfo-FoodSaturation-Number");
		m_defense_number = getWindow<GUIStaticText>("PlayerInfo-Defense-Number");
		m_attack_number = getWindow<GUIStaticText>("PlayerInfo-Attack-Number");
		m_apple_number = getWindow<GUIStaticText>("PlayerInfo-Apple-Number");
		m_water_number = getWindow<GUIStaticText>("PlayerInfo-Water-Number");

		if (checkGameType(ClientGameType::SkyBlockMain)
			|| checkGameType(ClientGameType::SkyBlockMineCave)
			|| checkGameType(ClientGameType::SkyBlockProduct))
		{
			getWindow<GUIStaticImage>("PlayerInfo-Health-Icon")->SetImage("set:skyblock_main.json image:skyblock_hp");
			getWindow<GUIStaticImage>("PlayerInfo-Health-Icon")->SetArea(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f), UDim(0.0f, 31.f), UDim(0, 30.f));
			getWindow<GUIStaticImage>("PlayerInfo-FoodSaturation-Icon")->SetImage("set:skyblock_main.json image:skyblock_hunger");
			getWindow<GUIStaticImage>("PlayerInfo-FoodSaturation-Icon")->SetArea(UDim(0.0f, 0.0f), UDim(0.0f, -6.0f), UDim(0.0f, 29.f), UDim(0, 34.f));
			getWindow<GUIProgressBar>("PlayerInfo-Health-Value")->SetArea(UDim(0.1356f, 0.0f), UDim(0.0f, 0.0f), UDim(0.883f, 0.f), UDim(0.8f, 0.f));
			getWindow<GUIProgressBar>("PlayerInfo-Health-Value")->SetBackImage("set:skyblock_main.json image:skyblock_hp_bg");
			getWindow<GUIProgressBar>("PlayerInfo-Health-Value")->SetProgressImage("set:skyblock_main.json image:skyblock_hp_progress");
			getWindow<GUIProgressBar>("PlayerInfo-FoodSaturation-Value")->SetArea(UDim(0.1356f, 0.0f), UDim(0.0f, 0.0f), UDim(0.883f, 0.f), UDim(0.8f, 0.f));
			getWindow<GUIProgressBar>("PlayerInfo-FoodSaturation-Value")->SetBackImage("set:skyblock_main.json image:skyblock_hp_bg");
			getWindow<GUIProgressBar>("PlayerInfo-FoodSaturation-Value")->SetProgressImage("set:skyblock_main.json image:skyblock_hunger_progress");
			getWindow<GUIProgressBar>("PlayerInfo-Health-Value")->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			getWindow<GUIProgressBar>("PlayerInfo-Health-Value")->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "10 10 10 10");
			getWindow<GUIProgressBar>("PlayerInfo-FoodSaturation-Value")->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
			getWindow<GUIProgressBar>("PlayerInfo-FoodSaturation-Value")->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "10 10 10 10");
		}
		else if (checkGameType(ClientGameType::BlockFort))
		{
			getWindow<GUIStaticImage>("PlayerInfo-Health-Icon")->SetImage("set:blockfort_main.json image:health");
			m_health_value->SetBackImage("set:blockfort_main.json image:pb_health_defense_bg");
			m_health_value->SetProgressImage("set:blockfort_main.json image:pb_health_red");
			m_health_value->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "10 10 10 10");
			m_defenseLayout->SetPosition(UVector2(UDim(0, 171.f), UDim(0, -95.f)));
			getWindow<GUIStaticImage>("PlayerInfo-Defense-Icon")->SetImage("set:blockfort_main.json image:defense");
			m_defense_value->SetBackImage("set:blockfort_main.json image:pb_health_defense_bg");
			m_defense_value->SetProgressImage("set:blockfort_main.json image:pb_defense_white");
			m_defense_value->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "10 10 10 10");
		}
		else
		{
			getWindow<GUIStaticImage>("PlayerInfo-Health-Icon")->SetImage("set:player_info_icon.json image:playerInfoFullHP");
			m_health_value->SetBackImage("set:other_icon.json image:player_info_healt_nor");
			m_health_value->SetProgressImage("set:other_icon.json image:player_info_healt_pre");
			m_health_value->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "2 2 2 2");
			m_defenseLayout->SetPosition(UVector2(UDim(0, -171.f), UDim(0, -125.f)));
			getWindow<GUIStaticImage>("PlayerInfo-Defense-Icon")->SetImage("set:player_info_icon.json image:playerInfoFullArmor");
			m_defense_value->SetBackImage("set:other_icon.json image:player_info_healt_nor");
			m_defense_value->SetProgressImage("set:new_gui_material.json image:bg_defense_progress");
			m_defense_value->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "2 2 2 2");
		}


		//EffectToxication EffectMoveSpeed EffectNightVision EffectJump EffectInvisibility EffectBackground
		m_listBuff.emplace_back(initBuffItem("PlayerInfo-Effect-Jump", "EffectJump"));
		m_listBuff.emplace_back(initBuffItem("PlayerInfo-Effect-Invisibility", "EffectInvisibility"));
		m_listBuff.emplace_back(initBuffItem("PlayerInfo-Effect-Night-Vision", "EffectNightVision"));
		m_listBuff.emplace_back(initBuffItem("PlayerInfo-Effect-Move-Speed", "EffectMoveSpeed"));
		m_listBuff.emplace_back(initBuffItem("PlayerInfo-Effect-Toxication", "EffectToxication"));
		m_listBuff.emplace_back(initBuffItem("PlayerInfo-Effect-Heal", "EffectHeal"));

		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Drowning", 4,"Drowning"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Fracture", 5,"Fracture"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Healing", 6,"Healing"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Hungry", 7,"Hungry"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Satiety", 9,"Satiety"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Thirst", 11,"Thirst"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-AddWater", 14,"AddWater"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Speed", 10,"Speed"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Bleed", 2,"Bleed"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Infection", 8,"Infection"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Poisoning", 12,"Poisoning"));
		m_listWalkingDeathBuff.emplace_back(initWDBuffItem("PlayerInfo-Buff-Fire", 13,"Fire"));

		list<GUIStaticImage*>::iterator image;
		for (image = m_listBuff.begin(); image != m_listBuff.end(); image++)
		{
			GUIStaticImage* item = *image;
			m_buffLayout->AddChildWindow(item);
			item->SetVisible(false);
			item->subscribeEvent(EventWindowTouchUp, std::bind(&gui_playerInfo::openEffectInfo, this, std::placeholders::_1));
		}

		for (image = m_listWalkingDeathBuff.begin(); image != m_listWalkingDeathBuff.end(); image++)
		{
			GUIStaticImage* item = *image;
			m_walkingDeathBuffLayout->AddChildWindow(item);
			item->SetVisible(false);
			item->subscribeEvent(EventWindowTouchUp, std::bind(&gui_playerInfo::openWalkingDeathBuffInfo, this, std::placeholders::_1));
		}

		for (int i = 1; i < 11; i++)
		{
			GUIStaticImage* airValue = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, StringUtil::Format("PlayerInfo-Air-Value-%d", i).c_str());
			airValue->SetTouchable(false);
			airValue->SetArea(UDim(1, -(24.0f + 26.0f * (i - 1))), UDim(0, 0), UDim(0, 24.0f), UDim(0, 24.0f));
			airValue->SetImage("set:player_info_icon.json image:playerInfoAirFull");
			m_listAir.push_back(airValue);
			m_airLayout->AddChildWindow(airValue);
		}

		for (int i = 1; i < 11; i++)
		{
			GUIStaticImage* armorValue = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, StringUtil::Format("PlayerInfo-Armor-Value-%d", i).c_str());
			armorValue->SetTouchable(false);
			armorValue->SetArea(UDim(0, (i - 1) * 26.0f), UDim(0, 0), UDim(0, 24.0f), UDim(0, 28.0f));
			armorValue->SetImage("set:player_info_icon.json image:playerInfoNullArmor");
			m_listArmor.push_back(armorValue);
			m_armorLayout->AddChildWindow(armorValue);
		}

		m_subscriptionGuard.add(ChangeHeartEvent::subscribe(std::bind(&gui_playerInfo::updateHeartBar, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ChangeDefenseEvent::subscribe(std::bind(&gui_playerInfo::updateDefenseUI, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ChangeAttackEvent::subscribe(std::bind(&gui_playerInfo::updateAttackUI, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ChangeAppleEvent::subscribe(std::bind(&gui_playerInfo::updateAppleUI, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BirdBuffChangeEvent::subscribe(std::bind(&gui_playerInfo::onBirdBuffChange, this)));
		m_subscriptionGuard.add(ShowAppleFreshTimeEvent::subscribe(std::bind(&gui_playerInfo::showAppleFresh, this, std::placeholders::_1)));
	}

	void gui_playerInfo::onGetCommonData(String key, String data)
	{
		if (key == "WalkingDeadBuffInfo")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int buffId = builder.getIntParam("type");
			int duration = builder.getIntParam("duration");
			changeWDBuffItem(buffId, duration);
			return;
		}

		if (key == "LuckSkySetEffectLimit")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int time = builder.getIntParam("time");
			setShowEffectLimit(time);
			return;
		}
	}

	bool gui_playerInfo::updateHeartBar(int hp, int maxHp)
	{
		if (!m_heartBar)
			return false;
		m_heartBar->CleanupChildren();
		if (maxHp <= 0)
			return false;
		float itemWidth = 30;
		float itemSpace = 10;
		float width = itemWidth * maxHp + (maxHp - 1) * itemSpace;
		float startX = (m_heartBar->GetWidth().d_offset - width) / 2;
		for (int i = 0; i < maxHp; i++)
		{
			GUIStaticImage* itemHeart = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, StringUtil::Format("PlayerInfo-Heart-Item-%d", i).c_str());
			itemHeart->SetTouchable(false);
			itemHeart->SetArea(UDim(0, startX + i * (itemWidth + itemSpace)), UDim(0, 0), UDim(0, itemWidth), UDim(0, itemWidth));
			if (i < hp)
				itemHeart->SetImage("set:player_info_icon.json image:playerInfoFullHP");
			else
				itemHeart->SetImage("set:player_info_icon.json image:playerInfoNullHP");
			m_heartBar->AddChildWindow(itemHeart);
		}
		return true;
	}

	bool gui_playerInfo::updateDefenseUI(int defense, int maxDefense)
	{
		if (maxDefense <= 0)
		{
			m_defenseLayout->SetVisible(false);
			return false;
		}


		m_defenseLayout->SetVisible(true);
		m_defense_value->SetProgress((float)defense / (float)maxDefense);
		m_defense_number->SetText(StringUtil::Format("%d/%d", defense, maxDefense).c_str());
		return true;
	}

	bool gui_playerInfo::updateAttackUI(int attack, int maxAttack)
	{
		if (maxAttack <= 0)
		{
			m_attackLayout->SetVisible(false);
			return false;
		}
		m_attackLayout->SetVisible(true);
		m_attack_value->SetProgress((float)attack / (float)maxAttack);
		m_attack_number->SetText(StringUtil::Format("%d/%d", attack, maxAttack).c_str());
		return true;
	}

	bool gui_playerInfo::updateAppleUI(int cur, int max)
	{
		if (max <= 0)
		{
			m_appleLayout->SetVisible(false);
			return false;
		}
		m_appleLayout->SetVisible(true);
		m_apple_value->SetProgress((float)cur / (float)max);
		m_apple_number->SetText(StringUtil::Format("%d/%d", cur, max).c_str());
		m_appleLayout->SetAlpha(1.0f);
		return true;
	}

	bool gui_playerInfo::onBirdBuffChange()
	{
		if (!UICommon::checkBirdParam())
		{
			m_birdBuffList->SetVisible(false);
			return false;
		}

		const auto buffs = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBuffs();
		m_birdBuffList->SetVisible(buffs.size() > 0);
		initBirdBuff(buffs);
		return true;
	}

	void gui_playerInfo::onUpdate(ui32 nTimeElapse)
	{
		updateEffectUI();
		updateHealthUI();
		updateFoodStatUI();
		updateArmorUI();
		updateAirUI();
		updateAppleFreshUI(nTimeElapse);
		updateWaterUI();
		updateBuffUI(nTimeElapse);
	}


	bool gui_playerInfo::openEffectInfo(const EventArgs & events)
	{
		return false;
	}

	bool gui_playerInfo::openWalkingDeathBuffInfo(const EventArgs & events)
	{
		return false;
	}

	void gui_playerInfo::updateEffectUI()
	{
		list<GUIStaticImage*>::iterator image;
		for (image = m_listBuff.begin(); image != m_listBuff.end(); image++)
		{
			GUIStaticImage* item = *image;
			item->SetVisible(false);
		}

		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		map<int, PotionEffect*>::type::iterator iter;

		int itemShowCount = 0;
		for (iter = player->getActivePotionEffects().begin(); iter != player->getActivePotionEffects().end(); iter++)
		{
			int id = iter->second->getPotionID();
			int duration = iter->second->getDuration() / 20;
			switch (id)
			{
			case 1:
				showBuffItem("PlayerInfo-Effect-Move-Speed-bg", itemShowCount, duration);
				break;
			case 6:
				showBuffItem("PlayerInfo-Effect-Heal-bg", itemShowCount, duration);
				break;
			case 8:
				showBuffItem("PlayerInfo-Effect-Jump-bg", itemShowCount, duration);
				break;
			case 14:
				showBuffItem("PlayerInfo-Effect-Invisibility-bg", itemShowCount, duration);
				break;
			case 16:
				//showBuffItem("PlayerInfo-Effect-Night-Vision-bg", itemShowCount, duration);
				break;
			case 19:
				showBuffItem("PlayerInfo-Effect-Toxication-bg", itemShowCount, duration);
				break;
			}
		}
	}
	void gui_playerInfo::updateBuffUI(ui32 nTimeElapse)
	{
		for (auto image = m_listWalkingDeathBuff.begin(); image != m_listWalkingDeathBuff.end(); ++image)
		{
			(*image)->SetVisible(false);
		}

		int itemShowCount = 0;
		for (auto buff = m_mapWalkingDeathBuff.begin(); buff != m_mapWalkingDeathBuff.end(); ++buff)
		{
			int name = buff->second.buffName;

			if (buff->second.duration > 0)
			{
				buff->second.lasttime += nTimeElapse;
				if (buff->second.duration < (int)buff->second.lasttime/1000)
				{
					changeWDBuffItem(buff->second.buffName, 0);
				}
			}
			if (buff->second.duration == 0)
			{
				continue;
			}

			int lastTime = (int)buff->second.lasttime / 1000;

			switch (name)
			{
			case 2:
				showWDBuffItem("PlayerInfo-Buff-Bleed-Icon", itemShowCount, (buff->second.duration- lastTime));
				break;
			case 4:
				showWDBuffItem("PlayerInfo-Buff-Drowning-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			case 5:
				showWDBuffItem("PlayerInfo-Buff-Fracture-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			case 6:
				showWDBuffItem("PlayerInfo-Buff-Healing-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			case 7:
				showWDBuffItem("PlayerInfo-Buff-Hungry-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			case 8:
				showWDBuffItem("PlayerInfo-Buff-Infection-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			case 9:
				showWDBuffItem("PlayerInfo-Buff-Satiety-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			case 10:
				showWDBuffItem("PlayerInfo-Buff-Speed-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			case 11:
				showWDBuffItem("PlayerInfo-Buff-Thirst-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			case 12:
				showWDBuffItem("PlayerInfo-Buff-Poisoning-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			case 13:
				showWDBuffItem("PlayerInfo-Buff-Fire-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			case 14:
				showWDBuffItem("PlayerInfo-Buff-AddWater-Icon", itemShowCount, (buff->second.duration - lastTime));
				break;
			}
		}
	}

	void gui_playerInfo::updateHealthUI()
	{
		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		float curHp = player->getHealth();
		float maxHp = player->getMaxHealth();
		m_health_value->SetProgress(curHp / maxHp);
		m_health_number->SetText(StringUtil::Format("%d/%d", (int)Math::Floor(curHp + 0.5), (int)Math::Floor(maxHp + 0.5)).c_str());
	}

	void gui_playerInfo::updateFoodStatUI()
	{
		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		int foodLevel = player->getFoodStats()->getFoodLevel();
		int prevFoodLevel = player->getFoodStats()->getPrevFoodLevel();
		int maxFoodLevel = player->getFoodStats()->getmaxFoodLevel();
		m_foodstat_value->SetProgress((float)foodLevel / (float)maxFoodLevel);
		m_foodstat_number->SetText(StringUtil::Format("%d/%d", foodLevel, maxFoodLevel).c_str());
	}

	void gui_playerInfo::updateWaterUI()
	{
		if (checkGameType(ClientGameType::WalkingDead))
		{
			m_waterLayout->SetVisible(true);
		}

		if (m_airLayout->IsVisible())
		{
			m_waterLayout->SetVisible(false);
			return;
		}

		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		int waterLevel = player->getFoodStats()->getWaterLevel();
		int maxWaterLevel = player->getFoodStats()->getmaxWaterLevel();
		m_water_value->SetProgress((float)waterLevel / (float)maxWaterLevel);
		m_water_number->SetText(StringUtil::Format("%d/%d", waterLevel, maxWaterLevel).c_str());
	}

	void gui_playerInfo::updateArmorUI()
	{
		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		std::string imageName = "";
		list<GUIStaticImage*>::iterator guiIter;
		GUIStaticImage* guiStaticImage = nullptr;
		float armorValue = player->getTotalArmorValue() / 2.0f;
		m_armorLayout->SetVisible(armorValue != 0);
		if (armorValue != 0)
		{
			int i = 1;
			for (guiIter = m_listArmor.begin(); guiIter != m_listArmor.end(); guiIter++)
			{
				guiStaticImage = *guiIter;
				if (armorValue >= i)
				{
					imageName = "set:player_info_icon.json image:playerInfoFullArmor";
				}
				else if (armorValue > i - 1)
				{
					imageName = "set:player_info_icon.json image:playerInfoHalfArmor";
				}
				else
				{
					imageName = "set:player_info_icon.json image:playerInfoNullArmor";
				}
				guiStaticImage->SetImage(imageName);
				i++;
			}
		}
	}

	void gui_playerInfo::updateAirUI()
	{
		if (m_attackLayout->IsVisible())
		{
			m_airLayout->SetVisible(false);
			return;
		}
		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		m_airLayout->SetVisible(player->isInsideOfMaterial(BM_Material::BM_MAT_water));
		list<GUIStaticImage*>::iterator guiIter;
		GUIStaticImage* guiStaticImage = nullptr;
		int i = 1;
		if (player->isInsideOfMaterial(BM_Material::BM_MAT_water))
		{
			i = 1;
			float air = ((float)player->getAir()) / 30.f;
			air = (air > 0.0f) ? floor(air + 0.5f) : ceil(air - 0.5f);

			if ((int)air != m_oldAir && m_airFps < 3)
			{
				for (guiIter = m_listAir.begin(); guiIter != m_listAir.end(); guiIter++) {
					guiStaticImage = *guiIter;
					guiStaticImage->SetVisible(i <= m_oldAir);
					guiStaticImage->SetImage(i == m_oldAir ? "set:player_info_icon.json image:playerInfoAirRupture" : "set:player_info_icon.json image:playerInfoAirFull");
					i++;
				}
				m_airFps++;
			}
			else
			{
				m_airFps = 0;
				m_oldAir = (int)air;
				for (guiIter = m_listAir.begin(); guiIter != m_listAir.end(); guiIter++) {
					guiStaticImage = *guiIter;
					guiStaticImage->SetImage("set:player_info_icon.json image:playerInfoAirFull");
					guiStaticImage->SetVisible(i <= air);
					i++;
				}
			}
		}
	}

	void gui_playerInfo::updateAppleFreshUI(ui32 nTimeElapse)
	{
		if (m_appleFreshTime > 0)
		{
			m_appleFreshTime = m_appleFreshTime - nTimeElapse;

			float alpha = m_appleLayout->GetAlpha();
			float interval = nTimeElapse * 1.0f / 500;
			if (alpha >= 1.0f)
			{
				alpha -= interval;
			}
			else if (alpha <= 0.0f)
			{
				alpha = 1.0f;
			}
			else
			{
				alpha -= interval;
			}
			m_appleLayout->SetAlpha(alpha);
		}
		
	}

	bool gui_playerInfo::showAppleFresh(int time)
	{
		m_appleFreshTime = time * 1000;
		m_appleLayout->SetAlpha(1.0f);
		return true;
	}

	void gui_playerInfo::initBirdBuff(const vector<BirdBuff>::type& birdBuffs)
	{
		m_birdBuffList->ClearAllItem();
		static int count = 0;
		for (auto buff : birdBuffs)
		{
			String  viewName = StringUtil::Format("PlayerInfo-BirfBuff-Item-%d", count++);
			String  levelViewName = StringUtil::Format("PlayerInfo-BirfBuff-Level-Item-%d", count);
			GUIStaticText* buffLevelView = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, levelViewName.c_str());
			buffLevelView->SetSize(UVector2(UDim(1.f, 0.f), UDim(1.f, 0.f)));
			buffLevelView->SetTextVertAlign(VA_BOTTOM);
			buffLevelView->SetTextHorzAlign(HA_RIGHT);
			buffLevelView->SetFont(GUIFontManager::Instance()->GetFont("HT14"));
			buffLevelView->SetText(StringUtil::Format("x%d", buff.level).c_str());
			buffLevelView->SetTouchable(false);
			GUIStaticImage* buffView = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, viewName.c_str());
			buffView->SetPosition(UVector2(UDim(0, 0), UDim(0, 0)));
			buffView->SetSize(UVector2(UDim(0, 50.f), UDim(0, 50.f)));
			buffView->SetTouchable(false);
			buffView->SetImage(buff.icon.c_str());
			buffView->AddChildWindow(buffLevelView);
			m_birdBuffList->AddItem(buffView, false);
		}
	}

	void gui_playerInfo::showBuffItem(GUIString itemName, int& showCount, int time)
	{
		list<GUIStaticImage*>::iterator image;
		for (image = m_listBuff.begin(); image != m_listBuff.end(); image++)
		{
			GUIStaticImage* item = *image;
			if (item->GetName() == itemName)
			{
				item->SetVisible(true);
				item->SetArea(UDim(0, 0), UDim(0, 51.0f*showCount), UDim(0, 50.0f), UDim(0, 50.0f));

				if (time > 0 && time <= m_showEffectLimit)
				{
					String timeTxt = StringUtil::Format("%ds", time).c_str();
					item->GetChildByIndex(1)->SetText(timeTxt.c_str());
					item->SetVisible(true);
				}
				else
				{
					item->SetVisible(false);
				}
				showCount++;
				break;
			}
		}
	}

	void gui_playerInfo::showWDBuffItem(GUIString itemName, int& showCount, int time)
	{
		list<GUIStaticImage*>::iterator image;
		for (image = m_listWalkingDeathBuff.begin(); image != m_listWalkingDeathBuff.end(); image++)
		{
			GUIStaticImage* item = *image;
			if (item->GetName() == itemName)
			{
				item->SetVisible(true);
				item->SetArea(UDim(0.1f*showCount, 0.f), UDim(0.f, 0.f), UDim(0.1f, 0.f), UDim(1.0f, 0.f));
				if (time > 0)
				{
					String timeTxt = StringUtil::Format("%ds", time).c_str();
					item->GetChildByIndex(0)->SetText(timeTxt.c_str());
					item->SetVisible(true);
				}
				showCount++;
				break;
			}
		}
	}

	GUIStaticImage* gui_playerInfo::initBuffItem(String itemName, String itemRes)
	{
		GUIStaticImage* itemBg = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, (itemName + "-bg").c_str());
		itemBg->SetArea(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f), UDim(0.0f, 50.0f), UDim(0.0f, 50.0f));
		itemBg->SetImage("set:inventory.json image:EffectBackground");
		itemBg->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
		itemBg->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "4 4 4 4");
		GUIStaticImage* itemIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, (itemName + "-Icon").c_str());
		itemIcon->SetImage(("set:inventory.json image:" + itemRes).c_str());
		itemBg->AddChildWindow(itemIcon);
		itemIcon->SetArea(UDim(0, 2.5f), UDim(0, 2.5f), UDim(1.0f, -5.0f), UDim(1, -5.0f));
		GUIStaticText* buffTime = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, (itemName + "-Time").c_str());
		buffTime->SetArea(UDim(-1.0f, 0.f), UDim(0.f, 0.f), UDim(1.0f, 0.f), UDim(1.0f, 0.f));
		buffTime->SetTextVertAlign(VA_CENTRE);
		buffTime->SetTextHorzAlign(HA_CENTRE);
		buffTime->SetFont(GUIFontManager::Instance()->GetFont("HT14"));
		buffTime->SetText("");
		buffTime->SetTouchable(false);
		itemBg->AddChildWindow(buffTime);
		return itemBg;
	}

	GUIStaticImage* gui_playerInfo::initWDBuffItem(String itemName, int buffId, String itemRes)
	{
		GUIStaticImage* itemIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, (itemName + "-Icon").c_str());
		itemIcon->SetImage(("set:walkingdead_buffs.json image:" + itemRes).c_str());
		itemIcon->SetArea(UDim(0, 0.f), UDim(0, 0.f), UDim(1, 0.f), UDim(1, 0.f));
		itemIcon->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
		itemIcon->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "4 4 4 4");
		GUIStaticText* buffTime = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, (itemName + "-Time").c_str());
		buffTime->SetArea(UDim(0.f, 0.f), UDim(0.f, 0.f), UDim(1.0f, 0.f), UDim(1.0f, 0.f));
		buffTime->SetTextVertAlign(VA_CENTRE);
		buffTime->SetTextHorzAlign(HA_CENTRE);
		buffTime->SetFont(GUIFontManager::Instance()->GetFont("HT14"));
		buffTime->SetText("");
		buffTime->SetTouchable(false);
		buffTime->SetTextColor(Color(255,255,0));
		buffTime->SetTextBoader(true);
		itemIcon->AddChildWindow(buffTime);

		return itemIcon;
	}

	bool gui_playerInfo::changeWDBuffItem(int buffId, int duration)
	{
		for (auto buff = m_mapWalkingDeathBuff.begin(); buff != m_mapWalkingDeathBuff.end(); ++buff)
		{
			if (buff->second.buffName == buffId)
			{
				buff->second.duration = duration;
				buff->second.lasttime = 0;
				return true;
			}
		}

		buffItem buffinfo;
		buffinfo.buffName = buffId;
		buffinfo.duration = duration;
		buffinfo.lasttime = 0;
		m_mapWalkingDeathBuff.insert({ buffId, buffinfo});

		return false;
	}
}
