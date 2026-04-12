require "property.BaseProperty"

MaxHealthProperty = class("MaxHealthProperty", BaseProperty)

function MaxHealthProperty:onPlayerGet(player)
    player.maxHealth = player.maxHealth + self.config.value
    player:changeMaxHealth(player.maxHealth)
end

function MaxHealthProperty:onPlayerRemove(player)
    player.maxHealth = player.maxHealth - self.config.value
    player:changeMaxHealth(player.maxHealth)
end

return MaxHealthProperty