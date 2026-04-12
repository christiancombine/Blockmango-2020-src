require "property.BaseProperty"

DisarmProperty = class("DisarmProperty", BaseProperty)

function DisarmProperty:onPlayerUsed(player, target, damage)
    DeBuffManager:createDeBuff(player, target, self.config)
end

return DisarmProperty
