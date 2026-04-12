---
--- Created by Jimmy.
--- DateTime: 2017/10/19 0019 11:32
---
require "messages.Messages"

MerchantConfig = {}

MerchantConfig.prices = {}
MerchantConfig.shopTruck = {}
MerchantConfig.equipment = {}

function MerchantConfig:initGoods(prices)
    for team, teamList in pairs(prices) do
        for level, levelList in pairs(teamList) do
            self.prices[(team - 1) * 2 + level] = {}
            for itemtype, goodsList in pairs(levelList) do
                local pirce_table = {}
                pirce_table.type = itemtype
                pirce_table.goods = {}
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
                    pirce_table.goods[tostring(item.gid)] = item
                    MerchantUtil:addCommodity((team - 1) * 2 + level, itemtype, self:newCommodity(item))

                    if itemtype == 1 or itemtype == 2 then
                        if self.equipment[item.gid] == nil then
                            self.equipment[item.gid] = 1
                        end
                    end
                end
                self.prices[(team - 1) * 2 + level][itemtype] = pirce_table
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

function MerchantConfig:initShopTruck(config)
    for i, v in pairs(config) do
        local truck = {}
        truck.position = VectorUtil.newVector3(v.position[1], v.position[2], v.position[3])
        truck.yaw = tonumber(v.position[4])
        truck.actorName = tostring(v.actorName)
        truck.name = tostring(v.name)
        self.shopTruck[i] = truck
    end
end

function MerchantConfig:prepareShopTruck()
    for i, v in pairs(self.shopTruck) do
        if v.actorName ~= nil then
            EngineWorld:addActorNpc(v.position, v.yaw, v.actorName, function(entity)
                entity:setHeadName(v.name or "")
            end)
        end
    end
end

return MerchantConfig