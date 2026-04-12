---
--- Created by Jimmy.
--- DateTime: 2017/10/19 0019 11:32
---
require "data.GameMerchant"
require "Match"

MerchantConfig = class()

function MerchantConfig:ctor(begin, stores)
    self.storeIds = {}
    self.prices = {}
    self.coinMapping = {}
    self.merchants = {}
    self:initStores(begin, stores)
end

function MerchantConfig:initStores(begin, stores)
    local beginIndex = begin
    for i, v in pairs(stores) do
        local store = {}
        store.initPos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        store.storeYaw = tonumber(v.initPos[4])
        store.storeName = v.name
        self.merchants[i] = GameMerchant.new(beginIndex, store.initPos, store.storeYaw, store.storeName)
        self:initGoods(beginIndex, v.goods)
        beginIndex = begin + i * 3
    end
    SceneConfig.beginIndex = beginIndex
end

function MerchantConfig:initGoods(begin, prices)
    for role, roleList in pairs(prices) do
        for type, goodsList in pairs(roleList) do
            for i, goods in pairs(goodsList) do
                local item = {}
                item.desc = goods.desc
                item.image = goods.image or ""
                item.gid = tonumber(goods.price[1])
                item.gmt = tonumber(goods.price[2])
                item.gc = tonumber(goods.price[3])
                item.eid = tonumber(goods.price[4])
                item.ec = tonumber(goods.price[5])
                if goods.notify ~= nil then
                    item.notify = tonumber(goods.notify)
                else
                    item.notify = 0
                end
                MerchantUtil:addCommodity(begin + role, type, self:newCommodity(item))
            end
        end
    end
end

function MerchantConfig:newCommodity(item)
    local commodity = Commodity.new()
    commodity.itemId = item.gid
    commodity.itemMeta = item.gmt
    commodity.itemNum = item.gc
    commodity.coinId = item.eid
    commodity.price = item.ec
    commodity.desc = item.desc
    commodity.notify = item.notify
    commodity.image = item.image
    return commodity
end

function MerchantConfig:prepareStore(player)
    for i, v in pairs(self.merchants) do
        v:syncPlayer(player)
    end
end

return MerchantConfig