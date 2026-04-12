require "property.BaseProperty"

SubSpeedProperty = class("SubSpeedProperty", BaseProperty)

function SubSpeedProperty:onPlayerUsed(player, target, damage)
    local time = self.config.duration
    if self.config.duration < 0 then
        time = 10000
    end
    target:addEffect(PotionID.MOVE_SLOWDOWN, time, self.config.value)
end

return SubSpeedProperty