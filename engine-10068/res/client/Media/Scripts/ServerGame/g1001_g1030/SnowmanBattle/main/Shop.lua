require "config.GameConfig"
require "Match"

Shop = {}

function Shop:init()
    for _, shop in ipairs(GameConfig.shops) do
        EngineWorld:addGoodsGroupToShop(shop.type, shop.icon, shop.name)
        for _, item in ipairs(shop.goods) do
            local goods = Goods.new()
            goods.itemId = item.itemId
            goods.itemMeta = item.itemMeta
            goods.itemNum = item.itemNum
            goods.coinId = item.coinId
            goods.blockymodsPrice = item.blockymodsPrice
            goods.blockmanPrice = item.blockmanPrice
            goods.limit = item.limit
            goods.desc = item.desc
            EngineWorld:addGoodsToShop(shop.type, goods)
        end
    end
end

return Shop
