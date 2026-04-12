require "skill.BaseSkill"

BackPearlSkill = class("BackPearlSkill", BaseSkill)

function BackPearlSkill:onPlayerEffected(player)
    if player:ifOnMount() then
        return
    end
    self.oldPos = player:getPosition()
    self:telePos(player)
    player.backPearTimer = LuaTimer:schedule(function (userId, pos)
        local pPlayer = PlayerManager:getPlayerByUserId(userId)
        if pPlayer == nil or pPlayer.isLife == false or pPlayer:ifOnMount() then
            return
        end
        if pPlayer:ifOnMount() then
            return
        end
        if (pPlayer:getPosition().y - pos.y < 0) then
            pPlayer.noFallHurt = true
        end
        pPlayer:teleportPos(VectorUtil.newVector3(pos.x, pos.y, pos.z))
    end, self.duration * 1000, nil, player.userId, self.oldPos)
end

return BackPearlSkill