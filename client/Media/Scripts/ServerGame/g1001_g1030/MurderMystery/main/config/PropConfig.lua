---
--- Created by Jimmy.
--- DateTime: 2017/11/15 0015 15:12
---
PropConfig = class()

function PropConfig:ctor(config)
    self.props = {}
    self:init(config)
end

function PropConfig:init(config)
    for i, v in pairs(config) do
        local key = tostring(v.id)
        self.props[key] = {}
        self.props[key].id = tonumber(v.id)
        self.props[key].hurt = tonumber(v.hurt)
    end
end

function PropConfig:getArmsHurt(id)
    local prop = self.props[tostring(id)]
    if prop ~= nil then
        return prop.hurt
    end
    return 0
end

return PropConfig