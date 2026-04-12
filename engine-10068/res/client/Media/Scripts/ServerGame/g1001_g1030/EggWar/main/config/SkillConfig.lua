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
        item.itemId = tonumber(skill.ItemId)
        item.atlasName = skill.AtlasName
        item.iconName = skill.IconName
        item.moveSpeed = tonumber(skill.MoveSpeed)
        item.dropSpeed = tonumber(skill.DropSpeed)
        item.distance = tonumber(skill.Distance)
        item.freeCd = tonumber(skill.FreeCd)
        item.length = tonumber(skill.Length)
        item.width = tonumber(skill.Width)
        item.height = tonumber(skill.Height)
        item.duration = tonumber(skill.Duration)
        item.damage = tonumber(skill.Damage)
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
    setting.Parabola = true
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