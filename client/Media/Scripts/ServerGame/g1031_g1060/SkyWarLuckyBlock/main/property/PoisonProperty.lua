require "property.BaseProperty"

PoisonProperty = class("PoisonProperty", BaseProperty)

function PoisonProperty:onPlayerUsed(player, target, damage)
    target:addEffect(PotionID.POISON, self.config.duration, self.config.value)
end

return PoisonProperty