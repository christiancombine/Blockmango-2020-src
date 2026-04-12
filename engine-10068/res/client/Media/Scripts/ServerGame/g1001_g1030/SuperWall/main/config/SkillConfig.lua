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
        item.name = skill.name
        item.image = skill.image
        item.desc = skill.desc
        item.itemId = tonumber(skill.itemId)
        item.occupationId = tonumber(skill.occupationId)
        item.moveSpeed = tonumber(skill.moveSpeed)
        item.dropSpeed = tonumber(skill.dropSpeed)
        item.distance = tonumber(skill.distance)
        item.atlasName = skill.atlasName
        item.iconName = skill.iconName
        item.effectId = tonumber(skill.effectId)
        item.freeCd = tonumber(skill.freeCd)
        item.length = tonumber(skill.length)
        item.width = tonumber(skill.width)
        item.height = tonumber(skill.height)
        item.targets = StringUtil.split(skill.targets, "#")
        item.effectNum = tonumber(skill.effectNum)
        item.isMoment = tonumber(skill.isMoment or "0") == 1
        item.duration = tonumber(skill.duration)
        item.effectCd = tonumber(skill.effectCd)
        item.effectType = tonumber(skill.effectType)
        item.damage = tonumber(skill.damage)
        local effect = StringUtil.split(skill.effect or "0#0#0", "#")
        item.effect = {}
        item.effect.id = tonumber(effect[1])
        item.effect.time = tonumber(effect[2])
        item.effect.lv = tonumber(effect[3])
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
    setting.Occupation = item.occupationId
    setting.ItemId = item.itemId
    setting.ItemSkillCd = item.freeCd
    setting.ItemSkillDistace = item.distance
    setting.MoveSpeed = item.moveSpeed
    setting.DropSpeed = item.dropSpeed
    setting.AtlasName = item.atlasName
    setting.ItemIconName = item.iconName
    return setting
end

function SkillConfig:getSkill(itemId, occupationId)
    for id, skill in pairs(self.skills) do
        if skill.itemId == itemId and skill.occupationId == occupationId then
            return skill
        end
    end
    return nil
end

function SkillConfig:getSkillsByOccupationId(occupationId)
    local skills = {}
    for id, skill in pairs(self.skills) do
        if skill.occupationId == occupationId then
            skills[#skills + 1] = skill
        end
    end
    table.sort(skills, function(a, b)
        return a.index < b.index
    end)
    return skills
end

return SkillConfig