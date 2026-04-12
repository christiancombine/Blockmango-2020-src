
require "data.GiftBag"
PersonalStoreLabel = class("PersonalStore")

function PersonalStoreLabel:ctor(config)
    self.id = config.id
    self.name = config.name
    self.pic = config.pic
    self.giftbags = {}
    self.sortgiftBags = {}
    self:NewGiftbag()
end

function PersonalStoreLabel:NewGiftbag()
    local giftbagItems = ShopConfig:GetgiftbagInfo(self.id)
    for _, config in ipairs(giftbagItems) do
        local bag = GiftBag.new(config)
        self.giftbags[config.teamId] = bag
        table.insert(self.sortgiftBags, bag)
    end
    if next(self.sortgiftBags) then
        table.sort(self.sortgiftBags, function(a, b)
            return  a.id < b.id
        end)
    end
end

function PersonalStoreLabel:SyncStoreLableInfo(player)
    local birdPersonalStoreTab = BirdPersonalStoreTab.new()
    birdPersonalStoreTab.tabId = self.id
    birdPersonalStoreTab.tabName = self.name
    birdPersonalStoreTab.tabIcon = self.pic
    local data = {}
    for index, gift in ipairs(self.sortgiftBags) do
        local info = gift:SyncGiftInfo(player)
        table.insert(data, info)
    end
    birdPersonalStoreTab.items = data
    return birdPersonalStoreTab
end

function PersonalStoreLabel:OnPlayerBuyItem(player, giftbagId)
    local bag = self.giftbags[giftbagId]
    if type(bag) == "table" then
        return bag:OnPlayerByGiftbag(player)
    end
    return false
end
