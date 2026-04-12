---
--- Created by Jimmy.
--- DateTime: 2018/6/13 0013 12:00
---
SkillManager = {}
SkillManager.HashCode = 1
SkillManager.Skills = {}

function SkillManager:addSkill(skill)
    local uniqueId = tostring(self.HashCode)
    skill.uniqueId = uniqueId
    self.Skills[uniqueId] = skill
    self.HashCode = self.HashCode + 1
end

function SkillManager:onTick(tick)
    for uniqueId, skill in pairs(self.Skills) do
        skill:onTick(tick)
    end
end

function SkillManager:removeSkill(skill)
    if skill.uniqueId then
        self.Skills[skill.uniqueId] = nil
    end
end

function SkillManager:clearSkill()
    for i, skill in pairs(self.Skills) do
        skill = nil
    end
    self.Skills = {}
    self.HashCode = 1
end

return SkillManager