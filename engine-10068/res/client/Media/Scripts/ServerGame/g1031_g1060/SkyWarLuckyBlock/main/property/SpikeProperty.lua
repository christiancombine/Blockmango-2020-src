require "util.RandomUtil"
require "property.BaseProperty"

SpikeProperty = class("SpikeProperty", BaseProperty)

function SpikeProperty:onCreate()
    RandomUtil:setRandomSeed()
    local data = {}
    table.insert(data, self.config.value / 100)
    table.insert(data, math.max((100 - self.config.value) / 100, 0))
    RandomUtil:initRandomSeed(self.config.id, data)
end

function SpikeProperty:onPlayerUsed(player, target, damage)
    local random = RandomUtil:get(self.config.id)
    if random == nil then
        random = 0
    end
    if random > 1 then
        return
    end
    damage.value = 99999
end

return SpikeProperty