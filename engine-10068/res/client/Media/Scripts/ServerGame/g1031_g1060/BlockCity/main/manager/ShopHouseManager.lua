require "data.GameShopHouse"

ShopHouseManager = {}
ShopHouseManager.mainShop = {}

function ShopHouseManager:init()
    local allShop = ShopHouseConfig:getAllShopHouses()
    for _, mainShop in pairs(allShop) do
        local classifyId = mainShop.classifyId

        for branchId, branch in pairs(mainShop.branch) do
            local infos = ShopHouseConfig:getBranchItemsByBranchId(branchId)
            branch.itemsInfos = infos
        end

        local shop = GameShopHouse.new(mainShop)
        self.mainShop[classifyId] = shop
    end
end

function ShopHouseManager:getItemInfosByItemId(itemId)
    for _, v in pairs(self.mainShop) do
        local itemInfos = v:getItemInfosByItemId(itemId)
        if itemInfos then
            return itemInfos
        end
    end
    return nil
end

function ShopHouseManager:getMainShopByClassifyId(classifyId)
    classifyId = tonumber(classifyId)
    return self.mainShop[classifyId]
end

function ShopHouseManager:syncShopInfo(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end

    local sendPacket = {}
    for _, mainShop in pairs(self.mainShop) do
        local packet = mainShop:syncMainShopInfo(rakssid)
        table.insert(sendPacket, packet)
    end

    player.entityPlayerMP:getBlockCity():setHouseShop(sendPacket)
end

function ShopHouseManager:syncSingleShop(rakssid, classifyId, itemId, isNeedRemove, index)
    classifyId = tonumber(classifyId)
    local mainShop = self:getMainShopByClassifyId(classifyId)
    if not mainShop then
        return
    end
    --更新商品数据
    isNeedRemove = (isNeedRemove and {isNeedRemove} or {false})[1]
    mainShop:changeGoodsStatus(rakssid, itemId, isNeedRemove, index)
    local packet = mainShop:syncMainShopInfo(rakssid)
    HostApi.sendBlockCityPackTab(rakssid, packet)
end

function ShopHouseManager:getItemInfoByClassifyIdAndItemId(classifyId, itemId)
    classifyId = tonumber(classifyId)
    itemId = tonumber(itemId)
    local shop = ShopHouseManager:getMainShopByClassifyId(classifyId)
    if shop then
        return shop.itemsInfos[itemId]
    end

    return nil
end

return ShopHouseManager