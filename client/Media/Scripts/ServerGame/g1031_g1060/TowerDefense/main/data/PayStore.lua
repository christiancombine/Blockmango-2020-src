PayStore = {}

function PayStore:init()
    --TODO
end

function PayStore:onBuyPayItem(player, itemId, num)
    if not player or num <= 0 then
        return
    end

    local config = PayStoreConfig:getConfigById(itemId)
    if not config or player.payItemList[tostring(config.itemType)] then
        return
    end

    if itemId == 2003 and player.exchangeLimit < num * GameConfig.GameGoldExchangeRate then
        return
    end

    if config.moneyType == Define.Money.HallGold then
        if player.Wallet:costMoney(Define.Money.HallGold, config.price * num) then
            self:onBuyPayItemSuccess(player, itemId, num)
        end
    elseif config.moneyType == Define.Money.DiamondsGold then
        PayHelper.payDiamondsSync(player.userId, config.id, config.price * num, function(player)
            self:onBuyPayItemSuccess(player, itemId, num)
        end, nil)
    end
end

function PayStore:onBuyPayItemSuccess(player, itemId, num)
    local config = PayStoreConfig:getConfigById(itemId)

    if config.itemType == Define.PayStoreItem.ShortcutBarOne or config.itemType == Define.PayStoreItem.ShortcutBarTwo then
        player.payItemList[tostring(config.itemType)] = config.itemType
        ReportUtil.reportPlayerBuyShortCutBar(player, itemId)
    end

    if config.itemType == 3 then
        local exchangeNum = num * GameConfig.GameGoldExchangeRate
        player.Wallet:addMoney(Define.Money.GameGold, exchangeNum)
        player.totalMoney = player.totalMoney + exchangeNum
        player.exchangeLimit = player.exchangeLimit - exchangeNum
    end

    GamePacketSender:sendBuyPayItemSuccess(player.rakssid, itemId , num)
    --通知玩家购买成功
end

return PayStore