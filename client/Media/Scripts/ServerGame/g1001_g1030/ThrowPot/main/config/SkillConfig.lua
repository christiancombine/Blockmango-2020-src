---
--- Created by Jimmy.
--- DateTime: 2018/7/18 0018 17:35
---
SkillConfig = {}
SkillConfig.skills = {}

function SkillConfig:init(skills)
    self:initSkills(skills)
end

function SkillConfig:initSkills(skills)
    for id, skill in pairs(skills) do
        local item = {}
        item.id = skill.id
        item.itemId = tonumber(skill.itemId)
        item.tip = tonumber(skill.tip)
        self.skills[tonumber(id)] = item
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
    setting.Occupation = 1
    setting.ItemId = item.itemId
    setting.ItemSkillCd = 1
    setting.ItemSkillDistace = 1
    setting.MoveSpeed = 1
    setting.DropSpeed = 1
    setting.AtlasName = ""
    setting.ItemIconName = ""
    return setting
end

function SkillConfig:getSkill(id)
    return self.skills[tonumber(id)]
end

function SkillConfig:getSkills()
    return self.skills
end

function SkillConfig:getSkillByItemId(itemId)
    for id, skill in pairs(self.skills) do
        if skill.itemId == itemId then
            return skill
        end
    end
    return nil
end

return SkillConfig