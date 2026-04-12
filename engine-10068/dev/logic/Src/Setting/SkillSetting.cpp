#include "SkillSetting.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, SkillEffect*> SkillSetting::m_skillEffectMap = {};
	std::map<int, SkillItem*> SkillItemSetting::m_skillItemMap = {};
	std::map<i64, int> SkillItemSetting::m_freeSkillRecords = {};
	int SkillItemSetting::m_lastTime = 0;

	void SkillSetting::unloadSetting()
	{
		for (auto iter = m_skillEffectMap.begin(); iter != m_skillEffectMap.end(); iter++)
		{
			SkillEffect* pSkillEffect = iter->second;
			delete pSkillEffect;
		}

		m_skillEffectMap.clear();
	}

	SkillEffect * SkillSetting::getSkillEffect(int id)
	{
		auto iter = m_skillEffectMap.find(id);
		if (iter != m_skillEffectMap.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	void SkillSetting::addSkillEffect(SkillEffect skillEffect)
	{
		SkillEffect * pSkillEffect = getSkillEffect(skillEffect.id);
		if (!pSkillEffect)
		{
			pSkillEffect = new SkillEffect();
			pSkillEffect->id = skillEffect.id;
			m_skillEffectMap.insert(std::pair<int, SkillEffect*>(pSkillEffect->id, pSkillEffect));
		}
		pSkillEffect->name = skillEffect.name;
		pSkillEffect->duration = skillEffect.duration;
		pSkillEffect->width = skillEffect.width;
		pSkillEffect->height = skillEffect.height;
		pSkillEffect->density = skillEffect.density;
		pSkillEffect->color = skillEffect.color;
		pSkillEffect->effectType = skillEffect.effectType;
	}

	void SkillItemSetting::unloadSetting()
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			SkillItem* pSkillItem = iter->second;
			delete pSkillItem;
		}
		m_skillItemMap.clear();
		m_freeSkillRecords.clear();
	}

	bool SkillItemSetting::isSkillItem(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				return iter->second->ItemId == itemId;
			}
		}
		return false;
	}

	bool SkillItemSetting::isAutoCastSkill(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				return iter->second->AutoCast;
			}
		}
		return false;
	}

	bool SkillItemSetting::isShowCrossHair(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				return iter->second->IsCrossHair;
			}
		}
		return true;
	}

	bool SkillItemSetting::isThrowSkillItem(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				return iter->second->MoveSpeed > 0 || iter->second->DropSpeed > 0;
			}
		}
		return false;
	}

	bool SkillItemSetting::isLongTouchItem(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				return iter->second->IsLongTouch;
			}
		}
		return false;
	}

	SkillItem * SkillItemSetting::getSkillItem(int itemId, int Occupation)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId && iter->second->Occupation == Occupation)
			{
				return iter->second;
			}
		}
		return nullptr;
	}

	SkillItem * SkillItemSetting::getSkillItem(int skillId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->SkillId == skillId)
			{
				return iter->second;
			}
		}
		return nullptr;
	}

	void SkillItemSetting::addSkillItem(SkillItem skillItem)
	{
		SkillItem * pSkillItem = getSkillItem(skillItem.ItemId,skillItem.Occupation);
		if (!pSkillItem)
		{
			pSkillItem = new SkillItem();
			pSkillItem->SkillId = skillItem.SkillId;
			m_skillItemMap.insert(std::pair<int, SkillItem*>(pSkillItem->SkillId, pSkillItem));
		}
		pSkillItem->Occupation = skillItem.Occupation;
		pSkillItem->ItemId = skillItem.ItemId;
		pSkillItem->ItemSkillCd = skillItem.ItemSkillCd;
		pSkillItem->ItemSkillDistace = skillItem.ItemSkillDistace;
		pSkillItem->MoveSpeed = skillItem.MoveSpeed;
		pSkillItem->DropSpeed = skillItem.DropSpeed;
		pSkillItem->AtlasName = skillItem.AtlasName;
		pSkillItem->ItemIconName = skillItem.ItemIconName;
		pSkillItem->Parabola = skillItem.Parabola;
		pSkillItem->AutoCast = skillItem.AutoCast;
		pSkillItem->IsCrossHair = skillItem.IsCrossHair;
		pSkillItem->EffectName = skillItem.EffectName;
		pSkillItem->IsActor = skillItem.IsActor;
		pSkillItem->CanFallNoUse = skillItem.CanFallNoUse;
		pSkillItem->SkillDuration = skillItem.SkillDuration;
		pSkillItem->CanFallNoUseTip = skillItem.CanFallNoUseTip;
		pSkillItem->IsLongTouch = skillItem.IsLongTouch;
	}

	bool SkillItemSetting::isCanFallNoUse(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				return iter->second->CanFallNoUse;
			}
		}
		return false;
	}

	int SkillItemSetting::getCanFallNoUseTip(int itemId)
	{
		for (auto iter = m_skillItemMap.begin(); iter != m_skillItemMap.end(); iter++)
		{
			if (iter->second->ItemId == itemId)
			{
				return iter->second->CanFallNoUseTip;
			}
		}
		return 0;
	}

	void SkillItemSetting::onUpdate()
	{
		int currentTime = Root::Instance()->getCurrentTime();
		int duration = currentTime - m_lastTime;
		for (auto iter = m_freeSkillRecords.begin(); iter != m_freeSkillRecords.end(); )
		{
			iter->second -= duration;
			if (iter->second <= 0)
				iter = m_freeSkillRecords.erase(iter);
			else
				++iter;
		}
		m_lastTime = Root::Instance()->getCurrentTime();
	}

	bool SkillItemSetting::canFreeSkill(int entityId, int itemId, int occupation)
	{
		SkillItem* skill = getSkillItem(itemId, occupation);
		if (skill)
		{
			i64 param1 = (i64)entityId;
			i64 param2 = (i64)itemId;
			i64 param3 = (i64)occupation;
			i64 key = (param1 << 32) + (param2 << 16) + param3;
			return m_freeSkillRecords.find(key) == m_freeSkillRecords.end();
		}
		return false;
	}

	void SkillItemSetting::freeSkill(int entityId, int itemId, int occupation)
	{
		SkillItem* skill = getSkillItem(itemId, occupation);
		if (skill)
		{
			i64 param1 = (i64)entityId;
			i64 param2 = (i64)itemId;
			i64 param3 = (i64)occupation;
			i64 key = (param1 << 32) + (param2 << 16) + param3;
			m_freeSkillRecords[key] = (int)(skill->ItemSkillCd * 800);
		}
	}

}