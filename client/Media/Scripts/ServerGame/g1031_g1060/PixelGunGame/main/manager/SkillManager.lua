---
--- Created by Jimmy.
--- DateTime: 2018/6/13 0013 12:00
---
require "skill.AddHealthSkill"
require "skill.AddDefenseSkill"
require "skill.AddSpeedSkill"
require "skill.AddAttackSkill"
require "skill.AddHeadShotSkill"
require "skill.GrenadeSkill"
require "skill.SubSpeedSkill"
require "skill.InvisibleSkill"
require "skill.FlySkill"
require "skill.AdrenalineSkill"
require "skill.LandmineSkill"
require "skill.SnowstormSkill"
require "skill.SwordGasSkill"

SkillManager = {}
SkillManager.SkillType = {
    AddHealth = "1", --加血
    AddDefense = "2", --加防
    AddSpeed = "3", --加速
    AddAttack = "4", --加攻击
    AddHeadShot = "5", --加爆头
    Grenade = "6", --手雷
    SubSpeed = "7", --减速
    Invisible = "8", --隐身
    Fly = "9", --飞行
    Adrenaline = "10", --肾上腺素
    Landmine = "11", --伪装地雷
    Snowstorm = "12", --暴风雪
    SwordGas = "13", --剑气
}
SkillManager.SkillMapping = {}
SkillManager.SkillMapping[SkillManager.SkillType.AddHealth] = AddHealthSkill
SkillManager.SkillMapping[SkillManager.SkillType.AddDefense] = AddDefenseSkill
SkillManager.SkillMapping[SkillManager.SkillType.AddSpeed] = AddSpeedSkill
SkillManager.SkillMapping[SkillManager.SkillType.AddAttack] = AddAttackSkill
SkillManager.SkillMapping[SkillManager.SkillType.AddHeadShot] = AddHeadShotSkill
SkillManager.SkillMapping[SkillManager.SkillType.Grenade] = GrenadeSkill
SkillManager.SkillMapping[SkillManager.SkillType.SubSpeed] = SubSpeedSkill
SkillManager.SkillMapping[SkillManager.SkillType.Invisible] = InvisibleSkill
SkillManager.SkillMapping[SkillManager.SkillType.Fly] = FlySkill
SkillManager.SkillMapping[SkillManager.SkillType.Adrenaline] = AdrenalineSkill
SkillManager.SkillMapping[SkillManager.SkillType.Landmine] = LandmineSkill
SkillManager.SkillMapping[SkillManager.SkillType.Snowstorm] = SnowstormSkill
SkillManager.SkillMapping[SkillManager.SkillType.SwordGas] = SwordGasSkill

SkillManager.HashCode = 1
SkillManager.Skills = {}

function SkillManager:createSkill(config, player, position)
    local skill = SkillManager.SkillMapping[config.type]
    if skill ~= nil then
        skill.new(config, player, position)
    end
end

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