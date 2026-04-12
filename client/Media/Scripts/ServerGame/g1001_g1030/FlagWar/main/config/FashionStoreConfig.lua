---
--- Created by Jimmy.
--- DateTime: 2018/6/12 0012 10:33
---
FashionStoreConfig = {}
FashionStoreConfig.fashions = {}

function FashionStoreConfig:init(fashions)
    self:initFashionStore(fashions)
end

function FashionStoreConfig:initFashionStore(fashions)
    for index, fashion in pairs(fashions) do
        local item = {}
        item.id = fashion.id
        item.type = tonumber(fashion.type)
        item.name = fashion.name
        item.image = fashion.image
        item.desc = fashion.desc
        item.price = tonumber(fashion.price)
        local actor = StringUtil.split(fashion.actor, "=")
        item.actor = {}
        item.actor.name = actor[1]
        item.actor.id = actor[2]
        local effect = StringUtil.split(fashion.effect, "#")
        item.effect = {}
        item.effect.id = tonumber(effect[1])
        item.effect.time = 68400
        item.effect.lv = tonumber(effect[2])
        self.fashions[tonumber(item.id)] = item
    end
end

function FashionStoreConfig:getFashion(id)
    for i, fashion in pairs(self.fashions) do
        if fashion.id == id then
            return fashion
        end
    end
    return nil
end

function FashionStoreConfig:getFashions()
    return self.fashions
end

return ArmsConfig