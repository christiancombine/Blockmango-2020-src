GameShopHouse = class("GameShopHouse")

function GameShopHouse:ctor(mainShop)
    self.classifyId = mainShop.classifyId
    self.classify = mainShop.classify
    self.classifyImage = mainShop.classifyImage
    self.branchs = {}
    self.itemsInfos = {}
    self.sortItems = {}

    for _, branch in pairs(mainShop.branch) do
        for _, infos in pairs(branch.itemsInfos) do
            local itemId = infos.itemId
            local areaId = branch.areaId[1]
            self:addSessionNpc(infos, areaId)

            self.itemsInfos[itemId] = infos
            table.insert(self.sortItems, infos)
        end

        table.insert(self.branchs, branch)
        self:addAreaPush(branch.areaId, branch.pos)
    end

    table.sort(self.sortItems, function(a, b)
        return a.id < b.id
    end)
end

function GameShopHouse:addAreaPush(areaId, pos)
    for index, v in pairs(pos) do
        AreaInfoManager:push(
                areaId[index],
                AreaType.ShopHouse,
                "gui_blockcity_open_shopHouse",
                "gui_blockcity_open_shopHouse",
                "",
                "",
                false,
                v.min,
                v.max
        )
    end
end

function GameShopHouse:addSessionNpc(infos, areaId)
    local jsonInfos = {}
    jsonInfos.itemId = infos.itemId
    jsonInfos.areaId = areaId
    jsonInfos.isCanBuy = infos.isCanBuy
    EngineWorld:addSessionNpc(infos.pos, infos.yaw, 104708, infos.actorModelName, function(entity)
        entity:setNameLang(infos.itemName or "")
        entity:setActorBody(infos.actorBody or "body")
        entity:setActorBodyId(infos.actorBodyId or "")
        entity:setSessionContent(json.encode(jsonInfos) or "")
        entity:setPerson(true)
    end)
end

function GameShopHouse:changeGoodsStatus(rakssid, itemId, isNeedRemove, index)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    local itemInfos = self:getItemInfosByItemId(itemId)
    if not itemInfos then
        return
    end
    local itemType = itemInfos.itemType
    if itemType == ShopItemType.Car then

    elseif itemType == ShopItemType.Wings then
        player:setWing(itemId)
    else
        player:setShopOtherItem(itemType, itemId, isNeedRemove, index)
    end

end

function GameShopHouse:getItemInfosByItemId(itemId)
    itemId = tonumber(itemId)
    return self.itemsInfos[itemId]
end

function GameShopHouse:syncBranchItems(rakssid, itemsInfos)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end

    local items = {}
    for _, infos in pairs(itemsInfos) do
        local itemId = infos.itemId
        local hadNum = 0
        local wingsStartTime = player:getWingsRemainTimeById(itemId)
        local status = player:getItemStatusInfos(itemId)
        if status == ShopItemStatus.NotBuy then
            hadNum = 0
        elseif self.classifyId ~= ShopItemType.Flower then
            hadNum = 1
        else
            hadNum = player.bagRoses[itemId] or 0
            status = (hadNum == 0 and { ShopItemStatus.NotBuy } or { status })[1]
            player:setItemStatusInfos(itemId, status)
        end

        local packet = BlockCityHouseShopItem.new()
        packet.itemId = itemId
        packet.haveNum = hadNum
        packet.itemStatus = status
        packet.limit = infos.limit
        packet.itemScale = infos.scale
        packet.isCanBuy = infos.isCanBuy
        packet.moneyType = infos.moneyType
        packet.itemActorName = infos.actorModelName
        packet.limitTime = (infos.availableTime - (wingsStartTime and { os.time() - wingsStartTime } or { 0 })[1]) * 1000
        table.insert(items, packet)
    end
    return items
end

function GameShopHouse:syncMainShopInfo(rakssid)
    local shops = {}
    for _, v in pairs(self.branchs) do
        local items = self:syncBranchItems(rakssid, v.itemsInfos)
        local packet = BlockCityHouseShop.new()
        packet.items = items
        packet.areaIds = v.areaId
        table.insert(shops, packet)
    end

    local packet = BlockCityHouseShopClassify.new()
    packet.shops = shops
    packet.shopClassifyId = self.classifyId
    packet.shopClassifyName = self.classify
    packet.shopClassifyImage = self.classifyImage
    return packet
end
