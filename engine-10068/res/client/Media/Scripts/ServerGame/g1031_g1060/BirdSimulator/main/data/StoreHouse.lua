require "manager.ConsumeDiamondsManager"

StoreHouse = class("StoreHouse")

--  1 采集器  2 存储灌  3  装饰
function StoreHouse:ctor(config)
    self.storeId = config.storeId
    self.minPos = config.minPos
    self.maxPos = config.maxPos
    self.itemInfos = {}
    self.sortItems = {}
    local storeInfo = {}
    for _, info in pairs(config.itemInfos) do
        local id = info.ItemId
        self.itemInfos[id] = info
        storeInfo.itemId = info.ItemId
        storeInfo.storeId = info.BindStoreId
        EngineWorld:addSessionNpc(info.pos, info.yaw, 104103, info.actorModelName, function(entity)
            entity:setNameLang(info.ItemName or "")
            entity:setActorBody(info.actorBody or "body")
            entity:setActorBodyId(info.actorBodyId or "")
            entity:setSessionContent(json.encode(storeInfo) or "")
            entity:setPerson(true)
        end)
        table.insert(self.sortItems, info)
    end
    table.sort(self.sortItems, function(a, b)
        return a.id < b.id
    end)
end

function StoreHouse:SyncStoreHouse(player)
    local birdStore = BirdStore.new()
    birdStore.id = self.storeId
    birdStore.startPos = self.minPos
    birdStore.endPos = self.maxPos

    local items = {}
    for _, item in ipairs(self.sortItems) do
        local birdStoreItem = BirdStoreItem.new()
        birdStoreItem.id = item.ItemId
        birdStoreItem.storeId = item.BindStoreId
        birdStoreItem.type = item.ItemType
        birdStoreItem.price = item.price
        birdStoreItem.currencyType = item.currencyType
        birdStoreItem.status = player:GetItemStatus(item.ItemId) -- 根据玩家拿到物品状态

        birdStoreItem.name = item.ItemName
        birdStoreItem.desc = item.ItemDesc
        birdStoreItem.icon = item.icon
        birdStoreItem.iconNew = item.iconNew
        table.insert(items, birdStoreItem)
    end
    birdStore.items = items
    return birdStore
end

-- 当玩家切换物品状态的时候  切换其他同类物品的状态
function StoreHouse:OnChangeItemStatus(player, itemid, status)
    local curItem = self.itemInfos[itemid]
    if curItem.ItemType == 1 then
        --采集器
        player:SetCollector(itemid, curItem.actorName, curItem.actorId)
    elseif curItem.ItemType == 2 then
        --存储灌
        player:SetStorageBag(itemid, curItem.actorName, curItem.actorId)
    elseif curItem.ItemType == 3 then
        --装饰
        player:SetDecoration(itemid, curItem.actorName, curItem.actorId)
    elseif curItem.ItemType == 4 then
        --翅膀
        player:SetWing(itemid, curItem.actorName, curItem.actorId)
    end
end

function StoreHouse:OnPlayerBuyItem(player, itemId)
    local itemInfo = self.itemInfos[itemId]
    local num = player:getHaveItemFromStore(itemId)
    if num == false and itemInfo then
        local currencyType = itemInfo.currencyType
        local key = string.format("%s%s:%d", "BuyStoreItem", tostring(player.rakssid), itemId)
        local itemPrice = itemInfo.price
        if currencyType == CurrencyType.EggTicket then
            if player:GetTicketNum() < itemPrice then
                return false
            end
            player:SubEggTicket(itemPrice)
            self:OnConsumptionResultBack(key, player.rakssid)
        elseif currencyType == CurrencyType.Money then
            if player:GetMoneyNum() < itemPrice then
                return false
            end
            player:subCurrency(itemPrice)
            self:OnConsumptionResultBack(key, player.rakssid)
        elseif currencyType == CurrencyType.Diamond then

            player:consumeDiamonds(tonumber(itemId), itemPrice, key, false)
            ConsumeDiamondsManager.AddConsumeSuccessEvent(key, self.OnConsumptionResultBack, self)
        end
        return true
    end
    return false
end

function StoreHouse:OnConsumptionResultBack(key, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    local itemId = StringUtil.split(key, ":")[2]
    if itemId ~= nil then
        itemId = tonumber(itemId)
    end
    if type(itemId) ~= "number" or player == nil then
        return false
    end
    TaskHelper.dispatch(TaskType.CollectPropEvent, player.userId, itemId, 1)

    self:OnChangeItemStatus(player, itemId, StoreItemStatus.OnUser)
    StoreHouseManager:RefreshStoreInfo(player)
    return true
end
