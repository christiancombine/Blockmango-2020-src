#include "MonsterSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Memory/LordMemory.h"
#include "Item/Items.h"

namespace BLOCKMAN
{
	std::map<int, MonsterSetting*> MonsterSetting::m_monsterSettingMap = {};
	
	void MonsterSetting::unloadSetting()
	{
		for (auto iter = m_monsterSettingMap.begin(); iter != m_monsterSettingMap.end(); iter++)
		{
			LordSafeDelete(iter->second);
		}
		m_monsterSettingMap.clear();
	}

	MonsterSetting * MonsterSetting::getMonsterSetting(int MonsterId)
	{
		auto iter = m_monsterSettingMap.find(MonsterId);
		if (iter != m_monsterSettingMap.end())
		{
			return iter->second;
		}

		return nullptr;
	}

	void MonsterSetting::addMonsterSetting(MonsterSetting setting)
	{
		MonsterSetting * monsterSetting = getMonsterSetting(setting.monsterId);
		if (!monsterSetting)
		{
			monsterSetting = new MonsterSetting();
			monsterSetting->monsterId = setting.monsterId;
			monsterSetting->monsterType = setting.monsterType;
			m_monsterSettingMap.emplace(std::pair<int, MonsterSetting*>(monsterSetting->monsterId, monsterSetting));
		}
		monsterSetting->isAutoAttack = setting.isAutoAttack;
		monsterSetting->monsterBtTree = setting.monsterBtTree;
		monsterSetting->actorName = setting.actorName;
		monsterSetting->attackDistance = setting.attackDistance;
		monsterSetting->attackCd = setting.attackCd;
		monsterSetting->attackCount = setting.attackCount;
		monsterSetting->moveSpeed = setting.moveSpeed;
		monsterSetting->attackType = setting.attackType;
		monsterSetting->patrolDistance = setting.patrolDistance;
		monsterSetting->skillId = setting.skillId;
		monsterSetting->skillCd = setting.skillCd;
		monsterSetting->viewRange = setting.viewRange;
		monsterSetting->viewDegree = setting.viewDegree;
		monsterSetting->moveSpeedAddition = setting.moveSpeedAddition;
		monsterSetting->canFly = setting.canFly;
		monsterSetting->canEscape = setting.canEscape;
		monsterSetting->patrolType = setting.patrolType;
		monsterSetting->canTogether = setting.canTogether;
		monsterSetting->togetherRange = setting.togetherRange;
		monsterSetting->patrolDistanceOnce = setting.patrolDistanceOnce;
		monsterSetting->patrolCD = setting.patrolCD;
		monsterSetting->alertTime = setting.alertTime;
		monsterSetting->pursueDistance = setting.pursueDistance;
		monsterSetting->deathEffectName = setting.deathEffectName;
		monsterSetting->chaseRange = setting.chaseRange;
		monsterSetting->bombEffect = setting.bombEffect;
        monsterSetting->remoteSpeedFactor = setting.remoteSpeedFactor;
	}

    void MonsterSetting::loadMonsterSetting()
    {
        auto pReader = std::make_shared<CsvReader>();
        auto path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "Monster.csv");
        if (!pReader->LoadFile(path.c_str()))
        {
            LordLogError("MonsterSetting::loadMonsterSetting Failed! %s", path.c_str());
            return;
        }

        pReader->Read();

        while (pReader->Read())
        {
            MonsterSetting* pMonsterSetting = new MonsterSetting();
            pReader->GetInt("id", pMonsterSetting->monsterId);
            pReader->GetFloat("bloodStripOffset", pMonsterSetting->bloodStripOffset);
            m_monsterSettingMap.emplace(std::make_pair(pMonsterSetting->monsterId, pMonsterSetting));
        }
    }
}