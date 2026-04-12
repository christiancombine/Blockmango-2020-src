
ThirstBuff = class("ThirstBuff", BaseBuff)

function ThirstBuff:onCreate()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    local maxHealth = GameConfig.PlayerAttribute.MaxHealth
    if maxHealth - self.value < player:getHealth() then
        player.curHealth = player:getHealth() - self.value
        player:subHealth(player:getHealth() - (maxHealth - self.value))
    end
    player:changeMaxHealthSingle(maxHealth - self.value)
end

function ThirstBuff:onDestroy()
    local player = PlayerManager:getPlayerByUserId(self.targetUid)
    if not player then
        return
    end
    player:changeMaxHealthSingle(GameConfig.PlayerAttribute.MaxHealth)
end

return ThirstBuff