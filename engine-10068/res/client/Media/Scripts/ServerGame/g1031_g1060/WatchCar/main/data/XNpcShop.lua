require "config.XNpcShopConfig"
require "config.XNpcShopGoodsConfig"

XNpcShop = class()
XNpcShop.bindTeamId = 0
function XNpcShop:ctor(config)
   
    if config == nil then
        return
    end
    self.bindTeamId = config.BelongTeamId
    self.entityId = EngineWorld:addMerchantNpc(config.MerchantPos, config.MerchantYaw, config.MerchantName)
    local configs = XNpcShopGoodsConfig:getGoodsConfigs(config.MerchantId)
    for index = 1, #configs do
        local item = configs[index]
        local commodity = self:NewCommodity(item)
        MerchantUtil:addCommodity(self.bindTeamId, item.GoodsType, commodity)
    end
end

function XNpcShop:NewCommodity(item)
    local commodity = Commodity.new()
    commodity.itemId = item.GoodsId
    commodity.itemMeta = item.GoodsMetaId

    commodity.itemNum = item.Numberof
    commodity.coinId = item.CoinId
    commodity.price = item.GoodsPrice
    commodity.desc = item.GoodsDesc or ""
    commodity.notify = 0
    commodity.image = item.GoodsImage or ""
    commodity.tipDesc = ""
    return commodity
end

function XNpcShop:syncPlayer(player)
    MerchantUtil:changeCommodityList(self.entityId, player.rakssid, self.bindTeamId)
end

function XNpcShop:onRemove()
    EngineWorld:removeEntity(self.entityId)
end
