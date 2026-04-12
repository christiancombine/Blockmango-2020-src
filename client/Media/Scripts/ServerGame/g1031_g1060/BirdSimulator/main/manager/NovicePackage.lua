require "config.ShopConfig"
NovicePackage = {}

function NovicePackage:Init()
    self.teamId = 9999
    local config = ShopConfig:getPackageById(self.teamId)
    if config == nil then
        return
    end
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
    self.timeLeft = config.timeLeft
    self.itemInfos = ShopConfig:GetItemInfoByGiftbagid(self.teamId)
end

function NovicePackage:OnPlayerBuy(player)
    if player:getPackageTime(self.timeLeft) ~= -1 then
        if self.moneyType == CurrencyType.Money then -- 绿钞
            local moneyNum = player:GetMoneyNum()
            if moneyNum < self.price then
                return false
            end
            player:subCurrency(self.price)
            self:OnBuySuccess(nil, player.rakssid)
            return true
        end

        if self.moneyType == CurrencyType.EggTicket then --抽蛋票
            if player:SubEggTicket(self.price) then
                self:OnBuySuccess(nil, player.rakssid)
                return true
            end
        end

        if self.moneyType == CurrencyType.Diamond then -- 魔方
            local key = string.format("%s%s:%d", "NovicePackage", tostring(player.rakssid), self.teamId)
            player:consumeDiamonds(tonumber(self.teamId), self.price, key, false)
            ConsumeDiamondsManager.AddConsumeSuccessEvent(key, self.OnBuySuccess, self)
        end
        return true
    end
    return false
end

function NovicePackage:OnBuySuccess(key, rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    local birdGiftBag = self:SendPackageData(player)
    player.novicePackageId = self.teamId
    birdGiftBag.timeLeft = player:getPackageTime(self.timeLeft)
    local birdSimulator = player:getBirdSimulator()
    local items = ShopConfig:GetItemInfoByGiftbagid(self.teamId)
 
    if items == nil then
        print("not find novice gift bag reward")
        return
    end
    local birdGains = {}
    for _, item in pairs(items) do 
        local result = BirdConfig:getBirdInfoById(item.itemId)
        if result ~= nil then
            local id = player.userBird:getLastBirdIndex()
            local partIds = BirdConfig:getPartIdsById(item.itemId)
            local status = 0
            if player.userBirdBag ~= nil then
                if player.userBirdBag.curCarryBirdNum < player.userBirdBag.maxCarryBirdNum then
                    status = 1
                end
            end
            player.userBird:createBird(id, item.itemId, 1, 0, status, partIds)
            player:setBirdSimulatorBag()
        else
            player:addProp(item.itemId, item.num)
        end
        if item.itemId ~= 9000001 then
            local birdGain = BirdGain.new()
            birdGain.itemId = item.itemId
            birdGain.num = item.num
            birdGain.icon = item.icon
            birdGain.name = item.name
            table.insert(birdGains, birdGain)
        end     
    end
    if next(birdGains) then
        HostApi.sendBirdGain(player.rakssid, birdGains)
    end
    birdSimulator:setActivity(birdGiftBag)

    return true
end

function NovicePackage:SyncNovicePackageInfo(player)
    local birdSimulator = player:getBirdSimulator()
    if birdSimulator == nil then
        return
    end
    local birdGiftBag = self:SendPackageData(player)

    birdGiftBag.timeLeft = birdGiftBag.timeLeft <= 0 and -1 or birdGiftBag.timeLeft
    birdSimulator:setActivity(birdGiftBag)
end

function NovicePackage:SendPackageData(player)
    local countDonw = player:getPackageTime(self.timeLeft)

    local birdGiftBag = BirdGiftBag.new()
    birdGiftBag.id = self.teamId
    birdGiftBag.price = self.price
    birdGiftBag.limit = self.limit
    birdGiftBag.discount = self.discount
    birdGiftBag.currencyType = self.moneyType
    birdGiftBag.icon = self.pic or ""
    birdGiftBag.desc = self.desc or ""
    birdGiftBag.name = self.name or ""
    birdGiftBag.topLeftIcon = self.topLeftBg or ""
    birdGiftBag.topLeftName = self.topLeftDesc or ""
    birdGiftBag.topRightIcon = self.topRightBg or ""
    birdGiftBag.topRightName = self.topRightDesc or ""
    birdGiftBag.timeLeft = countDonw
    birdGiftBag.items = {}
    local items = {}
    for _, itemInfo in pairs(self.itemInfos) do
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
