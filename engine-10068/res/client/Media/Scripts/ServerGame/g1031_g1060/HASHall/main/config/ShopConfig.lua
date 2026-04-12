---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---
ShopConfig = {}

ShopConfig.shop = {}

function ShopConfig:init(config)
   self:initShopGoods(config)
end

function ShopConfig:initShopGoods(config)
 for groupIndex, group in pairs(config) do
        local goodsGroup = {}
        goodsGroup.type = group.type
        goodsGroup.icon = group.icon
        goodsGroup.name = group.name
        goodsGroup.goods = {}
        for goodsIndex, goodsItem in pairs(group.goods) do
            local item = {}
            item.itemId = goodsItem.price[1]
            item.itemMeta = goodsItem.price[2]
            item.itemNum = goodsItem.price[3]
            item.coinId = goodsItem.price[4]
            item.blockymodsPrice = goodsItem.price[5]
            item.blockmanPrice = goodsItem.price[6]
            item.limit = goodsItem.price[7]
            item.desc = goodsItem.desc
            goodsGroup.goods[goodsIndex] = item
        end
        self.shop[groupIndex] = goodsGroup
    end
end

function ShopConfig:prepareShop()
    for groupIndex, group in pairs(self.shop) do
        EngineWorld:addGoodsGroupToShop(group.type, group.icon, group.name)
        for goodsIndex, goods in pairs(group.goods) do
            EngineWorld:addGoodsToShop(group.type, self:newGoods(goods))
        end
    end
end

function ShopConfig:newGoods(item)
    local goods = Goods.new()
    goods.itemId =  item.itemId
    goods.itemMeta =  item.itemMeta
    goods.itemNum =  item.itemNum
    goods.coinId =  item.coinId
    goods.blockymodsPrice =  item.blockymodsPrice
    goods.blockmanPrice =  item.blockmanPrice
    goods.limit =  item.limit
    goods.desc =  item.desc
    return goods
end

return ShopConfig