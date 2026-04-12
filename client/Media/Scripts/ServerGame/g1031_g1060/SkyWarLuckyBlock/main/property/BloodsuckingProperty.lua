require "property.BaseProperty"

BloodsuckingProperty = class("BloodsuckingProperty", BaseProperty)

function BloodsuckingProperty:onPlayerUsed(player, target, damage)
    local health = damage.value * self.config.value / 100
    player:addHealth(health)
end

return BloodsuckingProperty