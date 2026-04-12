require "data.GameMerchant"

MerchantConfig = {}
MerchantConfig.prices = {}
MerchantConfig.coinMapping = {}
MerchantConfig.merchants = {}
MerchantConfig.items = {}
MerchantConfig.index = 1
function MerchantConfig:init(config)
    self:initStores(config.stores)
    self:initCoinMapping(config.coinMapping)
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
        beginIndex = i * 6
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
                item.charmValue = goods.charmValue
                self.items[self.index] = item
                self.index = self.index + 1
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

function MerchantConfig:prepareStore(player)
    for i, v in pairs(self.merchants) do
        v:syncPlayer(player)
    end
end

return MerchantConfig