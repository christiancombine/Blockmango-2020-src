#include "PlayerPrefs.h"
#include "Any/VarList.h"

namespace BLOCKMAN
{
	PlayerPrefs::PlayerPrefs()
	{
		init();
	}

	void PlayerPrefs::init()
	{
		m_mapProperty.clear();

#pragma region //Common
		putBoolPrefs("ClampDoHurt", true);
		putBoolPrefs("EnableReconnectNetwork", false);
#pragma endregion

#pragma region //Client
		putBoolPrefs("IgnoreFrustumCheck", false);
		putBoolPrefs("ShowHurtColor", true);
		putBoolPrefs("DisableFog", false);
		putBoolPrefs("DisStepSound", false);
		putBoolPrefs("DisFallSound", false);
		putBoolPrefs("EnableBodyParts", true);
		putBoolPrefs("FloatNumberRenderTop", true);
		putBoolPrefs("StaticNumberRenderTop", true);
		putBoolPrefs("StopAndUnloadSound", true);
		putBoolPrefs("DebugSound", false);
		putBoolPrefs("DisableInertialFly", false);
		putIntPrefs("SchematicId", 0);
		putIntPrefs("SimpleEffectRenderDistance", 80);
		putIntPrefs("ShowFloatNumberTime", 500);
		putIntPrefs("LoadingSoundId", 10005);
		putIntPrefs("ClientDelayTicksToFree", 50);
		putFloatPrefs("CameraDistanceMax", 200.0f);
		putFloatPrefs("ConcealPowerScale", 1.0F);
		putFloatPrefs("FloatNumberSpeed", 1.0F);
		putFloatPrefs("FloatNumberTimeSpeed", 1.0F);
		putFloatPrefs("EntityOtherPlayerMPRenderDistanceWeight", 1.f);
		SetProperty<Vector3iBoolMap>("SchematicPlaceCoverResult", Vector3iBoolMap{});
		SetProperty<ChestInfoMap>("ChestInfo", ChestInfoMap{});
		putFloatPrefs("BlockReachDistance", 6.5F);
		putFloatPrefs("EntityReachDistance", 5.0F);
		putBoolPrefs("IsSeparateCamera", false);
		putBoolPrefs("IsViewBobbing", true);
		putBoolPrefs("UseJumpBegin", false);
		putFloatPrefs("PoleControlMaxDistance", 25.0F);
		putFloatPrefs("MainControlKeyAlphaNormal", 0.5F);
		putFloatPrefs("MainControlKeyAlphaPress", 0.6F);
		putIntPrefs("QuicklyBuildBlockNum", 1);
		putFloatPrefs("RenderTextAlpha", 1.0F);

		putBoolPrefs("LockUIShowAndHide", false);

		putBoolPrefs("banClickCD", false);
		putBoolPrefs("SoundEffectSwitch", true);
		putBoolPrefs("RenderHeadText", true);
		putBoolPrefs("SyncClientPositionToServer", true);
		putFloatPrefs("SoundEffectVolume", 1.0F);
		putFloatPrefs("BackGroundVolume", 1.0F);

		// old game rule
		putBoolPrefs("IsCanSprint", true);
		putBoolPrefs("IsCreatureCollision", true);
		putBoolPrefs("IsActorAction", true);
		putBoolPrefs("IsGunAction", true);
		putBoolPrefs("IsGunNorAttack", true);
		putBoolPrefs("IsBlockmanCollision", true);
		putBoolPrefs("IsBlockmanLongHit", false);
		putIntPrefs("BlockmanLongHitTimes", -1);
		putIntPrefs("HurtProtectTime", 10);
		putBoolPrefs("IsLoadSeason", false);
		putBoolPrefs("IsCreatureBloodBar", false);
		putBoolPrefs("IsGunShootActor", false);
		putBoolPrefs("IsUseCustomArmor", false); // CustomArmor.csv
		putBoolPrefs("IsForcePlayerDeathSound", false);
		putBoolPrefs("IsHeldItemAction", true);
		putIntPrefs("BlockRenderDistance", -1);

		// old ui display
		putBoolPrefs("IsShowGunDetail", false);
		putBoolPrefs("IsShowBuyBullet", false);
		putBoolPrefs("IsShowGetGoodsBg", true);
		putBoolPrefs("IsHidePersonalShop", false);
		putBoolPrefs("IsShowBackHall", false);
		putBoolPrefs("IsShowTeamUI", true);
		putBoolPrefs("IsRedAim", true);
		putBoolPrefs("IsRedAimBlock", true);
		putBoolPrefs("IsRedAimActor", false);
		putBoolPrefs("IsShowCrafting", false);
		putBoolPrefs("IsShowItemDurability", false);
	
		// player Bobbing scale
		putFloatPrefs("PlayerBobbingScale", 0.35F);

#pragma endregion

#pragma region //Server
		putBoolPrefs("SendSystemsChat", true);
		putBoolPrefs("DisableAttrCloth", false);
		putIntPrefs("BlockNeedUpdateDistance", 80);
		putIntPrefs("BlockNeedSyncSectionDistance", 5);
#pragma endregion
	}

	void PlayerPrefs::putBoolPrefs(const String & key, bool value)
	{
		SetProperty<bool>(key.c_str(), value);
	}

	bool PlayerPrefs::getBoolPrefs(const String & key)
	{
		if (HasProperty(key.c_str()))
		{
			return GetProperty<bool>(key.c_str());
		}
		return false;
	}

	void PlayerPrefs::putIntPrefs(const String & key, int value)
	{
		SetProperty<int>(key.c_str(), value);
	}

	int PlayerPrefs::getIntPrefs(const String & key)
	{
		if (HasProperty(key.c_str()))
		{
			return GetProperty<int>(key.c_str());
		}
		return 0;
	}

	void PlayerPrefs::putFloatPrefs(const String & key, float value)
	{
		SetProperty<float>(key.c_str(), value);
	}

	float PlayerPrefs::getFloatPrefs(const String & key)
	{
		if (HasProperty(key.c_str()))
		{
			return GetProperty<float>(key.c_str());
		}
		return 0.0f;
	}

	void PlayerPrefs::putStringPrefs(const String & key, String value)
	{
		SetProperty<String>(key.c_str(), value);
	}

	String PlayerPrefs::getStringPrefs(const String & key)
	{
		if (HasProperty(key.c_str()))
		{
			return GetProperty<String>(key.c_str());
		}
		return "";
	}
}