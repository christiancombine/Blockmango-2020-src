---
--- Created by Jimmy.
--- DateTime: 2018/1/29 0029 11:16
---
require "config.Area"

LocationConfig = {}

LocationConfig.POLICE = 1
LocationConfig.JAIL = 2

LocationConfig.locations = {}

function LocationConfig:init(config)
    self:initLocation(config.location)
end

function LocationConfig:initLocation(config)
    for i, item in pairs(config) do
        local location = {}
        location.id = tonumber(item.id)
        location.name = tonumber(item.name)
        location.area = Area.new(item.area)
        self.locations[i] = location
    end
end

return LocationConfig