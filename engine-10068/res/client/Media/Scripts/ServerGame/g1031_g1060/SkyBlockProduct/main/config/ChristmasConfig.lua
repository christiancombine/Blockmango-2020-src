ChristmasConfig = {}
ChristmasConfig.List = {}
ChristmasConfig.Type =
{
    BuyType = 1,
    UseType = 2,
}
function ChristmasConfig:initConfig(configs)
    for _, config in pairs(configs) do
        local item = {
            Id = tonumber(config.Id) or 1,
            GiftType = tonumber(config.GiftType) or 1,
            ItemId = tonumber(config.ItemId) or 0,
            GiftBlockId = tonumber(config.GiftBlockId) or 0,
            Num = tonumber(config.Num) or 1,
            Name = tostring(config.Name) or "",
            Img = tostring(config.Img) or "",
            BuyPriceType = tonumber(config.BuyPriceType) or 0,
            BuyPriceNum = tonumber(config.BuyPriceNum) or 0,
            OpenType = tonumber(config.OpenType) or 0,
            OpenMoneyNum = tonumber(config.OpenMoneyNum) or 0,
            OpenDiamondNum = tonumber(config.OpenDiamondNum) or 0,
            Reward = StringUtil.split(tostring(config.Reward), "#") or {},
            Weights = StringUtil.split(tostring(config.Weights), "#") or {},
        }
        table.insert(self.List, item)
    end
end


function ChristmasConfig:getGiftdata()
    return self.List or {}
end

function ChristmasConfig:getGiftdataByType(giftType)
    for _, v in pairs(self.List) do
        if tonumber(giftType) == v.GiftType then
            return v
        end
    end
    return nil
end

function ChristmasConfig:buyChirstmasGift(userId, giftType)
    print(" buyChirstmasGift userId == "..tostring(userId).." giftType == "..giftType)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local cfg  = self:getGiftdataByType(giftType)
    if cfg == nil  then
        return
    end

    local inv = player:getInventory()
    if not inv:isCanAddItem(cfg.ItemId, 0, cfg.Num) then
        HostApi.sendCommonTip(player.rakssid, "gui_inventory_not_enough")
        return
    end

    if player:checkMoney(cfg.BuyPriceType, cfg.BuyPriceNum) == false then
        return
    end

    if cfg.BuyPriceType == Define.Money.DIAMOND then
        player:consumeDiamonds(1200, cfg.BuyPriceNum, Define.buyChirstmasGift.. "#" .. tostring(giftType))
        return
    end

    if cfg.BuyPriceType == Define.Money.MONEY then
        player:subCurrency(cfg.BuyPriceNum)
        GameAnalytics:design(player.userId, 0, { "g1048subMoney", tostring(player.userId), cfg.BuyPriceNum})
        self:buyChirstmasGiftSuccess(player, giftType)
    end
end

function ChristmasConfig:buyChirstmasGiftSuccess(player, giftType)
    print(" buyChirstmasGiftSuccess userId == "..tostring(player.userId).." giftType == "..giftType)
    if player == nil then
        return
    end

    local cfg = self:getGiftdataByType(giftType)
    if cfg == nil  then
        return
    end

    HostApi.notifyGetItem(player.rakssid, cfg.ItemId, 0, cfg.Num)
    player:addItem(cfg.ItemId, cfg.Num, 0)
    if cfg.ItemId == Define.ChristmasBox.COMMON then
        local day = DateUtil.getDate()
        GameAnalytics:design(player.userId, 0, { "g1048getCommonBoxEveryDay", tostring(day)})
    end
    if cfg.ItemId == Define.ChristmasBox.FINE then
        local day = DateUtil.getDate()
        GameAnalytics:design(player.userId, 0, { "g1048getSeniorBoxEveryDay", tostring(day)})
    end
    player:sendGiftNum()
    if player.enableIndie then
        GameAnalytics:design(player.userId, 0, { "g1048buy_box_suc_indie", cfg.ItemId})
    else
        GameAnalytics:design(player.userId, 0, { "g1048buy_box_suc", cfg.ItemId})
    end
end

function ChristmasConfig:openChirstmasGift(player, data)
    print(" openChirstmasGift userId == "..tostring(player.userId).." giftType == "
            ..tostring(data.giftType).."  openType == "..tostring(data.openType))

    if player == nil then
        return
    end
    local cfg = self:getGiftdataByType(data.giftType)
    if cfg == nil  then
        return
    end

    if cfg.ItemId == Define.ChristmasBox.COMMON then
        if player.enableIndie then
            GameAnalytics:design(player.userId, 0, { "g1048common_click_indie", Define.ClickType.TYPE1})
        else
            GameAnalytics:design(player.userId, 0, { "g1048common_click", Define.ClickType.TYPE1})
        end
    end
    if cfg.ItemId == Define.ChristmasBox.FINE then
        if player.enableIndie then
            GameAnalytics:design(player.userId, 0, { "g1048common_click_indie", Define.ClickType.TYPE2})
        else
            GameAnalytics:design(player.userId, 0, { "g1048common_click", Define.ClickType.TYPE2})
        end
    end

    local inv = player:getInventory()
    if not inv:isCanAddItem(cfg.ItemId, 0, cfg.Num) then
        HostApi.sendCommonTip(player.rakssid, "gui_inventory_not_enough")
        return
    end

    local cur_num = player:getItemNumById(cfg.ItemId, 0)
    if cur_num <= 0 then
        return
    end

    if data.openType == Define.GiftType.commonGift then
        if cfg.OpenType == Define.openGiftType.MONEY_OPEN then
            if player:checkMoney(Define.Money.MONEY, cfg.OpenMoneyNum) then
                player:subCurrency(cfg.OpenMoneyNum)
                GameAnalytics:design(player.userId, 0, { "g1048subMoney", tostring(player.userId), cfg.OpenMoneyNum})
                self:openChirstmasGiftSuccess(player, cfg.GiftType, Define.DrawGiftType.Money)
            end
        end
        -- GameAnalytics:design(player.userId, 0, { "g1048draw_suc", Define.GiftType.commonGift})
        return
    end

    if data.openType == Define.GiftType.fineGift then
        -- if cfg.OpenType == Define.openGiftType.DIAMOND_OPEN then
            if player.canSendCostDiamond == false then
                return
            end

            if player:checkMoney(Define.Money.DIAMOND, cfg.OpenDiamondNum) then
                player.canSendCostDiamond = false
                player:consumeDiamonds(1200, cfg.OpenDiamondNum, Define.openGift .. "#" .. cfg.GiftType)
            end
        -- GameAnalytics:design(player.userId, 0, { "g1048draw_suc",  Define.GiftType.fineGift})
        -- end
        return
    end
end

function ChristmasConfig:openChirstmasGiftSuccess(player, giftType, drawType)
    print(" openChirstmasGiftSuccess userId == "..tostring(player.userId).." giftType == "..tostring(giftType))

    if player == nil then
        return
    end
    local cfg = self:getGiftdataByType(giftType)
    if cfg == nil  then
        return
    end

    local cur_num = player:getItemNumById(cfg.ItemId, 0)
    if cur_num <= 0 then
        return
    end 

    local rewardId = self:lotteryGiftReward(player, giftType)
    if rewardId == nil then
        return
    end

    local rewardCfg = GiftRewardConfig:getRewardById(rewardId)
    if rewardCfg == nil then
        return
    end

    local rewardNum = HostApi.random("rewardNum", tonumber(rewardCfg.Num[1]), tonumber(rewardCfg.Num[2]) + 1 )

    if rewardCfg.Type == GiftRewardConfig.rewardType.ITEM then
        -- HostApi.notifyGetGoodsByItemId(player.rakssid, rewardCfg.ItemId, rewardCfg.Meta, rewardNum, rewardCfg.Icon)
        player:removeItem(cfg.ItemId, 1, 0)
        player:addItem(rewardCfg.ItemId, rewardNum, rewardCfg.Meta)
    end

    if rewardCfg.Type == GiftRewardConfig.rewardType.MONEY then
        -- HostApi.notifyGetItem(self.rakssid, 10000, 0, rewardNum)
        player:removeItem(cfg.ItemId, 1, 0)
        player:addCurrency(rewardNum)
    end

    local rewardData = 
    {
        Id = rewardCfg.Id,
        ItemId = rewardCfg.ItemId,
        Meta = rewardCfg.Meta,
        Num = rewardNum,
        Icon = rewardCfg.Icon,
        Name = rewardCfg.Name,
        Detail = rewardCfg.Detail,
        Type =  rewardCfg.Type,
    }

    player:sendGiftNum()
    GamePacketSender:sendOpenGiftReward(player.rakssid, rewardData)
    GameAnalytics:design(player.userId, 0, { "g1048draw_suc",  drawType})
    print(" openChirstmasGiftSuccess userId == "..tostring(player.userId).." giftType == "..tostring(giftType) .. " drawType == ".. tostring(drawType))
end

function ChristmasConfig:lotteryGiftReward(player, giftType)
    local cfg = self:getGiftdataByType(giftType)
    if cfg == nil  then
        return nil
    end

    local weights = 0
    for k, v in pairs(cfg.Weights) do
        weights = weights + tonumber(v)
    end

    if weights == 0 then
        return nil
    end

    local rewardIndex = 1
    local randWeight = HostApi.random("openGift", 0, weights)
    local cur_Weights = 0
    print("weight == "..tostring(weights).. "  randWeight == "..tostring(randWeight))
    for i, v in ipairs(cfg.Weights) do
        if randWeight >= cur_Weights and randWeight <= cur_Weights + tonumber(v) then
            rewardIndex = i
        end
        cur_Weights = cur_Weights + tonumber(v)
    end

    if cfg.Reward[rewardIndex] then
        return cfg.Reward[rewardIndex]
    end

    return nil
end

return ChristmasConfig