
PoisoningBuff = class("PoisoningBuff", BaseBuff)

function PoisoningBuff:doAction()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    if not player.isLife then
        self:onRemove()
        return
    end
    local currenthp = player:getHealth()
    if((currenthp - self.value) > self.limit) then
        player.curHealth = player:getHealth() - self.value
        player:subHealth(self.value)
    end
end

return PoisoningBuff