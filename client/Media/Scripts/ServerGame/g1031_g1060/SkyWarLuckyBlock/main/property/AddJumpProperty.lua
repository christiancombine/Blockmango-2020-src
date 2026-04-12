require "property.BaseProperty"

AddJumpProperty = class("AddJumpProperty", BaseProperty)

function AddJumpProperty:onPlayerEquip(player)
    local time = self.config.duration
    if self.config.duration < 0 then
        time = 10000
    end
    player:addEffect(PotionID.JUMP, time, self.config.value)
end

function AddJumpProperty:onPlayerUnload(player)
    player:removeEffect(PotionID.JUMP)
end

return AddJumpProperty