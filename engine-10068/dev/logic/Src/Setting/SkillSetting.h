#ifndef __SKILL_SETTING_HEADER__
#define __SKILL_SETTING_HEADER__

#include <map>
#include "Core.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;

namespace BLOCKMAN
{
	class SkillEffect
	{
	public:
		int id = 0;
		String name = "";
		float duration = 0.0f;
		int width = 0;
		int height = 0;
		int density = 0;
		int effectType = 0; // 0: particle  1: simple
		Vector3 color = Vector3::ZERO;
	};

	class SkillSetting
	{
	public:
		static void unloadSetting();
		static SkillEffect* getSkillEffect(int skillId);
		static void addSkillEffect(SkillEffect setting);
	private:
		static std::map<int, SkillEffect*> m_skillEffectMap;
	};

	class SkillItem
	{
		public:
			int Occupation = 0;
			int ItemId = 0;
			int SkillId = 0;
			float ItemSkillCd = 0;
			float ItemSkillDistace = 0;
			float MoveSpeed = 0;
			float DropSpeed = 0;
			bool Parabola = false;
			String AtlasName = "";
			String ItemIconName = "";
			bool AutoCast = false;
			bool IsCrossHair = true;
			String EffectName = "";
			bool IsActor = false;
			bool CanFallNoUse = false;
			int CanFallNoUseTip = 0;
			float SkillDuration = 0.f;
			bool IsLongTouch = false;
			DEFINE_NETWORK_SERIALIZER(Occupation, ItemId, SkillId, ItemSkillCd, ItemSkillDistace, MoveSpeed, DropSpeed, Parabola, AtlasName, ItemIconName, AutoCast, IsCrossHair, EffectName, IsActor, CanFallNoUse, SkillDuration, CanFallNoUseTip, IsLongTouch);
	};

	class SkillItemSetting
	{
	public:
		static void unloadSetting();
		static bool isSkillItem(int itemId);
		static bool isAutoCastSkill(int itemId);
		static bool isShowCrossHair(int ItemId);
		static bool isThrowSkillItem(int itemId);
		static bool isLongTouchItem(int itemId);
		static SkillItem* getSkillItem(int itemId,int Occupation);
		static SkillItem* getSkillItem(int skillId);
		static void addSkillItem(SkillItem setting);
		static bool isCanFallNoUse(int ItemId);
		static int getCanFallNoUseTip(int ItemId);
		static void onUpdate();
		static bool canFreeSkill(int entityId, int itemId, int occupation);
		static void freeSkill(int entityId, int itemId, int occupation);
	private:
		static int m_lastTime;
		static std::map<int, SkillItem*> m_skillItemMap;
		static std::map<i64, int> m_freeSkillRecords;
	};
}

#endif