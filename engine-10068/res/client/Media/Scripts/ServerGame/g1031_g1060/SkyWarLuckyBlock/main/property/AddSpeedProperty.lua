require "property.BaseProperty"

AddSpeedProperty = class("AddSpeedProperty", BaseProperty)

function AddSpeedProperty:onPlayerEquip(player)
    local time = self.config.duration
    if self.config.duration < 0 then
        time = 10000
    end
    player:addEffect(PotionID.MOVE_SPEED, time, self.config.value)
end

function AddSpeedProperty:onPlayerUnload(player)
    player:removeEffect(PotionID.MOVE_SPEED)
end

return AddSpeedProperty