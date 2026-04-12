
FractureBuff = class("FractureBuff", BaseBuff)

function FractureBuff:onCreate()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    GamePacketSender:sendWalkingDeadCanJump(player.rakssid, false)
    player:changeSpeed(self.value)
end

function FractureBuff:onDestroy()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    GamePacketSender:sendWalkingDeadCanJump(player.rakssid, true)
    player:changeSpeed(-self.value)
end

return FractureBuff
