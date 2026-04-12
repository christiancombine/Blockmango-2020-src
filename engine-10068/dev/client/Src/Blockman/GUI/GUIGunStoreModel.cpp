#include "GUIGunStoreModel.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/ActionStateSetting.h"
#include "Setting/GunSetting.h"
#include "Actor/ActorObject.h"

namespace BLOCKMAN
{
	GUIGunStoreModel::GUIGunStoreModel() :
		gui_layout("GunStoreModel.json")
	{
	}

	GUIGunStoreModel::~GUIGunStoreModel()
	{
	}

	void GUIGunStoreModel::onLoad()
	{
		m_model = getWindow<GuiActorWindow>("GunStoreModel-Model");
		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		m_model->SetActor(pPlayer->m_sex <= 1 ? "boy.actor" : "girl.actor", "idle", -45.0f);
		m_model->SetCustomColor(pPlayer->m_skinColor);
		m_model->reflreshArmor(pPlayer);
		m_shoot = getWindow<GUIButton>("GunStoreModel-Shoot");
		m_shoot->subscribeEvent(EventButtonClick, std::bind(&GUIGunStoreModel::tryGunShoot, this, std::placeholders::_1));
		m_shoot->SetVisible(false);
	}

	void GUIGunStoreModel::SetStoreGun(StoreGun * gun)
	{
		resetModel();
		m_cur_gun_item_id = gun->ItemId;
		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		m_model->SetHoldItem(pPlayer, gun->ItemId);
		String SkillName = ActionStateSetting::getSkillName((int)AS_IDLE, gun->ItemId, 0);
		if (!SkillName.empty())
		{
			m_model->PlaySkill(SkillName);
		}
		//GunSetting* setting = GunSetting::getGunSetting(m_cur_gun_item_id);
		//m_shoot->SetVisible(setting != NULL);
	}

	void GUIGunStoreModel::SetStoreProp(StoreProp * prop)
	{
		resetModel();
		m_cur_gun_item_id = 0;
		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		m_model->SetHoldItem(pPlayer, prop->ItemId);
		String SkillName = ActionStateSetting::getSkillName((int)AS_IDLE, prop->ItemId, 0);
		if (!SkillName.empty())
		{
			m_model->PlaySkill(SkillName);
		}
		m_shoot->SetVisible(false);
	}

	void GUIGunStoreModel::SetStoreModule(StoreBlockModule * module)
	{
		m_cur_gun_item_id = 0;
		m_model->SetActor(module->Actor.c_str(), "idle");
		m_shoot->SetVisible(false);
	}

	bool GUIGunStoreModel::tryGunShoot(const EventArgs & events)
	{
		if (m_cur_gun_item_id == 0)
			return false;
		String SkillName = ActionStateSetting::getSkillName((int)AS_GUN_FIRE, m_cur_gun_item_id, 0);
		if (!SkillName.empty())
		{
			m_model->PlaySkill(SkillName);
		}
		ActorObject* actor = m_model->GetActor();
		if (actor == NULL)
		{
			return false;
		}
		GunSetting* setting = GunSetting::getGunSetting(m_cur_gun_item_id);
		if (!setting)
		{
			return false;
		}
		Quaternion q = actor->GetOrientation();
		if (setting->flameEffect.length() != 0)
		{
			Vector3 pos = setting->uiFlameEffectOffset;
			actor->AddPluginEffect("FlameEffect", "bip01", setting->uiFlameEffect, setting->uiFlameEffectOffsetS, 1.0f, 1, pos, q);
		}
		if (setting->bulletEffect.length() != 0)
		{
			Vector3 pos = setting->uiBulletEffectOffset;
			actor->AddPluginEffect("BulletEffect", "bip01", setting->uiBulletEffect, setting->uiBulletEffectOffsetS, 1.0f, 1, pos, q);
		}
		return true;
	}

	void GUIGunStoreModel::resetModel()
	{
		EntityPlayerSPMP* pPlayer = Blockman::Instance()->m_pPlayer;
		m_model->SetActor(pPlayer->m_sex <= 1 ? "boy.actor" : "girl.actor", "idle", -45.0f);
		m_model->SetCustomColor(pPlayer->m_skinColor);
		m_model->reflreshArmor(pPlayer);
	}

}
