require "property.BaseProperty"


BleedProperty = class("BleedProperty", BaseProperty)

function BleedProperty:onPlayerUsed(player, target, damage)
    DeBuffManager:createDeBuff(player, target, self.config)
end

return BleedProperty