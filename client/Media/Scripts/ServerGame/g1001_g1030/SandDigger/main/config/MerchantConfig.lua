require "data.GameMerchant"
require "config.BackpackConfig"
require "config.Area"

MerchantConfig = {}
MerchantConfig.prices = {}
MerchantConfig.coinMapping = {}
MerchantConfig.merchants = {}
MerchantConfig.location = {}
MerchantConfig.exchangeMultiple = 0
MerchantConfig.exchangePosition = {}

function MerchantConfig:init(config)
    self:initStores(config.stores)
    self:initCoinMapping(config.coinMapping)
    self:initLocation(config.location)
    self:initExchangeMultiple(config.exchangeMultiple)
    self:initExchangePosition(config.exchangePosition)
end

function MerchantConfig:initLocation(location)
    self.location = Area.new(location)
end

function MerchantConfig:initExchangeMultiple(exchangeMultiple)
    self.exchangeMultiple = tonumber(exchangeMultiple)
end

function MerchantConfig:initExchangePosition(exchangePosition)
    self.exchangePosition = VectorUtil.newVector3(exchangePosition[1], exchangePosition[2], exchangePosition[3])
end

function MerchantConfig:initCoinMapping(coinMapping)
    for i, v in pairs(coinMapping) do
        self.coinMapping[i] = {}
        self.coinMapping[i].coinId = v.coinId
        self.coinMapping[i].itemId = v.itemId
    end
end

function MerchantConfig:initStores(stores)
    local beginIndex = 0
    for i, v in pairs(stores) do
        local store = {}
        store.initPos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        store.storeYaw = tonumber(v.initPos[4])
        store.storeName = v.name
        self.merchants[i] = GameMerchant.new(beginIndex, store.initPos, store.storeYaw, store.storeName)
        self:initGoods(beginIndex, v.goods)
        beginIndex = i * BackpackConfig.maxLevel
    end
end

function MerchantConfig:initGoods(index, prices)
    for level, levelList in pairs(prices) do
        for type, goodsList in pairs(levelList) do
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
                MerchantUtil:addCommodity(level + index, type, self:newCommodity(item))
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

return MerchantConfig