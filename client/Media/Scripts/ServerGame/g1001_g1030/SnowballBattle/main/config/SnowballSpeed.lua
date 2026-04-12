---
--- Created by Jimmy.
--- DateTime: 2017/10/30 0030 10:58
---
SnowballSpeed = {}
SnowballSpeed.speed = {}

function SnowballSpeed:init(snowballSpeed)
    self.speed["0"] = self:getSpeed(snowballSpeed.spp0)
    self.speed["1"] = self:getSpeed(snowballSpeed.spp1)
    self.speed["2"] = self:getSpeed(snowballSpeed.spp2)
    self:sort()
end

function SnowballSpeed:getSnowballCount(spp, time)
    local speed = self.speed[tostring(spp)]
    if speed == nil then
        speed = self.speed["0"]
    end
    for i, v in pairs(speed.items) do
        if time >= v.second then
            return v.count
        end
    end
    return 1
end

function SnowballSpeed:sort()
    for i, v in pairs(self.speed) do
        table.sort(v, self.compSpeedItem)
    end
end

function SnowballSpeed.compSpeedItem(a, b)
    return a.second < -b.second
end

function SnowballSpeed:getSpeed(speedList)
    local speed = {}
    speed.items = {}
    local index = 1
    for i, v in pairs(speedList) do
        local item = {}
        item.second = tonumber(v[1])
        item.count = tonumber(v[2])
        speed.items[index] = item
        index = index + 1
    end
    return speed
end

return SnowballSpeed