XNpcShopGoodsConfig = {}
XNpcShopGoodsConfig.confilTables = {}
function XNpcShopGoodsConfig:OnInit(configs)
    for _, config in pairs(configs) do
        local configTab = {}
        configTab.GoodsId = tonumber(config.GoodsId)
        configTab.BelongMer = tonumber(config.BelongMer)
        configTab.GoodsDesc = config.GoodsDesc == '#' and "" or config.GoodsDesc
        configTab.GoodsMetaId = tonumber(config.GoodsMetaId) or 0
        configTab.GoodsType = tonumber(config.GoodsType)
        configTab.GoodsPrice = tonumber(config.GoodsPrice)
        configTab.Numberof = tonumber(config.Numberof)
        configTab.GoodsImage = config.GoodsImage == '#' and "" or config.GoodsImage
        configTab.GoodsFumo = config.GoodsFumo == '#' and "" or config.GoodsFumo
        configTab.CoinId = tonumber(config.CoinId) or 1
        configTab.LimitCount = tonumber(config.LimitCount) or 1
        table.insert(self.confilTables, configTab)
    end
end

--获取 某个商人的 商品
function XNpcShopGoodsConfig:getGoodsConfigs(merId)
    local goodsList = {}
    for index = 1, #self.confilTables do
        local goods = self.confilTables[index]
        if goods.BelongMer == 0 or goods.BelongMer == merId then
            table.insert(goodsList, goods)
        end
    end
    return goodsList
end

function XNpcShopGoodsConfig:getLimitCountByGoodsId(goodsId)
    for _, goods in pairs(self.confilTables) do
        if goods.GoodsId == goodsId then
            return goods.LimitCount
        end
    end
    return 1
end