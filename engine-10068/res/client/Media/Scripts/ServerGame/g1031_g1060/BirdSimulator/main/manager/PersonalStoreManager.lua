require "config.ShopConfig"
require "data.PersonalStoreLabel"
PersonalStoreManager = {}

function PersonalStoreManager:Init()
    self.storeGroups = {}
    local groups = ShopConfig:GetStoreLableInfo()
    for k, config in pairs(groups) do
        local group = PersonalStoreLabel.new(config)
        self.storeGroups[config.id] = group
    end
end

function PersonalStoreManager:SyncStoreInfo(player)
    if player == nil then
        return
    end
    local birdSimulator = player:getBirdSimulator()
    if birdSimulator == nil then
        return
    end
    local storeInfos = {}
    for _, group in pairs(self.storeGroups) do
        local label = group:SyncStoreLableInfo(player)
        table.insert(storeInfos, label)   
    end
    birdSimulator:setPersonalStore(storeInfos)
end

function PersonalStoreManager:OnPlayerBuyItem(player, groupId, giftId)
    if giftId == 9999 then
        NovicePackage:OnPlayerBuy(player)
    end
    local group = self.storeGroups[groupId]
    if type(group) == "table" then
        return group:OnPlayerBuyItem(player, giftId)
    end
    return false
end

function PersonalStoreManager:getLabel(labelId)
    return self.storeGroups[labelId]
end