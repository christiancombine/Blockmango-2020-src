
---
--- Created by Jimmy.
--- DateTime: 2018/3/3 0003 14:47
---

RespawnConfig = {}
RespawnConfig.respawnGoods = {}

function RespawnConfig:init(config)
    self:initRespawnGoods(config)
end

function RespawnConfig:initRespawnGoods(config)
    for i, v in pairs(config) do
        local item = {}
        item.time = tonumber(v.time)
        item.coinId = tonumber(v.price[1])
        item.blockymodsPrice = tonumber(v.price[2])
        item.blockmanPrice = tonumber(v.price[3])
        item.times = tonumber(v.times)
        self.respawnGoods[i] = item
    end
end

function RespawnConfig:prepareRespawnGoods()
    for i, goods in pairs(self.respawnGoods) do
        EngineWorld:getWorld():addRespawnGoodsToShop(self:newRespawnGoods(goods))
    end
end

function RespawnConfig:newRespawnGoods(item)
    local goods = RespawnGoods.new()
    goods.time = item.time
    goods.coinId = item.coinId
    goods.blockymodsPrice = item.blockymodsPrice
    goods.blockmanPrice = item.blockmanPrice
    goods.times = item.times
    return goods
end

function RespawnConfig:showBuyRespawn(rakssId, times)
    if times >= #self.respawnGoods then
        return false
    end
    HostApi.showBuyRespawn(rakssId, times)
    return true
end

return RespawnConfig