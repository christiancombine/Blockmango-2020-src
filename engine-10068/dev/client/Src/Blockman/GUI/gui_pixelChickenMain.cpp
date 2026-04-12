#include "gui_pixelChickenMain.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "game.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GUIStorePropertyItem.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "cWorld/Blockman.h"

namespace BLOCKMAN
{
	gui_pixelChickenMain::gui_pixelChickenMain() :
		gui_layout("PixelChickenMain.json")
	{
	}

	gui_pixelChickenMain::~gui_pixelChickenMain()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_pixelChickenMain::onLoad()
	{
		m_survive = getWindow<GUIStaticText>("PixelChickenMain-Survive-Text");
		m_kill = getWindow<GUIStaticText>("PixelChickenMain-Kill-Text");
		m_poison = getWindow<GUIStaticText>("PixelChickenMain-Poison-Text");
		m_weapon = getWindow<GUIStaticImage>("PixelChickenMain-Weapon");
		m_weapon_icon = getWindow<GUIStaticImage>("PixelChickenMain-Weapon-Icon");
		m_weapon_progress = getWindow<GUIStaticImage>("PixelChickenMain-Weapon-Progress");
		m_shoot = getWindow<GUIStaticImage>("PixelChickenMain-Shoot-Icon");
		m_foot = getWindow<GUIStaticImage>("PixelChickenMain-Foot-Icon");
		m_gun = getWindow<GUIStaticImage>("PixelChickenMain-Gun-Icon");
		
		m_watch = getWindow<GUILayout>("PixelChickenMain-Watch");
		m_back = getWindow<GUIButton>("PixelChickenMain-Watch-Back");
		m_next = getWindow<GUIButton>("PixelChickenMain-Watch-Next");
		m_exit = getWindow<GUIButton>("PixelChickenMain-Watch-Exit");
		m_poison_mask = getWindow<GUIStaticImage>("PixelChickenMain-Poison-Mask");

		m_revive_bg = getWindow<GUILayout>("PixelChickenMain-Revive");
		//m_revive_icon = getWindow<GUIStaticImage>("PixelChickenMain-Revive-Icon");
		m_no_revive_icon = getWindow<GUIStaticImage>("PixelChickenMain-No-Revive-Icon");
		m_revive_text = getWindow<GUIStaticText>("PixelChickenMain-Revive-Text");

		m_weapon_info = getWindow<GUILayout>("PixelChickenMain-Weapon-Info");
		m_weapon_info_view = getWindow<GUILayout>("PixelChickenMain-Info-View");
		m_weapon_info_property = getWindow<GUILayout>("PixelChickenMain-Info-Property");
		m_weapon_info_img = getWindow<GUIStaticImage>("PixelChickenMain-Info-Img");
		m_weapon_info_btn = getWindow<GUIButton>("PixelChickenMain-Info-Btn");
		m_damage_value = getWindow<GUIStaticText>("PixelChickenMain-Damage-Value");
		m_speed_value = getWindow<GUIStaticText>("PixelChickenMain-Speed-Value");

		getWindow<GUIStaticText>("PixelChickenMain-Damage-Text")->SetText(getString("pixel_damage_text").c_str());
		getWindow<GUIStaticText>("PixelChickenMain-Speed-Text")->SetText(getString("pixel_speed_text").c_str());

		m_weapon_info->SetVisible(false);
		m_revive_bg->SetVisible(false);
		m_weapon->SetVisible(false);
		m_shoot->SetVisible(false);
		m_foot->SetVisible(false);
		m_gun->SetVisible(false);
		m_watch->SetVisible(false);
		m_poison_mask->SetVisible(false);
		m_back->SetText(getString("gui_pixel_gun_result_btn_back"));
		m_next->SetText(getString("gui_pixel_gun_result_btn_next"));
		m_exit->SetText(getString("pixel_chicken_btn_exit"));

		m_subscriptionGuard.add(SendChickenWarningEvent::subscribe(std::bind(&gui_pixelChickenMain::showWarning, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowChickenWatchEvent::subscribe([this]() -> bool {
			m_weapon_info->SetVisible(false);
			m_watch->SetVisible(true);
			return true;
		}));
		m_exit->subscribeEvent(EventButtonClick, std::bind(&gui_pixelChickenMain::watchButtonClick, this, WatchButton::EXIT));
		m_back->subscribeEvent(EventButtonClick, std::bind(&gui_pixelChickenMain::watchButtonClick, this, WatchButton::BACK));
		m_next->subscribeEvent(EventButtonClick, std::bind(&gui_pixelChickenMain::watchButtonClick, this, WatchButton::NEXT));
		m_weapon_info_btn->subscribeEvent(EventButtonClick, std::bind(&gui_pixelChickenMain::ifShowWeaponView, this));


		RootGuiLayout::Instance()->showWindowByLayoutId(RootGuiLayout::LayoutId::PIXEL_CHICKEN_MAIN);
	}

	void gui_pixelChickenMain::onUpdate(ui32 nTimeElapse)
	{
		if (m_poison_time > 0)
		{
			m_poison_time -= nTimeElapse;
			if (m_poison_time < 0)
				m_poison_time = 0;

			m_poison->SetText(getTimeString(m_poison_time).c_str());
		}


		if (m_revive_time > 0)
		{
			m_revive_time -= nTimeElapse;
			if (m_revive_time < 0)
			{
				m_revive_time = 0;
				setReviveStatus(false);
			}

			m_revive_text->SetText(getTimeString(m_revive_time).c_str());
		}

		if (m_shoot->IsVisible())
		{
			if (m_shootWarningTime <= 0)
			{
				m_shoot->SetVisible(false);
			}
			else
			{
				m_shootWarningTime -= nTimeElapse;
				m_shoot->SetPosition({ { 0.0f, Math::Sin(m_shootWarningRotateAngle * Math::DEG2RAD) * 120.f }, { 0.0f, Math::Cos(m_shootWarningRotateAngle * Math::DEG2RAD) * -120.f } });
				m_shoot->SetVisible(true);
			}
		}

		if (m_foot->IsVisible())
		{
			if (m_footWarningTime <= 0)
			{
				m_foot->SetVisible(false);
			}
			else
			{
				m_footWarningTime -= nTimeElapse;
				m_foot->SetPosition({ { 0.0f, Math::Sin(m_footWarningRotateAngle * Math::DEG2RAD) * 80.f }, { 0.0f, Math::Cos(m_footWarningRotateAngle * Math::DEG2RAD) * -80.f } });
				m_foot->SetVisible(true);
			}
		}

		if (m_weapon->IsVisible())
		{
			m_change_weapon_wait += nTimeElapse;
			int section = Math::Min((int)Math::Floor(m_change_weapon_wait / m_change_weapon_section), 30);
			m_weapon_progress->SetVisible(section > 0);
			m_weapon_progress->SetImage(StringUtil::Format("set:progress.json image:%d", section).c_str());
			if ((int)Math::Floor(m_change_weapon_wait / m_change_weapon_section) > 30)
			{
				m_weapon->SetVisible(false);
				ClientNetwork::Instance()->getCommonSender()->sendReceiveChickenWeapon();
			}

		}
	}

	bool gui_pixelChickenMain::isNeedLoad()
	{
		return checkGameType(ClientGameType::PixelGunChicken);
	}

	void gui_pixelChickenMain::onGetCommonData(String key, String data)
	{
		if (key == "ChangeWeapon")
		{
			auto builder = CommonDataBuilder().fromData(data);
			String image = builder.getStringParam("image");
			String bgImage = builder.getStringParam("bgImage");
			int time = builder.getIntParam("time");
			resetWeaponIcon(image, bgImage, time);
			return;
		}

		if (key == "ChickenGameData")
		{
			resetChickenInfo(data);
			return;
		}

		if (key == "ChickenWarningSetting")
		{
			auto builder = CommonDataBuilder().fromData(data);
			bool canShow = builder.getBoolParam("canShow");
			float shootRange = builder.getFloatParam("shootRange");
			float footRange = builder.getFloatParam("footRange");
			float weaponRange = builder.getFloatParam("weaponRange");
			setWarningRange(shootRange, footRange, weaponRange);
			setCanWarning(canShow);
			return;
		}

		if (key == "HideChickenWarning")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int type = builder.getIntParam("type");
			hideWarning(type);
			return;
		}

		if (key == "ShowChickenWarning")
		{
			auto builder = CommonDataBuilder().fromData(data);
			String position = builder.getStringParam("position");
			int type = builder.getIntParam("type");
			StringArray array = StringUtil::Split(position, ":");
			if (array.size() == 3)
			{
				float x = StringUtil::ParseFloat(array[0]);
				float y = StringUtil::ParseFloat(array[1]);
				float z = StringUtil::ParseFloat(array[2]);
				Vector3 pos = Vector3(x, y, z);
				SendChickenWarningEvent::emit(pos, type);
			}
			return;
		}

		if (key == "ShowPoisonMask")
		{
			auto builder = CommonDataBuilder().fromData(data);
			bool isShow = builder.getBoolParam("isShow");
			showPoisonMask(isShow);
			return;
		}

		if (key == "WeaponInfo")
		{
			updateWeaponInfo(data);
			return;
		}
	}

	bool gui_pixelChickenMain::resetWeaponIcon(const String& image, const String& bg, int time)
	{
		if (!isLoaded())
			return false;

		if (image.empty())
		{
			m_weapon->SetVisible(false);
			return false;
		}
		m_change_weapon_wait = 0;
		m_change_weapon_section = (float)time / 30.f;
		m_weapon_icon->SetImage(image.c_str());
		m_weapon->SetImage(bg.c_str());
		m_weapon->SetVisible(true);
		return true;
	}

	void gui_pixelChickenMain::resetChickenInfo(const String & data)
	{
		if (!isLoaded())
			return;

		if (data.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}
		m_survive->SetText(doc->FindMember("Survive")->value.GetString());
		m_kill->SetText(doc->FindMember("Kill")->value.GetString());

		if (doc->HasMember("Poison"))
		{
			m_poison_time = doc->FindMember("Poison")->value.GetInt();
		}

		if (doc->HasMember("Revive") && m_revive_time <= 0)
		{
		    m_revive_bg->SetVisible(true);
			m_revive_time = doc->FindMember("Revive")->value.GetInt();
		}
		setReviveStatus(m_revive_time > 0);
		LordDelete(doc);
	}

	void gui_pixelChickenMain::updateWeaponInfo(const String & data)
	{	
		if (!isLoaded())
			return;
		if (data.length() == 0)
			return;
		if (m_watch->IsVisible())
			return;

		m_weapon_info->SetVisible(true);
		m_isShowWeaponView = true;
		m_weapon_info_view->SetVisible(m_isShowWeaponView);
		m_weapon_info_btn->SetXPosition({ 0.f, 0.f });
		m_weapon_info_btn->SetNormalImage("set:pixel_chicken_main.json image:pack_up_btn");
		m_weapon_info_btn->SetPushedImage("set:pixel_chicken_main.json image:pack_up_btn");
		
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}

		m_weapon_info_img->SetImage(doc->FindMember("img")->value.GetString());
		m_damage_value->SetText(doc->FindMember("damage")->value.GetString());
		m_speed_value->SetText(doc->FindMember("speed")->value.GetString());
		auto properties = doc->FindMember("properties")->value.GetArray();
		m_weapon_info_property->CleanupChildren();
		float propertyHeight = 32.f;
		int index = 0;
		for (const auto& item : properties)
		{
			if (!item.HasMember("img") || !item.HasMember("name"))
			{
				return;
			}
			index++;
			GUIString PropertyName = StringUtil::Format("PixelChicken-Weapon-Property-%d", index).c_str();
			GUIStorePropertyItem* pPropertyItem = (GUIStorePropertyItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STORE_PROPERTY_ITEM, PropertyName);
			auto img = item.FindMember("img")->value.GetString();
			auto name = item.FindMember("name")->value.GetString();
			pPropertyItem->SetWidth({ 1.0f, 0.0f });
			pPropertyItem->SetHeight({ 0.f, propertyHeight });
			pPropertyItem->SetYPosition({ 0.0f, (index - 1) * propertyHeight });
			pPropertyItem->SetProperty(img, name, "set:pixel_chicken_main.json image:bg_infos_item");
			m_weapon_info_property->AddChildWindow(pPropertyItem);
		}
	}

	bool gui_pixelChickenMain::showWarning(const Vector3& position, int type)
	{
		if (!isLoaded())
			return false;

		if (!m_can_show_warning)
			return false;

		EntityPlayer* targetPlayer = BLOCKMAN::Blockman::Instance()->m_pPlayer;
		if (targetPlayer->isDead)
			return false;

		auto range = getWarningRange(type);

		float distanse = targetPlayer->position.distanceTo(position);
		if (distanse > range)
			return false;

		bool isScreenLeft = false;
		Vector3 targetDir = position - targetPlayer->position;
		targetDir.y = 0.0f;
		targetDir.normalize();

		Vector3 viewDir = SceneManager::Instance()->getMainCamera()->getDirection();
		viewDir.y = 0;
		viewDir.normalize();
		float cosValue = targetDir.dot(viewDir);
		Vector3 tangent = targetDir.cross(viewDir);
		if (tangent.y < 0)
			isScreenLeft = true;

		float rotateAngle = Math::ACos(cosValue);
		float rate = 1 - distanse / range;

		switch ((WarningType)type)
		{
		case WarningType::SHOOT:
			m_shoot->SetVisible(true);
			m_shoot->SetAlpha(rate);
			m_shootWarningTime = 1500;
			if (isScreenLeft)
			{
				m_shootWarningRotateAngle = 360.f - rotateAngle * Math::RAD2DEG;
			}
			else
			{
				m_shootWarningRotateAngle = rotateAngle * Math::RAD2DEG;
			}
			break;
		case WarningType::FOOT:
			m_foot->SetVisible(true);
			m_foot->SetAlpha(rate);
			m_footWarningTime = 1500;
			if (isScreenLeft)
			{
				m_footWarningRotateAngle = 360.0f - rotateAngle * Math::RAD2DEG;
			}
			else
			{
				m_footWarningRotateAngle = rotateAngle * Math::RAD2DEG;
			}
			break;
		case WarningType::GUN:
			m_gun->SetVisible(true);
			m_gun->SetAlpha(rate);
			if (isScreenLeft)
			{
				m_gunWarningRotateAngle = 360.0f - rotateAngle * Math::RAD2DEG;
			}
			else
			{
				m_gunWarningRotateAngle = rotateAngle * Math::RAD2DEG;
			}
			m_gun->SetPosition({ { 0.0f, Math::Sin(m_gunWarningRotateAngle * Math::DEG2RAD) * 160.f }, { 0.0f, Math::Cos(m_gunWarningRotateAngle * Math::DEG2RAD) * -160.f } });
			break;
		default:
			break;
		}
		return false;
	}

	bool gui_pixelChickenMain::hideWarning(int type)
	{
		if (!isLoaded())
			return false;

		switch ((WarningType)type)
		{
		case WarningType::SHOOT:
			m_shoot->SetVisible(false);
			break;
		case WarningType::FOOT:
			m_foot->SetVisible(false);
			break;
		case WarningType::GUN:
			m_gun->SetVisible(false);
			break;
		case WarningType::WEAPON_INFO:
			m_weapon_info->SetVisible(false);
			break;
		default:
			break;
		}
		return false; 
	}

	bool gui_pixelChickenMain::watchButtonClick(WatchButton type)
	{
		playSoundByType(ST_Click);

		switch (type)
		{
		case WatchButton::EXIT:
			GameClient::CGame::Instance()->exitGame();
			break;
		case WatchButton::BACK:
			ClientNetwork::Instance()->getCommonSender()->sendPixelGunBackHall();
			break;
		case WatchButton::NEXT:
			ClientNetwork::Instance()->getCommonSender()->sendPixelGunPlayAgain();
			break;
		default:
			break;
		}
		return false;
	}

	bool gui_pixelChickenMain::showPoisonMask(bool isShow)
	{
		if (!isLoaded())
			return false;
		m_poison_mask->SetVisible(isShow);
		return true;
	}


	void gui_pixelChickenMain::setWarningRange(float shootRange, float footRange, float weaponRange)
	{
		m_shootWarningRange = shootRange;
		m_footWarningRange = footRange;
		m_weaponWarningRange = weaponRange;
	}

	float gui_pixelChickenMain::getWarningRange(int type)
	{
		if (type == (int)WarningType::SHOOT)
			return m_shootWarningRange > 0.f ? m_shootWarningRange : 10000.0f;
		if (type == (int)WarningType::FOOT)
			return m_footWarningRange > 0.f ? m_footWarningRange : 10000.0f;
		if (type == (int)WarningType::GUN)
			return m_weaponWarningRange > 0.f ? m_weaponWarningRange : 10000.0f;
		return 10000.0f;
	}

	void gui_pixelChickenMain::setReviveStatus(bool canRevive)
	{
	    if (!m_revive_bg->IsVisible())
	        return;
		if (canRevive)
		{
			m_revive_bg->SetBackImage("set:pixel_chicken_main.json image:revive_bg");
			m_no_revive_icon->SetVisible(false);
		}
		else
		{
			m_revive_bg->SetBackImage("set:pixel_chicken_main.json image:stop_revive_bg");
			m_no_revive_icon->SetVisible(true);
		}
	}

	String gui_pixelChickenMain::getTimeString(int time)
	{
		if (time < 0)
			return "";
		int seconds = time / 1000;
		int second = seconds % 60;
		int minute = (seconds % 3600) / 60;
		auto  result = StringUtil::Format("%s:%s",
			minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
			second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str());
		return result;

	}

	bool gui_pixelChickenMain::ifShowWeaponView()
	{
		m_isShowWeaponView = !m_isShowWeaponView;
		m_weapon_info_view->SetVisible(m_isShowWeaponView);
		float btnY = m_isShowWeaponView ? 0.f : -0.93f;
		auto btnImg = m_isShowWeaponView ? "set:pixel_chicken_main.json image:pack_up_btn" : "set:pixel_chicken_main.json image:unfold_btn";
		m_weapon_info_btn->SetNormalImage(btnImg);
		m_weapon_info_btn->SetPushedImage(btnImg);
		m_weapon_info_btn->SetXPosition({ btnY, 0.0f });
		return false;
	}

}