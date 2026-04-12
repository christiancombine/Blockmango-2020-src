---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---
require "data.GameMerchant"


MerchantConfig = {}

MerchantConfig.coinMapping = {}
MerchantConfig.merchants = {}

function MerchantConfig:init(config)
    self:initCoinMapping(config.coinMapping)
    self:initStores(config.store)
end

function MerchantConfig:initCoinMapping(coinMapping)
    for i, v in pairs(coinMapping) do
        self.coinMapping[i] = {}
        self.coinMapping[i].coinId = v.coinId
        self.coinMapping[i].itemId = v.itemId
    end
end

function MerchantConfig:initStores(store)
    for i = 1, #store.initPos do
        local initPos = VectorUtil.newVector3(store.initPos[i].pos[1], store.initPos[i].pos[2], store.initPos[i].pos[3])
        local storeYaw = 0--tonumber(store.initPos[4])
        local storeName = ""
        self.merchants[i] = GameMerchant.new(initPos, storeYaw, storeName, store.initPos[i].sp_room_id, i)
        self:initGoods(store.goods, i)
    end
end

function MerchantConfig:initGoods(goods, index)
    for type, goodsList in pairs(goods) do
        for i, goods in pairs(goodsList) do
            local item = {}
            item.desc = goods.desc
            item.image = goods.image or ""
            item.itemId = tonumber(goods.price[1])
            item.itemMeta = tonumber(goods.price[2])
            item.itemNum = tonumber(goods.price[3])
            item.coinId = tonumber(goods.price[4])
            item.price = tonumber(goods.price[5])
            if goods.notify ~= nil then
                item.notify = tonumber(goods.notify)
            else
                item.notify = 0
            end
            MerchantUtil:addCommodity(index, type, self:newCommodity(item))
        end
    end
end

function MerchantConfig:newCommodity(item)
    local commodity = Commodity.new()
    commodity.itemId = item.itemId
    commodity.itemMeta = item.itemMeta
    commodity.itemNum = item.itemNum
    commodity.coinId = item.coinId
    commodity.price = item.price
    commodity.desc = item.desc
    commodity.notify = item.notify
    commodity.image = item.image
    return commodity
end

function MerchantConfig:prepareStore(player)
    for i, v in pairs(self.merchants) do
        v:syncPlayer(player)
    end
    -- self.merchants:syncPlayer(player)
end

return MerchantConfig