---
--- Created by Jimmy.
--- DateTime: 2018/6/11 0011 16:29
---
ArmsConfig = {}
ArmsConfig.arms = {}

function ArmsConfig:init(arms)
    self:initArms(arms)
end

function ArmsConfig:initArms(arms)
    for index, c_item in pairs(arms) do
        local item = {}
        item.itemId = tonumber(c_item.itemId)
        item.damage = tonumber(c_item.damage)
        local effect = StringUtil.split(c_item.effect, "#")
        item.effect = {}
        item.effect.id = tonumber(effect[1])
        item.effect.time = tonumber(effect[2])
        item.effect.lv = tonumber(effect[3])
        self.arms[tostring(item.itemId)] = item
    end
end

function ArmsConfig:getArms(itemId)
    return self.arms[tostring(itemId)]
end

return ArmsConfig