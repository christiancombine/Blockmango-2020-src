ActivityListener = {}

function ActivityListener:init()
    RotateOperationEvent:registerCallBack(self.onRotateOperation)
    GameOperationEvent:registerCallBack(self.onGameOperation)
    BlockFortOperationEvent:registerCallBack(self.onBlockFortOperation)
    GameTradeEvent:registerCallBack(self.onGameTrade)
end

function ActivityListener.onRotateOperation(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return false
    end

    --type: 1->decoration  ||  2->schematic
    --direction: 1->turn left   ||  2->turn right
    local type = DataBuilder.new():fromData(data):getNumberParam("type")
    local direction = DataBuilder.new():fromData(data):getNumberParam("direction")
    local id = DataBuilder.new():fromData(data):getNumberParam("id")

    if type == 1 then
        player:rotateDecoration(direction)
    elseif type == 2 then
        player:rotateSchematic(direction)
    end
end

function ActivityListener.onGameOperation(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    -- type: 1 -> go home, 2 -> go downtown 3 -> clear hotbar 4->upgrade manor
    local type = DataBuilder.new():fromData(data):getNumberParam("operationType")
    if type == 1 then
        local manorInfo = SceneManager:getUserManorInfo(player.userId)
        if not manorInfo then
            EngineUtil.sendEnterOtherGame(player.rakssid, "g1052", player.userId)
            return
        end

        local pos = ManorConfig:getInitPosByIndex(manorInfo.manorIndex, false)
        local yaw = ManorConfig:getInitYawByIndex(manorInfo.manorIndex)
        if pos then
            player:teleportPosWithYaw(pos, yaw)
        end
        return
    end

    if type == 2 then
        player:teleportPosWithYaw(GameConfig.teleportPos, GameConfig.teleportYaw)
        return
    end

    --if type == 3 then
    --    player:clearInv()
    --    return
    --end

    if type == 4 then
        local manorInfo = SceneManager:getUserManorInfo(player.userId)
        if not manorInfo then
            return
        end

        if manorInfo:isAlreadyMaxLevel() then
            --HostApi.showBlockCityCommonTip(player.rakssid, {}, 0, "manor_level_up_max")
            return
        end

        if not player.curManorIndex == player.manorIndex then
            return
        end

        local totalMoney = ManorLevelConfig:getNextPriceByLevel(manorInfo.level)
        local moneyType = ManorLevelConfig:getMoneyTypeByLevel(manorInfo.level)
        if moneyType == 1 then
            player:consumeDiamonds(1300 + manorInfo.level, totalMoney, Messages.upgradeManor .. ":", true)
        elseif moneyType == 2 then
            player:consumeGolds(1300 + manorInfo.level, totalMoney, Messages.upgradeManor .. ":", true)
        end
        return
    end
end

function ActivityListener.onBlockFortOperation(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    --operationType: 1 Remove Building /  2 Upgrade Building
    local operationType = DataBuilder.new():fromData(data):getNumberParam("operationType")
    local id = DataBuilder.new():fromData(data):getNumberParam("id")
    local index = DataBuilder.new():fromData(data):getNumberParam("index")

    if operationType == 1 then

        local item = StoreConfig:getItemInfoById(id)
        if not item then
            return
        end

        player:recallItems(index, item.type)
    end
end

function ActivityListener.onGameTrade(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    --type: 1 Buy /  2 Sell
    local id = DataBuilder.new():fromData(data):getNumberParam("id")
    local num = DataBuilder.new():fromData(data):getNumberParam("num")
    local type = DataBuilder.new():fromData(data):getNumberParam("type")

    local itemInfo = StoreConfig:getItemInfoById(id)
    if not itemInfo then
        return
    end

    local price = StoreConfig:getItemPriceById(id, itemInfo.moneyType)
    if not price then
        return
    end

    if type == 1 and player:isCanBuying(id)
            and not player:getIsWaitForPay()
            and player.manorLevel >= itemInfo.level
    then
        player:setIsWaitForPay(true)
        local moneyType = itemInfo.moneyType
        local totalMoney = price * num
        local itemId = itemInfo.itemId
        local keyInfo = Messages.buyItem .. ":" .. id .. ":" .. itemInfo.meta .. ":" .. num .. ":" .. itemId .. ":" .. itemInfo.limit --message, id, meta, num, itemId, limit
        if moneyType == MoneyType.Cube then
            player:consumeDiamonds(itemInfo.id, totalMoney, keyInfo, true)
        elseif moneyType == MoneyType.Gold then
            player:consumeGolds(itemInfo.id, totalMoney, keyInfo, true)
        end
    end
end

return ActivityListener