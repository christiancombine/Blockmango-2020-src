
SatietyBuff = class("SatietyBuff",BaseBuff)

function SatietyBuff:doAction()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    if not player.isLife then
        self:onRemove()
        return
    end
    --player:addHealth(self.value)
end

return SatietyBuff