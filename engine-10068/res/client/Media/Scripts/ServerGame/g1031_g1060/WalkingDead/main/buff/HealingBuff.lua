
HealingBuff = class("HealingBuff",BaseBuff)

function HealingBuff:doAction()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    if not player.isLife then
        self:onRemove()
        return
    end
    player:addHealth(self.value)
    player.curHealth = player:getHealth()
end

return HealingBuff