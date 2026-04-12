require "skill.BaseSkill"

EnderPearlSkill = class("EnderPearlSkill", BaseSkill)

function EnderPearlSkill:onPlayerEffected(player)
    if player:ifOnMount() then
        return
    end
    self:telePos(player)
end

return EnderPearlSkill