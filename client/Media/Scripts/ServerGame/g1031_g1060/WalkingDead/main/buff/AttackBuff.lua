
AttackBuff = class("AttackBuff",BaseBuff)

function AttackBuff:onCteate()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    --player.addDamage = player.addDamage + self.value
end

function AttackBuff:onDestroy()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    --player.addDamage = player.addDamage - self.value
end

return AttackBuff