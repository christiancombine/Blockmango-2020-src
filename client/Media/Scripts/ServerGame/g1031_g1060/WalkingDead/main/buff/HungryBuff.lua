
HungryBuff = class("HungryBuff", BaseBuff)

function HungryBuff:onCreate()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    player:changeSpeed(self.limit)
end

function HungryBuff:doAction()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    if not player.isLife then
        self:onRemove()
        return
    end
    player.curHealth = player:getHealth() - self.value
    player:subHealth(self.value)
end

function HungryBuff:onDestroy()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    player:changeSpeed(-self.limit)
end

return HungryBuff