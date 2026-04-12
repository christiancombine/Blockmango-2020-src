require "config.ShopConfig"
GiftBag = class("GiftBag")

local ItemType = {
    Type_Item = 1, -- 物品
    Type_Privilege = 2, -- 特权
    Type_Dress = 3 -- 装饰
}

function GiftBag:ctor(config)
    self.id = config.id
    self.teamId = config.teamId
    self.name = config.name
    self.pic = config.pic
    self.bgPic = config.bgPic
    self.desc = config.desc
    self.moneyType = config.moneyType
    self.price = config.price
    self.limit = config.limit
    self.discount = config.discount
    self.topLeftBg = config.topLeftBg
    self.topLeftDesc = config.topLeftDesc
    self.topRightBg = config.topRightBg
    self.topRightDesc = config.topRightDesc
    self.shopLabel = config.shopLabel
    self.itemInfos = ShopConfig:GetItemInfoByGiftbagid(self.teamId)
end

function GiftBag:SyncGiftInfo(player)
    local birdGiftBag = BirdGiftBag.new()
    birdGiftBag.id = self.teamId
    birdGiftBag.price = self.price
    birdGiftBag.limit = 0
    local num = player:GetGiftNumberof(self.teamId)
    if self.limit == -1 or num < self.limit then
        birdGiftBag.limit = 1
    end
    birdGiftBag.discount = self.discount
    birdGiftBag.currencyType = self.moneyType
    birdGiftBag.icon = self.pic == nil or self.pic == "@@@" and "" or self.pic
    birdGiftBag.desc = self.desc == nil or self.desc == "@@@" and "" or self.desc
    birdGiftBag.name = self.name == nil or self.name == "@@@" and "" or self.name
    birdGiftBag.topLeftIcon = self.topLeftBg == nil or self.topLeftBg == "@@@" and "" or self.topLeftBg
    birdGiftBag.topLeftName = self.topLeftDesc == nil or self.topLeftDesc == "@@@" and "" or self.topLeftDesc
    birdGiftBag.topRightIcon = self.topRightBg == nil or self.topRightBg == "@@@" and "" or self.topRightBg
    birdGiftBag.topRightName = self.topRightDesc == nil or self.topRightDesc == "@@@" and "" or self.topRightDesc
    birdGiftBag.timeLeft = 0
    birdGiftBag.items = {}
    local items = {}
    for _, itemInfo in ipairs(self.itemInfos) do
        local birdGiftBagItem = BirdGiftBagItem.new()
        birdGiftBagItem.id = itemInfo.id
        birdGiftBagItem.num = itemInfo.num
        birdGiftBagItem.type = itemInfo.type
        birdGiftBagItem.giftBagId = itemInfo.teamId
        birdGiftBagItem.icon = itemInfo.icon
        birdGiftBagItem.isAutoUse = itemInfo.autoEquip == 1 and true or false
        table.insert(items, birdGiftBagItem)
    end
    birdGiftBag.items = items
    return birdGiftBag
end

-- 判断 item类型  根据类型给玩家加对应的效果
function GiftBag:OnPlayerByGiftbag(player)
    if player:GetGiftNumberof(self.teamId) >= self.limit and self.limit ~= -1 then
        return false
    end
    if self.moneyType == CurrencyType.Money then -- 绿钞
        local moneyNum = player:GetMoneyNum()
        if moneyNum < self.price then
            return false
        end
        player:subCurrency(self.price)
        self:OnBuyGiftbagSucess(nil, player.rakssid)
        return true
    end

    if self.moneyType == CurrencyType.EggTicket then --抽蛋票
        if player:SubEggTicket(self.price) then
            self:OnBuyGiftbagSucess(nil, player.rakssid)
            return true
        end
    end

    if self.moneyType == CurrencyType.Diamond then -- 魔方
        local key = string.format("%s%s:%d", "BuyGiftbag", tostring(player.rakssid), self.teamId)
        player:consumeDiamonds(tonumber(self.teamId), self.price, key, false)
        ConsumeDiamondsManager.AddConsumeSuccessEvent(key, self.OnBuyGiftbagSucess, self)
    end
end

function GiftBag:OnBuyGiftbagSucess(key, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    player:AddGiftbag(self.teamId)
    local birdGains = {}
    for _, info in pairs(self.itemInfos) do
        if type(info.type) == "number" and type(info.itemId) == "number" then
            if info.itemId ~= 9000001 then
                local birdGain = BirdGain.new()
                birdGain.itemId = info.itemId
                birdGain.num = info.num
                birdGain.icon = info.icon
                birdGain.name = info.name
                table.insert(birdGains, birdGain)
            end
            if info.type == ItemType.Type_Item then
                player:addProp(info.itemId, info.num, info.icon)
            elseif info.type == ItemType.Type_Privilege then
                player:addPrivilegeId(info.itemId)
            elseif info.type == ItemType.Type_Dress then
                local partInfo = BirdConfig:getPartInfoById(info.itemId)
                if partInfo ~= nil then
                    player.userBird:addDress(partInfo.id, partInfo.type, partInfo.bodyId)
                    player:setBirdSimulatorBirdDress()
                    player:setBirdSimulatorBag()
                end
            end
        end
    end
    if next(birdGains) then
        HostApi.sendBirdGain(player.rakssid, birdGains)
    end
    --PersonalStoreManager:SyncStoreInfo(player)
    self:SyncPersonalStoreInfo(player)
    return true
end

function GiftBag:SyncPersonalStoreInfo(player)
    local birdGiftBag = self:SyncGiftInfo(player)
    local label = PersonalStoreManager:getLabel(self.shopLabel)
    if type(label) == "table" then
        local birdPersonalStoreTab = label:SyncStoreLableInfo(player)
        if birdPersonalStoreTab ~= nil then
            HostApi.sendBirdSimulatorPersonStoreTab(player.rakssid, birdPersonalStoreTab)
        end
    end
end
