
FireBuff = class("FireBuff", BaseBuff)

function FireBuff:onCreate()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    player:setOnFire(GameConfig.FireDurationTime)
end
return FireBuff