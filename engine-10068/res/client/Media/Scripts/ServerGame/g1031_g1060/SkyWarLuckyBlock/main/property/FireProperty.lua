require "property.BaseProperty"

FireProperty = class("FireProperty", BaseProperty)

function FireProperty:onPlayerUsed(player, target, damage)
    target:setOnFire(self.config.duration)
end

return FireProperty