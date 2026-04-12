---
--- Created by Jimmy.
--- DateTime: 2018/4/27 0027 16:29
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
        item.name = skill.name
        item.duration = tonumber(skill.duration)
        item.width = tonumber(skill.width)
        item.height = tonumber(skill.height)
        item.density = tonumber(skill.density)
        local color = StringUtil.split(skill.color or "1#1#1", "#")
        item.color = VectorUtil.newVector3(color[1] or "1", color[2] or "1", color[3] or "1")
        self.skills[id] = item
    end
    self:prepareSkills()
end

function SkillConfig:prepareSkills()
    for id, skill in pairs(self.skills) do
        HostApi.addSkillEffect(SkillConfig.newSkillEffect(skill))
    end
end

function SkillConfig.newSkillEffect(item)
    local setting = SkillEffect.new()
    setting.id = item.id
    setting.name = item.name
    setting.duration = item.duration
    setting.width = item.width
    setting.height = item.height
    setting.density = item.density
    setting.color = item.color
    return setting
end

return SkillConfig