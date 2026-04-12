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
        local group = {}
        local groupId = commodity.groupId
        if self.commoditys[groupId] ~= nil then
            group = self.commoditys[groupId]
        end
        local item = {}
        item.type = tonumber(commodity.type)
        item.desc = commodity.desc
        if commodity.image == "#" then
            item.image = ""
        else
            item.image = commodity.image
        end
        item.id = tonumber(commodity.id)
        item.meta = tonumber(commodity.meta)
        item.num = tonumber(commodity.num)
        item.coinId = tonumber(commodity.coinId)
        item.price = tonumber(commodity.price)
        group[#group + 1] = item
        self.commoditys[groupId] = group
        MerchantUtil:addCommodity(tonumber(groupId), item.type, self:newCommodity(item))
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
    return commodity
end

return CommodityConfig