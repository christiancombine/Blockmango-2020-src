GameLottery = {}

GameLottery.LotteryInfo = {}

function GameLottery:addPlayerLotteryInfo(userId)
    local info = {
        isFirstOpen = true,
        DoneLottery = true,
        lotteryTimes = 1,
        lotteryRefreshTimes = 0,
        lotteryPool = {},
        lotteryCard = { [1] = true, [2] = true, [3] = true, [4] = true, [5] = true, [6] = true },
        replaceRewardId = "",
        lotteryCardNum = 6
    }
    self.LotteryInfo[tostring(userId)] = info
end

function GameLottery:onPlayerLottery(userId, seq)
    local player = PlayerManager:getPlayerByUserId(userId)
    local lotteryInfo = self.LotteryInfo[tostring(userId)]
    if not lotteryInfo then
        return
    end
    seq = seq or lotteryInfo.lotteryCardNum
    if not seq or not player or lotteryInfo.lotteryPool == nil or lotteryInfo.DoneLottery == false or lotteryInfo.lotteryCard[seq] == false then
        return
    end
    lotteryInfo.DoneLottery = false
    if not GameLottery:checkCanAddItem(player) then
        lotteryInfo.DoneLottery = true
        return
    end
    local lotteryConfig = LotteryPricesConfig:getLotteryPrices(lotteryInfo.lotteryTimes)
    if not lotteryConfig then
        lotteryInfo.DoneLottery = true
        return
    end
    if not GameLottery:checkIsHaveMoney(player, lotteryConfig) then
        lotteryInfo.DoneLottery = true
        return
    end
    local reward = LotteryRewardConfig:getRewardById(lotteryInfo.lotteryPool[1])
    if not reward or not reward.Id then
        lotteryInfo.DoneLottery = true
        return
    end
    lotteryInfo.lotteryCard[seq] = false
    lotteryInfo.lotteryCardNum = lotteryInfo.lotteryCardNum - 1
    table.remove(lotteryInfo.lotteryPool, 1)
    local rewardId = GameReward:onPlayerGetReward(player, reward)    --   调用给奖品方法
    self:consumeMoney(player, lotteryConfig.Type, lotteryConfig.Prices, "key_GameLottery")
    ReportUtil.reportLotteryOpenCard(player, lotteryInfo.lotteryTimes)
    lotteryInfo.lotteryTimes = lotteryInfo.lotteryTimes + 1
    lotteryConfig = LotteryPricesConfig:getLotteryPrices(lotteryInfo.lotteryTimes)
    if rewardId then
        GamePacketSender:sendBedWarLotteryResult(player.rakssid, reward.Id, lotteryConfig.Prices, seq)
        lotteryInfo.DoneLottery = true
        lotteryInfo.oldGoods = nil
        lotteryInfo.newGoods = nil
    else
        GamePacketSender:sendBedWarLotteryResult(player.rakssid, reward.Id, lotteryConfig.Prices, seq)
        if reward.RewardType == Define.RewardType.STORE then
            lotteryInfo.replaceRewardId = reward.RewardId
            GameLottery:sendTipAfterDelay(player.userId, reward)
        elseif reward.RewardType == Define.RewardType.PRIVILEGE then
            lotteryInfo.DoneLottery = true
        end
    end

    if lotteryInfo.lotteryCardNum == 0 then
        GamePacketSender:sendBedWarLotteryFreeRefresh(player.rakssid)
    end

end

function GameLottery:RefreshLotteryPool(userId)
    GamePlayerControl.printId = math.fmod(GamePlayerControl.printId, 12) + 1
    local lotteryInfo = self.LotteryInfo[tostring(userId)]
    if not lotteryInfo then
        return
    end
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local RefreshPrices = RefreshPricesConfig:getRefreshPrices(lotteryInfo.lotteryRefreshTimes)
    if not RefreshPrices then
        return
    end

    if lotteryInfo.lotteryCardNum == 0 then
        self:lotteryRefresh(player, lotteryInfo)
        return
    end

    if not GameLottery:checkIsHaveMoney(player, RefreshPrices) then
        return
    end

    if self:consumeMoney(player, RefreshPrices.Type, RefreshPrices.Prices, "key_LotteryRefresh") then
        self:lotteryRefresh(player, lotteryInfo)
    end
end

function GameLottery:lotteryRefresh(player, lotteryInfo)
    for i, _ in pairs(lotteryInfo.lotteryCard) do
        lotteryInfo.lotteryCard[i] = true
    end
    lotteryInfo.lotteryCardNum = 6
    lotteryInfo.isFirstOpen = false
    local rewardLv = RewardLevelConfig:getRewardLevel()
    local coreItem = LotteryRewardConfig:getRandomCoreItem(rewardLv.Number)
    local commonItem = LotteryRewardConfig:getRandomCommonItem(6 - rewardLv.Number)
    lotteryInfo.lotteryPool = self:lotteryPoolSort(commonItem, coreItem)
    lotteryInfo.lotteryTimes = 1
    lotteryInfo.lotteryRefreshTimes = lotteryInfo.lotteryRefreshTimes + 1
    local RefreshConfig = RefreshPricesConfig:getRefreshPrices(lotteryInfo.lotteryRefreshTimes)
    local LotteryConfig = LotteryPricesConfig:getLotteryPrices(lotteryInfo.lotteryTimes)
    GamePacketSender:sendBedWarRefreshResult(player.rakssid, commonItem, coreItem, RefreshConfig.Prices, LotteryConfig.Prices)
end

function GameLottery:lotteryPoolSort(commonItem, coreItem)
    local weightSum = 0
    local allItem = {}
    local result = {}
    for _, item in pairs(commonItem) do
        table.insert(allItem, item)
    end
    for _, item in pairs(coreItem) do
        table.insert(allItem, item)
    end
    for _, itemId in pairs(allItem) do
        weightSum = weightSum + LotteryRewardConfig:getRewardById(itemId).Weight
    end

    while (TableUtil.getTableSize(allItem) ~= 0) do
        local rate = math.random(1, weightSum)
        local sum = 0
        for i, itemId in pairs(allItem) do
            local itemWeight = LotteryRewardConfig:getRewardById(itemId).Weight
            sum = BaseUtil:incNumber(sum, itemWeight)
            if sum >= rate then
                table.insert(result, itemId)
                table.remove(allItem, i)
                weightSum = weightSum - itemWeight
                break
            end
        end
    end
    return result
end

function GameLottery:consumeMoney(player, moneyType, Prices, source)
    if moneyType == Define.Money.KEY and MoneyUtil:checkHasMoney(player, moneyType, Prices) then
        player:subYaoShi(Prices, source, Prices)
        return true
    end
    return false
end

function GameLottery:checkCanAddItem(player)
    if not player:getInventory():isCanAddItem(268, 0, 1) then
        local params = {
            AdType = Define.AdType.Full_Inventory,
            Title = "gui_dialog_tip_title_tip",
            Content = "lottery_full_Inventory",
            SureButtonTxt = "gui_dialog_tip_btn_sure",
        }
        local data = DataBuilder.new():fromTable(params):getData()
        GamePacketSender:sendBedWarTipAd(player.rakssid, data)
        return false
    end
    return true
end

function GameLottery:checkIsHaveMoney(player, Config)
    local lotteryInfo = self.LotteryInfo[tostring(player.userId)]
    if not MoneyUtil:checkHasMoney(player, Config.Type, Config.Prices) then
        lotteryInfo.DoneLottery = true
        return false
    end
    return true
end

function GameLottery:sendTipAfterDelay(userId, reward)
    local lotteryInfo = self.LotteryInfo[tostring(userId)]

    local c_player = PlayerManager:getPlayerByUserId(userId)
    if not c_player then
        return
    end
    local newGoods = StoreConfig:getGoodsById(reward.RewardId)
    if not newGoods then
        lotteryInfo.DoneLottery = true
        return
    end
    local oldGoods = StoreConfig:getEquipArmorGoods(c_player, newGoods.LevelGroup)
    if not oldGoods then
        lotteryInfo.DoneLottery = true
        return
    end
    lotteryInfo.oldGoods = oldGoods
    if newGoods.Level > oldGoods.Level then
        lotteryInfo.DoneLottery = true
        return
    end
    lotteryInfo.newGoods = newGoods
end

function GameLottery:onLotteryReplace(userId, isReplace)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    self.LotteryInfo[tostring(userId)].DoneLottery = true
    if isReplace then
        GameStore:onPlayerLotteryReplace(userId, self.LotteryInfo[tostring(userId)].replaceRewardId)
    end
end

function GameLottery:onLotteryEffectEnd(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local info = self.LotteryInfo[tostring(userId)]
    info.DoneLottery = true
    if info.oldGoods ~= nil and info.newGoods ~= nil then
        GamePacketSender:sendLotteryReplaceTip(player.rakssid, info.oldGoods.Id, info.newGoods.Id)
    end
end
