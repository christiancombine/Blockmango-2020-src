---
--- Created by Jimmy.
--- DateTime: 2018/6/7 0007 11:27
---

CommodityConfig = {}
CommodityConfig.commoditys = {}

function CommodityConfig:init(commoditys)
    self:initCommoditys(commoditys)
end

function CommodityConfig:initCommoditys(commoditys)
    for id, commodity in pairs(commoditys) do
        local item = {}
        item.type = tonumber(commodity.type)
        item.desc = commodity.desc
        if commodity.image == "#" then
            item.image = ""
        else
            item.image = commodity.image
        end
        if commodity.tipDesc == "#" then
            item.tipDesc = ""
        else
            item.tipDesc = commodity.tipDesc
        end
        item.id = tonumber(commodity.id)
        item.meta = tonumber(commodity.meta)
        item.num = tonumber(commodity.num)
        item.coinId = tonumber(commodity.coinId)
        item.price = tonumber(commodity.price)
        self.commoditys[tonumber(id)] = item
    end
    for _, commodity in pairs(self.commoditys) do
        MerchantUtil:addCommodity(1, commodity.type, self:newCommodity(commodity))
    end
end

function CommodityConfig:newCommodity(item)
    local commodity = Commodity.new()
    commodity.itemId = item.id
    commodity.itemMeta = item.meta
    commodity.itemNum = item.num
    commodity.coinId = item.coinId
    commodity.price = item.price
    commodity.desc = item.desc
    commodity.notify = 0
    commodity.image = item.image
    commodity.tipDesc = item.tipDesc
    return commodity
end

return CommodityConfig