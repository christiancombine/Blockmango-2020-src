require "property.BaseProperty"

FrozenProperty = class("FrozenProperty", BaseProperty)

function FrozenProperty:onPlayerUsed(player, target, damage)
    target:setOnFrozen(self.config.duration)
end

return FrozenProperty
