#include "GunSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN 
{
	std::map<int, GunSetting*> GunSetting::m_gunSettingMap = {};

	bool GunSetting::loadSetting(const String& path)
	{
		GunSetting* pSetting = NULL;
		CsvReader* pReader = new CsvReader();
			
		bool bRet = pReader->LoadFile(path.c_str());
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}

		int tempInt;
		String tempStr;
		StringArray tempArr;
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pSetting = new GunSetting();
			pReader->GetInt("GunId", pSetting->gunId);
			pReader->GetInt("BulletPerShoot", pSetting->bulletPerShoot);
			pReader->GetInt("MaxBulletNum", pSetting->maxBulletNum);
			pReader->GetInt("SoundType", pSetting->soundType);
			pReader->GetInt("EmptySoundType", pSetting->emptySoundType);
			pReader->GetInt("AdapedtBullet", pSetting->adaptedBulletId);
			pReader->GetInt("SwitchClipSoundType", pSetting->switchClipSoundType);
			pReader->GetInt("ReloadTime", pSetting->reloadTick);

			pReader->GetInt("GunType", tempInt);
			pSetting->gunType = (GUN_TYPE)tempInt;
			pReader->GetInt("BulletType", tempInt);
			pSetting->bulletType = (BulletType)tempInt;

			pReader->GetFloat("ShootRange", pSetting->shootRange);
			pReader->GetFloat("PrepareTime", pSetting->prepareTime);
			pReader->GetFloat("CdTime", pSetting->cdTime);
			pReader->GetFloat("BulletOffset", pSetting->bulletOffset);
			pReader->GetFloat("SniperDistance", pSetting->sniperDistance);
			pReader->GetFloat("MouseSensitivity", pSetting->mouseSensitivity);
			pReader->GetFloat("Damage", pSetting->damage);
			pReader->GetFloat("BulletSpeed", pSetting->bulletSpeed);
			pReader->GetFloat("KnockBackDistance", pSetting->knockbackDistance);
			pReader->GetFloat("Recoil", pSetting->recoil);
			pReader->GetFloat("CrossHairMoveSpeed", pSetting->crossHairMoveSpeed);
			pReader->GetFloat("CrossHairMoveDistance", pSetting->crossHairMoveDistance);
			pReader->GetFloat("HeadshotDamage", pSetting->headshotDamage);

			pReader->GetString("Name", pSetting->gunName);
			pReader->GetString("Desc", pSetting->gunDesc);
			pReader->GetString("Model", pSetting->gunModel);
			pReader->GetString("SwitchMagazineAction", pSetting->switchMagazineAction);
			pReader->GetString("PreviewMesh", pSetting->previewMesh);
			pReader->GetString("GunIcon", pSetting->gunIcon);

			String bulletEffect;
			pReader->GetString("BulletEffect", bulletEffect);
			if (bulletEffect.length() <= 7)
				pSetting->bulletEffect = "effect_gun_default_bullet.effect";
			else
				pSetting->bulletEffect = bulletEffect;
			
			String flameEffect;
			pReader->GetString("FlameEffect", flameEffect);
			if (flameEffect != "none")
				if (flameEffect.length() <= 7)
					pSetting->flameEffect = "effect_gun_default_flame.effect";
				else
					pSetting->flameEffect = flameEffect;

			String beShotEffect;
			pReader->GetString("BeShotEffect", beShotEffect);
			if (beShotEffect != "none")
				if (beShotEffect.length() <= 7)
					pSetting->beShotEffect = "effect_gun_be_shot.effect";
				else
					pSetting->beShotEffect = beShotEffect;

			String explosionEffect;
			pReader->GetString("ExplosionEffect", explosionEffect);
			if (explosionEffect != "none")
				if (explosionEffect.length() <= 7)
					pSetting->explosionEffect = "effect_gun_default_explosion.effect";
				else
					pSetting->explosionEffect = explosionEffect;

			String uiBulletEffect;
			pReader->GetString("UIBulletEffect", uiBulletEffect);
			if (uiBulletEffect.length() <= 7)
				pSetting->uiBulletEffect = "effect_gun_default_bullet.effect";
			else
				pSetting->uiBulletEffect = uiBulletEffect;

			String uiFlameEffect;
			pReader->GetString("UIFlameEffect", uiFlameEffect);
			if (uiFlameEffect != "none")
				if (uiFlameEffect.length() <= 7)
					pSetting->uiFlameEffect = "effect_gun_default_flame.effect";
				else
					pSetting->uiFlameEffect = uiFlameEffect;

			pReader->GetBool("IsPiercing", pSetting->isPiercing);
			pReader->GetFloat("ExplosionRange", pSetting->explosionRange);
			pReader->GetFloat("GravityVelocity", pSetting->gravityVelocity);
			pReader->GetInt("BounceTimes", pSetting->bounceTimes);
			pReader->GetBool("IsAutoShoot", pSetting->isAutoShoot);

			pReader->GetString("FPVEffect", tempStr);
			tempArr = StringUtil::Split(tempStr, "#");
			LordAssert(tempArr.size() == 4);
			pSetting->flame_1_frontOff = StringUtil::ParseFloat(tempArr[0]);
			pSetting->flame_1_rightOff = StringUtil::ParseFloat(tempArr[1]);
			pSetting->flame_1_downOff = StringUtil::ParseFloat(tempArr[2]);
			pSetting->flame_1_scale = StringUtil::ParseFloat(tempArr[3]);

			pReader->GetString("TPVEffect", tempStr);
			tempArr = StringUtil::Split(tempStr, "#");
			LordAssert(tempArr.size() == 4);
			pSetting->flame_3_frontOff = StringUtil::ParseFloat(tempArr[0]);
			pSetting->flame_3_rightOff = StringUtil::ParseFloat(tempArr[1]);
			pSetting->flame_3_downOff = StringUtil::ParseFloat(tempArr[2]);
			pSetting->flame_3_scale = StringUtil::ParseFloat(tempArr[3]);

			pReader->GetString("UIBulletEffectOffset", tempStr);
			tempArr = StringUtil::Split(tempStr, "#");
			LordAssert(tempArr.size() == 4);
			pSetting->uiBulletEffectOffset = Vector3(StringUtil::ParseFloat(tempArr[0]), StringUtil::ParseFloat(tempArr[1]), StringUtil::ParseFloat(tempArr[2]));
			pSetting->uiBulletEffectOffsetS = StringUtil::ParseFloat(tempArr[3]);

			pReader->GetString("UIFlameEffectOffset", tempStr);
			tempArr = StringUtil::Split(tempStr, "#");
			LordAssert(tempArr.size() == 4);
			pSetting->uiFlameEffectOffset = Vector3(StringUtil::ParseFloat(tempArr[0]), StringUtil::ParseFloat(tempArr[1]), StringUtil::ParseFloat(tempArr[2]));
			pSetting->uiFlameEffectOffsetS = StringUtil::ParseFloat(tempArr[3]);

			auto oldSetting = m_gunSettingMap[pSetting->gunId];
			if (oldSetting)
			{
				LordSafeDelete(oldSetting);
				m_gunSettingMap[pSetting->gunId] = NULL;
			}
			m_gunSettingMap[pSetting->gunId] = pSetting;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void GunSetting::unloadSetting()
	{
		for (auto iter = m_gunSettingMap.begin(); iter != m_gunSettingMap.end(); iter++)
		{
			GunSetting* pSetting = iter->second;
			delete pSetting;
		}

		m_gunSettingMap.clear();
	}

	GunSetting * GunSetting::getGunSetting(int gunId)
	{
		auto iter = m_gunSettingMap.find(gunId);
		if (iter != m_gunSettingMap.end())
		{
			return iter->second;
		}

		return nullptr;
	}

	void GunSetting::updateGunSetting(GunPluginSetting setting)
	{
		GunSetting * gunSetting = getGunSetting(setting.gunId);
		if (gunSetting)
		{
			if (setting.damage != -1)
			{
				gunSetting->damage = setting.damage;
			}
			if (setting.maxBulletNum != -1)
			{
				gunSetting->maxBulletNum = setting.maxBulletNum;
			}
			if (setting.gunType != 0)
			{
				gunSetting->gunType = (GUN_TYPE)setting.gunType;
			}
			if (setting.knockback != -1)
			{
				gunSetting->knockbackDistance = setting.knockback;
			}
			if (setting.bulletId != 0)
			{
				gunSetting->adaptedBulletId = setting.bulletId;
			}
			if (setting.shootRange != 0)
			{
				gunSetting->shootRange = setting.shootRange;
			}
			if (setting.cdTime != 0)
			{
				gunSetting->cdTime = setting.cdTime;
			}
			if (setting.sniperDistance != -1)
			{
				gunSetting->sniperDistance = setting.sniperDistance;
			}
			if (setting.mouseSensitivity != 0)
			{
				gunSetting->mouseSensitivity = setting.mouseSensitivity;
			}
			if (setting.bulletPerShoot != -1)
			{
				gunSetting->bulletPerShoot = setting.bulletPerShoot;
			}
			if (setting.bulletOffset != -1)
			{
				gunSetting->bulletOffset = setting.bulletOffset;
			}
			if (setting.headshotDamage != -1)
			{
				gunSetting->headshotDamage = setting.headshotDamage;
			}
			if (setting.recoil >= 0.01f)
			{
				gunSetting->recoil = setting.recoil;
			}
			if (setting.bulletEffect.length() > 7)
			{
				gunSetting->bulletEffect = setting.bulletEffect;
			}
			if (setting.flameEffect.length() > 7)
			{
				gunSetting->flameEffect = setting.flameEffect;
			}
			if (setting.isPiercing != -1)
			{
				gunSetting->isPiercing = setting.isPiercing;
			}
			if (setting.explosionRange >= 0.0f)
			{
				gunSetting->explosionRange = setting.explosionRange;
			}
			if (setting.gravityVelocity >= 0.0f)
			{
				gunSetting->gravityVelocity = setting.gravityVelocity;
			}
			if (setting.bounceTimes != -1)
			{
				gunSetting->bounceTimes = setting.bounceTimes;
			}
			if (setting.isAutoShoot != -1)
			{
				gunSetting->isAutoShoot = setting.isAutoShoot;
			}
			if (setting.bulletSpeed != -1.0f)
			{
				gunSetting->bulletSpeed = setting.bulletSpeed;
			}
			if (setting.isFireworks != -1)
			{
				gunSetting->isFireworks = setting.isFireworks;
			}
		}
	}
	bool GunSetting::isGunItem(int itemId)
	{
		auto setting = getGunSetting(itemId);
		if (setting)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}


