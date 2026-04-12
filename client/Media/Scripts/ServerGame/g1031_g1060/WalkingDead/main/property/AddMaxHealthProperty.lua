AddMaxHealthProperty = class("AddMaxHealthProperty", BaseProperty)

function AddMaxHealthProperty:onPlayerEquip(player)
    player:changeMaxHealthSingle(player:getMaxHealth() + self.config.value[1])
end

function AddMaxHealthProperty:onPlayerUnload(player)
    player:changeMaxHealthSingle(player:getMaxHealth() - self.config.value[1])
end

function AddMaxHealthProperty:onPlayerUsed(player,itemId)
    if self.config.probability >= math.random(100) then
        player:changeMaxHealthSingle(player:getMaxHealth() + self.config.value[1])
    end
end

return AddMaxHealthProperty