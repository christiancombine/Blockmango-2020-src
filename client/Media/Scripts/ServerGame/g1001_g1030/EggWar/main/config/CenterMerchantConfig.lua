
require "data.CenterMerchant"

CenterMerchantConfig = {}
CenterMerchantConfig.CenterMerchantNpcs = {}
CenterMerchantConfig.index = 30
function CenterMerchantConfig:init(config)
    self:initStores(config)
end

function CenterMerchantConfig:initStores(stores)
    local beginIndex = 30
    for i, v in pairs(stores) do
        local store = {}
        store.initPos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        store.storeYaw = tonumber(v.initPos[4])
        store.storeName = v.name
        store.effect = v.effect or ""
        self.CenterMerchantNpcs[i] = CenterMerchant.new(beginIndex, store.initPos, store.storeYaw, store.storeName, store.effect)
        self:initGoods(beginIndex, v.goods)
        beginIndex = beginIndex + 1
    end
end

function CenterMerchantConfig:initGoods(index, prices)
    for type, goodsList in pairs(prices) do
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
            MerchantUtil:addCommodity(index, type, self:newCommodity(item))
            self.index = self.index + 1
        end
    end
end

function CenterMerchantConfig:newCommodity(item)
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

function CenterMerchantConfig:prepareStore(rakssid)
    for i, v in pairs(self.CenterMerchantNpcs) do
        v:syncPlayer(rakssid)
    end
end

function CenterMerchantConfig:addEffect()
    for i, v in pairs(self.CenterMerchantNpcs) do
        EngineWorld:addSimpleEffect(v.effect, v.pos, v.yaw)
    end
end


return CenterMerchantConfig