require "config.StoreHouseConfig"
require "config.StoreHouseItems"
require "data.StoreHouse"

StoreItemStatus = {
    NotBuy = 1, --没有购买
    NotUse = 2, --没有使用
    OnUser = 3 --正在使用
}

StoreHouseManager = {}

function StoreHouseManager:Init()
    self.storeTable = {}
    local storeConfis = StoreHouseConfig:GetAllConfigs()
    for _, config in pairs(storeConfis) do
        local id = config.storeId
        local itemInfo = StoreHouseItems:GetItemInfoByStoreId(id)
        config.itemInfos = itemInfo
        local store = StoreHouse.new(config)
        self.storeTable[id] = store
    end
end

function StoreHouseManager:SyncStoreInfo(player)
    if player == nil then
        return
    end
    self:RefreshStoreInfo(player)
end

function StoreHouseManager:OnPlayerBuyItem(player, storeId, itemId, operationType)
    local store = self.storeTable[storeId]
    if operationType == 1 then -- 当玩家购买的时候
        if type(store) == "table" then
            store:OnPlayerBuyItem(player, itemId)
            return true        
        end
        return false
    elseif operationType == 2 then --当玩家装备的时候
        local result = player:getHaveItemFromStore(itemId)
        if result == false then
            player:SetItemStatus(itemId, StoreItemStatus.NotBuy)
            return false
        end
        player:SetItemStatus(itemId, StoreItemStatus.OnUser)
        store:OnChangeItemStatus(player, itemId, StoreItemStatus.OnUser)
        self:RefreshStoreInfo(player)
    end
    return false
end

function StoreHouseManager:RefreshStoreInfo(player)
    local birdSimulator = player:getBirdSimulator()
    if birdSimulator ~= nil then
        local stores = {}
        for _, store in pairs(self.storeTable) do
            if type(store) == "table" then
                local houseInfo = store:SyncStoreHouse(player)
                table.insert(stores, houseInfo)
            end
        end
        birdSimulator:setStores(stores)
    end
end
