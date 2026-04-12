---
--- Created by Jimmy.
--- DateTime: 2018/3/3 0003 14:47
---

KeepItemConfig = {}
KeepItemConfig.keepGoods = {}

function KeepItemConfig:init(config)
    self:initkeepGoods(config)
end

function KeepItemConfig:initkeepGoods(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.coinId = tonumber(v.CoinId)
        item.price = tonumber(v.Price)
        item.times = tonumber(v.Times)
        item.tipTime = tonumber(v.TipTime)
        ---复活倒计时改为服务器计算,为了还原表现 秒数+1
        item.tipTime = item.tipTime + 1
        self.keepGoods[item.id] = item
    end
end

function KeepItemConfig:getKeepItemByTimes(times)
    if times >= #self.keepGoods then
        return self.keepGoods[#self.keepGoods]
    end
    for k, v in pairs(self.keepGoods) do
        if v.times == times then
            return v
        end
    end
end

return KeepItemConfig