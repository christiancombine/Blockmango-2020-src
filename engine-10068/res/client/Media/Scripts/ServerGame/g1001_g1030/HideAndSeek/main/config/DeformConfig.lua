---
--- Created by Jimmy.
--- DateTime: 2018/4/9 0009 11:13
---
DeformConfig = {}
DeformConfig.deformPrice = {}

function DeformConfig:init(config)
    self:initDeformPrice(config)
end

function DeformConfig:initDeformPrice(config)
    for i, v in pairs(config) do
        local item = {}
        item.blockymodsPrice = tonumber(v.price[1])
        item.blockmanPrice = tonumber(v.price[2])
        item.times = tonumber(v.times)
        self.deformPrice[i] = item
    end
end

function DeformConfig:getDeformPriceByTimes(times)
    if times > #self.deformPrice then
        return nil
    end
    return self.deformPrice[times]
end

return RespawnConfig