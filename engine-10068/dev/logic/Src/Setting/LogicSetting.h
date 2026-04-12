#pragma once

#ifndef __LOGIC_SETTINF_HEADER__
#define __LOGIC_SETTINF_HEADER__

#include "BM_TypeDef.h"
#include "Inventory/InventoryPlayer.h"
#include "GunSetting.h"
#include "BulletClipSetting.h"
#include "MonsterSetting.h"
#include "SkillSetting.h"
#include "GameTypeSetting.h"

namespace BLOCKMAN
{
	class BlockDynamicAttr
	{
	public:
		int blockId{};
		float hardness{};
		DEFINE_NETWORK_SERIALIZER(blockId, hardness);
	};

	class ToolItemDynamicAttr
	{
	public:
		int itemId{};
		float distance{};
		float efficiency{};
		DEFINE_NETWORK_SERIALIZER(itemId, distance, efficiency);
	};

	class LogicSetting : public Singleton<LogicSetting>, public ObjectAlloc
	{
	private:
		int m_maxInventorySize = InventoryPlayer::OLD_MAIN_INVENTORY_COUNT;
		int m_maxEnderInventorySize = 27;
		bool m_sneakShowName = false;
		bool m_breakBlockSoon = true;
		bool m_isCanDamageItem = true;
		bool m_isNeedFoodStats = true;
		bool m_canEatWhenever = false;
		bool m_isFoodHeal = true;
		float m_attackCoefficientX = 0.04f;
		float parachuteDownSpeed = 0.8f; // = 0.8F;
		float parachuteMoveSpeed = 0.05f; // = 0.05F;
		int m_entityItemLife = 6000;
		float m_disableSelectEntity = false;
		float m_hideClouds = false;
		float m_canCloseChest = true;
		vector<GunPluginSetting>::type m_gunPluginSetting;
		vector<BulletClipPluginSetting>::type m_bulletClipPluginSetting;
		vector<BlockDynamicAttr>::type m_blockDynamicAttr;
		vector<ToolItemDynamicAttr>::type m_toolItemDynamicAttr;
		vector<SkillItem>::type m_skillItemSetting;
		bool m_gunIsNeedBullet = true;
		bool m_showGunEffectWithSingle = false;// single war has no different teamId...it is also useful to the headshot with single war
		bool m_allowHeadshot = false;
		bool m_enableAutoShoot = false;
		float m_thirdPersonDistance = 3.0f;
		int m_quicklyBuildBlockCost = 12;
		int m_quicklyBuildBlockNum = 5;
		bool m_swordCanBreakBlock = false;
		bool m_need_durable = true;
		float m_renderYawThreshold = 50.0f;
		float m_renderYawScale = 0.2f;
		String m_deathAnimEffect = "";
		bool m_isEnableIndie = false;
		bool m_isUseMovingEnderChest = false;
		bool m_hideArmor = false;
		int m_max_changes_in_on_chunk = 64;
		ClientGameType m_gameType = ClientGameType::COMMON;
		bool m_isSeparateCameraControl = false;
		bool m_useLocalScaleInHand = false;
		int m_gameWeatherType = 0;// 1 = rain  2 = snow
	public:
		LogicSetting();
		~LogicSetting();

		// max inventory size
		int getMaxInventorySize() { return m_maxInventorySize; }
		void setMaxInventorySize(int size) { m_maxInventorySize = size; }
		int getMaxEnderInventorySize() { return m_maxEnderInventorySize; }
		void setMaxEnderInventorySize(int size) { m_maxEnderInventorySize = size; }
		// sneak show name
		void setSneakShowName(bool isShow) { m_sneakShowName = isShow; }
		bool isSneakShowName() { return m_sneakShowName;  }

		// gun setting
		vector<GunPluginSetting>::type getGunPluginSetting() { return m_gunPluginSetting; }
		void setGunPluginSetting(vector<GunPluginSetting>::type gunPluginSetting);
		void addGunPluginSetting(GunPluginSetting setting);

		//bullet setting
		vector<BulletClipPluginSetting>::type getBulletClipPluginSetting() { return m_bulletClipPluginSetting; }
		void setBulletClipPluginSetting(vector<BulletClipPluginSetting>::type bulletClipPluginSetting);
		void addBulletClipPluginSetting(int bulletId, int maxStack);

		// block dynamic attr
		vector<BlockDynamicAttr>::type getBlockDynamicAttr() { return m_blockDynamicAttr; }
		void setBlockDynamicAttr(vector<BlockDynamicAttr>::type blockDynamicAttr);
		void addBlockDynamicAttr(int blockId, float hardness);
		void setBlockElasticity(int blockId, float elasticity);

		// tool item dynamic attr
		vector<ToolItemDynamicAttr>::type getToolItemDynamicAttr() { return m_toolItemDynamicAttr; }
		void setToolItemDynamicAttr(vector<ToolItemDynamicAttr>::type toolItemDynamicAttr);
		void addToolItemDynamicAttr(int itemId, float distance, float efficiency);

		// break block soon
		void setBreakBlockSoon(bool isSoon) { m_breakBlockSoon = isSoon; }
		bool isBreakBlockSoon() { return m_breakBlockSoon; }

		// can damage item
		void setCanDamageItem(bool canDamageItem) { m_isCanDamageItem = canDamageItem; }
		bool isCanDamageItem() { return m_isCanDamageItem; }

		// need food stats
		void setNeedFoodStats(bool isNeedFoodStats) { m_isNeedFoodStats = isNeedFoodStats; }
		bool isNeedFoodStats() { return m_isNeedFoodStats; }

		//can eat whenever
		void setCanEatWhenever(bool canEatWhenever) { m_canEatWhenever = canEatWhenever; }
		bool isCanEatWhenever() { return m_canEatWhenever; }

		// attack coefficient x
		void setAttackCoefficientX(float x) { m_attackCoefficientX = x; }
		float getAttackCoefficientX() { return m_attackCoefficientX; }

		//Parachute  speed
		float getParachuteDownSpeed() { return parachuteDownSpeed; }
		void setParachuteDownSpeed(float speed) { parachuteDownSpeed = speed > 0 ? speed : parachuteDownSpeed; }
		float getParachuteMoveSpeed() { return parachuteMoveSpeed; }
		void setParachuteMoveSpeed(float speed) { parachuteMoveSpeed = speed > 0 ? speed : parachuteMoveSpeed; }
		bool isDisableSelectEntity() { return m_disableSelectEntity; }
		void setDisableSelectEntity(bool isDisable) { m_disableSelectEntity = isDisable; }

		//isNeedBullet
		void setGunIsNeedBulletStatus(bool isNeedBullet) { m_gunIsNeedBullet = isNeedBullet; }
		bool getGunIsNeedBulletStatus() { return m_gunIsNeedBullet; }

		//monster Setting
		void addMonsterSetting(MonsterSetting setting);
	
		// skill setting
		void addSkillEffect(SkillEffect effect);

		// skillItem setting
		vector<SkillItem>::type getSkillItemSetting() { return m_skillItemSetting; }
		void addSkillItem(SkillItem skillItem);
		void setSkillItemSetting(vector<SkillItem>::type skillItemSetting);
		SkillItem * getSkillItem(int itemId, int Occupation);
		SkillItem * getSkillItem(int skillId);

		//hide clouds
		bool isHideClouds() { return m_hideClouds; }
		void setHideClouds(bool isHide) { this->m_hideClouds = isHide; }

		// food heal
		void setFoodHeal(bool isFoodHeal) { m_isFoodHeal = isFoodHeal; }
		bool isFoodHeal() { return m_isFoodHeal; }

		// entityItem life
		void setEntityItemLife(int life) { m_entityItemLife = life; }
		int getEntityItemLife() { return m_entityItemLife; }

		bool isCanCloseChest() { return m_canCloseChest; }
		void setCanCloseChest(bool closeChest) { this->m_canCloseChest = closeChest; }

		// showGunEffectWithSingle
		void setShowGunEffectWithSingleStatus(bool showGunEffectWithSingle) { m_showGunEffectWithSingle = showGunEffectWithSingle; }
		bool getShowGunEffectWithSingleStatus() { return m_showGunEffectWithSingle; }

		// game type
		void setGameType(ClientGameType gameType) { m_gameType = gameType; }
		ClientGameType getGameType() { return m_gameType; }

		//allow headshot or not
		void setAllowHeadshotStatus(bool isAllowHeadshot) { m_allowHeadshot = isAllowHeadshot; }
		bool getAllowHeadshotStatus() { return m_allowHeadshot; }

		void setAutoShootEnable(bool enable) { m_enableAutoShoot = enable; }
		bool getAutoShootEnable() { return m_enableAutoShoot; }
		
		// set third person distance
		void setThirdPersonDistance(float thirdPersonDistance) {
			if (thirdPersonDistance < 0)
			{
				thirdPersonDistance = 0;
			}
			m_thirdPersonDistance = thirdPersonDistance; 
		}
		float getThirdPersonDistance() { return m_thirdPersonDistance; }

		// set quickly build block cost
		void setQuicklyBuildBlockCost(int cost) { m_quicklyBuildBlockCost = cost; }
		int getQuicklyBuildBlockCost() { return m_quicklyBuildBlockCost; }

		// set quickly build block num
		void setQuicklyBuildBlockNum(int num) { m_quicklyBuildBlockNum = num; }
		int getQuicklyBuildBlockNum() { return m_quicklyBuildBlockNum; }

		// set sword break block
		void setSwordBreakBlock(bool sign) { m_swordCanBreakBlock = sign; }
		bool getSwordBreakBlock() { return m_swordCanBreakBlock; }

		// set tool durable
		void setToolDurable(bool sign) { m_need_durable = sign; }
		bool getToolDurable() { return m_need_durable; }

		// render yaw threshold
		void setRenderYawThreshold(float renderYawThreshold) { m_renderYawThreshold = renderYawThreshold; }
		float getRenderYawThreshold() { return m_renderYawThreshold; }

		// render yaw scale
		void setRenderYawScale(float renderYawScale) { m_renderYawScale = renderYawScale; }
		float getRenderYawScale() { return m_renderYawScale; }

		// set death anim effect
		void setDeathAnimEffect(String deathAnimEffect) { m_deathAnimEffect = deathAnimEffect; }
		String getDeathAnimEffect() { return m_deathAnimEffect; }

		void setEnableIndie(bool isEnableIndie) { m_isEnableIndie = isEnableIndie; }
		bool isEnableIndie() { return m_isEnableIndie; }

		void setUseMovingEnderChest(bool isUseMovingEnderChest) { m_isUseMovingEnderChest = isUseMovingEnderChest; }
		bool isUseMovingEnderChest() { return m_isUseMovingEnderChest; }

		bool canPlayGameSound() { return getGameType() != ClientGameType::PixelGunHall || !isEnableIndie(); }

		// set hide armor
		void setHideArmor(bool hideArmor) { m_hideArmor = hideArmor; }
		bool getHideArmor() { return m_hideArmor; }

		bool checkGameType(ClientGameType gameType);
		bool checkGameType(vector<ClientGameType>::type games);

		void setMaxChangeInChunk(int num) { m_max_changes_in_on_chunk = num; }
		int getMaxChangeInChunk() { return m_max_changes_in_on_chunk; }

		// separate camera control
		void setSeparateCameraControl(bool isSeparateCameraControl) { m_isSeparateCameraControl = isSeparateCameraControl; }
		bool isSeparateCameraControl() { return m_isSeparateCameraControl; }

		// render item in hand with local scale
		void setUseLocalScaleInHand(bool useLocal) { m_useLocalScaleInHand = useLocal; }
		bool getUseLocalScaleInHand() { return m_useLocalScaleInHand; }

		// set the game weather
		void setGameWeather(int gameWeatherType) { m_gameWeatherType = gameWeatherType; }
		int getGameWeather() { return m_gameWeatherType; }

		int getLordPlatform() { return LORD_PLATFORM; }

	};

}

#endif // __LOGIC_SETTINF_HEADER__