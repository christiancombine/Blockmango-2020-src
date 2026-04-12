
DrowningBuff = class("DrowningBuff", BaseBuff)

function DrowningBuff:doAction()
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

return DrowningBuff