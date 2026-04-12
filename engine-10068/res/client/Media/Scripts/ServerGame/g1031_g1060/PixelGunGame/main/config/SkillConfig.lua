---
--- Created by Jimmy.
--- DateTime: 2018/6/13 0013 11:00
---
SkillConfig = {}
SkillConfig.skills = {}

function SkillConfig:init(skills)
    self:initSkills(skills)
end

function SkillConfig:initSkills(skills)
    for id, skill in pairs(skills) do
        local item = {}
        item.id = id
        item.index = tonumber(id)
        item.itemId = tonumber(skill.itemId)
        item.moveSpeed = tonumber(skill.moveSpeed)
        item.dropSpeed = tonumber(skill.dropSpeed)
        item.distance = tonumber(skill.distance)
        item.atlasName = skill.atlasName
        item.iconName = skill.iconName
        item.effectName = skill.effectName
        item.hitEffectName = skill.hitEffectName
        item.hitEffectTime = tonumber(skill.hitEffectTime)
        item.freeCd = tonumber(skill.freeCd)
        item.length = tonumber(skill.length)
        item.width = tonumber(skill.width)
        item.height = tonumber(skill.height)
        item.targets = StringUtil.split(skill.targets, "#")
        item.effectNum = tonumber(skill.effectNum)
        item.isMoment = tonumber(skill.isMoment or "0") == 1
        item.duration = tonumber(skill.duration)
        item.effectCd = tonumber(skill.effectCd)
        item.type = skill.type
        item.damage = tonumber(skill.damage)
        self.skills[id] = item
    end
    self:prepareSkills()
end

function SkillConfig:prepareSkills()
    for id, skill in pairs(self.skills) do
        HostApi.addSkillItem(SkillConfig.newSkillItem(skill))
    end
end

function SkillConfig.newSkillItem(item)
    local setting = SkillItem.new()
    setting.SkillId = tonumber(item.id)
    setting.Occupation = 0
    setting.ItemId = item.itemId
    setting.ItemSkillCd = item.freeCd
    setting.ItemSkillDistace = item.distance
    setting.MoveSpeed = item.moveSpeed
    setting.DropSpeed = item.dropSpeed
    setting.AtlasName = item.atlasName
    setting.ItemIconName = item.iconName
    setting.EffectName = item.effectName
    setting.Parabola = item.dropSpeed > 0
    return setting
end

function SkillConfig:getSkill(itemId)
    for id, skill in pairs(self.skills) do
        if skill.itemId == itemId then
            return skill
        end
    end
    return nil
end

return SkillConfig