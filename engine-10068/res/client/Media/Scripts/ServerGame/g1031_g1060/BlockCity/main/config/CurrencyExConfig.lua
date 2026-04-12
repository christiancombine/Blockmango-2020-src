CurrencyExConfig = {}
CurrencyExConfig.shop = {}

function CurrencyExConfig:init(data)
    for _, v in pairs(data) do
        local level = tonumber(v.level)
        local type = tonumber(v.type)
        local icon = v.icon
        local name = v.name

        local item = {}
        item.itemId = tonumber(v.itemId)
        item.itemMeta = tonumber(v.itemMeta)
        item.itemNum = tonumber(v.itemNum)
        item.coinId = tonumber(v.coinId)
        item.blockmodsPrice = tonumber(v.blockmodsPrice)
        item.blockmanPrice = tonumber(v.blockmanPrice)
        item.limit = tonumber(v.limit)
        item.desc = v.desc

        if not self.shop[type] then
            self.shop[type] = {
                type = type,
                icon = icon,
                name = name,
                goods = {}
            }
        end

        self.shop[type].goods[level] = item
    end
end

function CurrencyExConfig:prepareCurrencyExShop()
    for _, v in pairs(self.shop) do
        EngineWorld:addGoodsGroupToShop(v.type, v.icon, v.name)
        for _, item in pairs(v.goods) do
            EngineWorld:addGoodsToShop(v.type, self.newGoods(item))
        end
    end
end

function CurrencyExConfig.newGoods(item)
    local goods = Goods.new()
    goods.itemId =  item.itemId
    goods.itemMeta =  item.itemMeta
    goods.itemNum =  item.itemNum
    goods.coinId =  item.coinId
    goods.blockymodsPrice =  item.blockmodsPrice
    goods.blockmanPrice =  item.blockmanPrice
    goods.limit =  item.limit
    goods.desc =  item.desc
    return goods
end
