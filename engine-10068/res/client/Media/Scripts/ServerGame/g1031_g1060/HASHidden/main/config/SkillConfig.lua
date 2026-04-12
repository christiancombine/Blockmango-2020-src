---
--- Created by leilei.
--- DateTime: 2018/6/13 0013 11:00
---
SkillConfig = {}
SkillConfig.skills = {}

function SkillConfig:init(config)
    self:initSkills(config)
end

function SkillConfig:initSkills(config)
    for id, skill in pairs(config) do
        local item = {}
        item.id = id
        item.name = skill.name
        item.icon = skill.icon
        item.itemId = tonumber(skill.itemId)
        item.occupationId = tonumber(skill.occupationId)
        item.atlasName = skill.atlasName
        item.iconName = skill.iconName
        item.moveSpeed = tonumber(skill.moveSpeed)
        item.dropSpeed = tonumber(skill.dropSpeed)
        item.distance = tonumber(skill.distance)
        item.effectId = tonumber(skill.effectId)
        item.freeCd = tonumber(skill.freeCd)
        item.length = tonumber(skill.length)
        item.width = tonumber(skill.width)
        item.height = tonumber(skill.height)
        item.targets = StringUtil.split(skill.targets, "#")
        item.duration = tonumber(skill.duration)
        item.effectType = tonumber(skill.effectType)
        item.damage = tonumber(skill.damage)
        item.useTime = tonumber(skill.useTime)
        self.skills[id] = item
    end
    self:prepareSkills()
end

function SkillConfig:prepareSkills()
    for id, skill in pairs(self.skills) do
        HostApi.addSkillItem(SkillConfig.newSkillItem(skill))
    end
end

function SkillConfig.newSkillItem(skill)
    local setting = SkillItem.new()
    setting.SkillId = tonumber(skill.id)
    setting.Occupation = skill.occupationId
    setting.ItemId = skill.itemId
    setting.ItemSkillCd = skill.freeCd
    setting.ItemSkillDistace = skill.distance
    setting.MoveSpeed = skill.moveSpeed
    setting.DropSpeed = skill.dropSpeed
    setting.AtlasName = skill.atlasName
    setting.ItemIconName = skill.iconName
    return setting
end

function SkillConfig:getSkill(skillId)
    for i, skill in pairs(self.skills) do
        local id = tostring(skillId)
        if i == id then
            return skill
        end
    end
end

function SkillConfig:getSkillById(itemId)
    for i, skill in pairs(self.skills) do
        if skill.itemId == itemId then
            return skill
        end
    end
end

function SkillConfig:ifCanBuy(id, role)
    for i, skill in pairs(self.skills) do
        if skill.itemId == id and skill.occupationId ~= role then
            return true
        end
    end
    return false
end

return SkillConfig