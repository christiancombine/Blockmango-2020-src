
InfectionBuff = class("InfectionBuff",BaseBuff)

function InfectionBuff:onCreate()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    player.infection = self.limit
end

function InfectionBuff:doAction()
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

function InfectionBuff:onDestroy()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    player.infection = 0
end

return InfectionBuff