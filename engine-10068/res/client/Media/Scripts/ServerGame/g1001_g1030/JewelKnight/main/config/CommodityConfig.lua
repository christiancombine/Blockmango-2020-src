
CommodityConfig = {}
CommodityConfig.commoditys = {}

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
        item.id = tonumber(commodity.id)
        item.meta = tonumber(commodity.meta)
        item.num = tonumber(commodity.num)
        item.coinId = tonumber(commodity.coinId)
        item.price = tonumber(commodity.price)
        item.groupId = tonumber(commodity.groupId)
        self.commoditys[tonumber(id)] = item
    end

    for i, commodity in pairs(self.commoditys) do
        MerchantUtil:addCommodity(commodity.groupId, commodity.type, self:newCommodity(commodity))
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