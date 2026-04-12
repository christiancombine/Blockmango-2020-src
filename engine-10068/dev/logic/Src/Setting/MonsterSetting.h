#pragma once
#include <map>
#include "Core.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;
namespace BLOCKMAN
{
	class MonsterSetting
	{
	public:
		static void unloadSetting();
		static MonsterSetting* getMonsterSetting(int monsterId);
		static void addMonsterSetting(MonsterSetting setting);
        static void loadMonsterSetting();

		int					monsterId = 0;
		String		        monsterBtTree = "";
		String				actorName = "";
		int					monsterType = 0;
		int					isAutoAttack = 0;
		float				attackDistance = 0.0f;
		float				attackCd = 0.0f;
		int					attackCount = 2;
		int					moveSpeed = 0;
		int					attackType = 0;
		float				patrolDistance = 0.0f;
		int					skillId = 0;
		float				skillCd = 0;
		String				bombEffect = ""; //远程攻击效果
		int					chaseRange = 0; //追击范围
        float				remoteSpeedFactor{1.0F}; //远程攻击弹道
        float				bloodStripOffset{ 0.0F }; //血条偏移

		//G1049添加
		float               viewRange = 0.0; //视野范围
		float               viewDegree = 0.0; //视野角度
		float               moveSpeedAddition = 0.0; //奔跑速度增量
		bool                canFly = false; //是否飞行
		bool                canEscape = false; //能否逃跑
		int                 patrolType = 0; //巡逻类型
		bool                canTogether = false; //能否召集同伴
		float               togetherRange = 0.0; //召集范围
		float               patrolDistanceOnce = 0.0; //每次巡逻移动距离
		float               patrolCD = 0; //巡逻CD 毫秒
		float               alertTime = 0.0; //警戒时间。
		float               pursueDistance = 0.0; //追击范围
		LORD::String        deathEffectName = "";


	private:
		static std::map<int, MonsterSetting*> m_monsterSettingMap;
	};
}
