TowerStore = {}

function TowerStore:init()
    --TODO
end

function TowerStore:onBuyPayItem(player, itemId)
    if not player then
        return
    end

    local config = TowerStoreConfig:getConfigById(itemId)
    if not config then
        return
    end

    if player.towerList and player.towerList[config.towerId] ~= nil then
        return
    end

    if config.moneyType == Define.Money.HallGold then
        if player.Wallet:costMoney(Define.Money.HallGold, config.price) then
            self:onBuyPayItemSuccess(player, itemId)
        end
    elseif config.moneyType == Define.Money.DiamondsGold then
        PayHelper.payDiamondsSync(player.userId, config.id, config.price, function(player)
            self:onBuyPayItemSuccess(player, itemId)
        end, nil)
    end
end

function TowerStore:onBuyPayItemSuccess(player, itemId)
    local config = TowerStoreConfig:getConfigById(itemId)
    local reward = RewardConfig:getRewardById(tonumber(config.rewardId))
    GameReward:onPlayerGetReward(player, reward)
    player:addNewTowerBook(config.towerId)

    GamePacketSender:sendTowerDefenseGetNewTower(player.rakssid, player.towerList[config.towerId])
    --通知玩家购买成功
    GamePacketSender:sendTowerDefenseBuySuccess(player.rakssid, itemId)
    ReportUtil.reportBuyGoodsSuccess(player, itemId)
end


function TowerStore:onNeedBuyHallGold(player, extraDiamondsNum, itemId)
    if not player then
        return
    end

    local config = TowerStoreConfig:getConfigById(itemId)
    if not config then
        return
    end

    if player.towerList and player.towerList[config.towerId] ~= nil then
        return
    end

    PayHelper.payDiamondsSync(player.userId, config.id, extraDiamondsNum, function(player)
        self:onBuyHallGoldSuccess(player, itemId, extraDiamondsNum)
    end, nil)
end

function TowerStore:onBuyHallGoldSuccess(player, itemId, extraDiamondsNum)
    local config = TowerStoreConfig:getConfigById(itemId)

    player.Wallet:addMoney(Define.Money.HallGold, extraDiamondsNum * GameConfig.ExchangeRate)

    if config.moneyType == Define.Money.HallGold then
        if player.Wallet:costMoney(Define.Money.HallGold, config.price) then
            self:onBuyPayItemSuccess(player, itemId)
        end
    end
end

return TowerStore