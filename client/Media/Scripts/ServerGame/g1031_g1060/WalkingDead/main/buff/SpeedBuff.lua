
SpeedBuff = class("SpeedBuff",BaseBuff)

function SpeedBuff:onCreate()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    player:changeSpeed(self.value)
end

function SpeedBuff:onDestroy()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    player:changeSpeed(-self.value)
end

return SpeedBuff