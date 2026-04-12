require "skill.BaseSkill"

DropArrowSkill = class("DropArrowSkill", BaseSkill)

function DropArrowSkill:onPlayerEffected(player)
    local effect = SkillEffectConfig:getSkillEffect(self.itemId)
    if effect == nil then
        return
    end
    if player:onMountHunt(tonumber(effect.value)) then
        return
    end
    player:subHealth(tonumber(effect.value))
end

return DropArrowSkill