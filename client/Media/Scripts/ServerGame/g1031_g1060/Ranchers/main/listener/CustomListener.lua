CustomListener = {}

function CustomListener:init()
    ConsumeDiamondsEvent.registerCallBack(self.onConsumeDiamonds)
end

function CustomListener.onConsumeDiamonds(rakssid, isSuccess, message, extra, payId)
    if isSuccess == false then
        if message == "diamonds.not.enough" then
            HostApi.sendCommonTip(rakssid, "ranch_tip_not_enough_money")
        elseif message == "pay.failed" then
            HostApi.sendCommonTip(rakssid, "ranch_tip_pay_failure")
        elseif message == "diamondBlues.not.enough" then
            --do nothing
        else
            HostApi.sendCommonTip(rakssid, "ranch_tip_something_error")
        end
        EngineUtil.disposePlatformOrder(0, payId, false)
        return false
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] CustomListener.onConsumeDiamonds : player rakssid[".. tostring(rakssid) .. "] is nil")
        EngineUtil.disposePlatformOrder(0, payId, false)
        return false
    end

    local extras = StringUtil.split(extra, ":")
    if extras[1] == RanchersWebRequestType.refreshOrder then
        if player.task == nil then
            EngineUtil.disposePlatformOrder(player.userId, payId, false)
            return false
        end

        local orderId = tonumber(extras[2])
        if player.task:isTaskCanRefreshById(orderId) == false then
            local ranchTask = player.task:initRanchTask()
            player:getRanch():setOrders(ranchTask)
            EngineUtil.disposePlatformOrder(player.userId, payId, false)
            return false
        end

        player.task:refreshTaskByIndex(orderId, player.level)
        player.task:updateRefreshTimeById(orderId)
        local ranchTask = player.task:initRanchTask()
        player:getRanch():setOrders(ranchTask)
        EngineUtil.disposePlatformOrder(player.userId, payId, true)
        return true
    end

    if extras[1] == RanchersWebRequestType.productionSpeedUp and extras[2] ~= nil and extras[3] ~= nil then

        if player.wareHouse == nil then
            EngineUtil.disposePlatformOrder(player.userId, payId, false)
            return false
        end

        if player.building == nil then
            EngineUtil.disposePlatformOrder(player.userId, payId, false)
            return false
        end

        local entityId = tonumber(extras[2])
        local buildingInfo = player.building:getBuildingInfoByEntityId(entityId)
        if buildingInfo == nil then
            EngineUtil.disposePlatformOrder(player.userId, payId, false)
            return false
        end

        local queueIndex = player.building:getFirstIdleQueueId(buildingInfo.id)
        if queueIndex == 0 then
            EngineUtil.disposePlatformOrder(player.userId, payId, false)
            return false
        end

        local productId = tonumber(extras[3])
        local needTime = ItemsConfig:getNeedTimeByItemId(productId)
        local recipeItems = ItemsConfig:getRecipeItemsById(productId)
        player.wareHouse:subItemsViaUseCubeMoney(recipeItems)

        local wareHouse = player.wareHouse:initRanchWareHouse()
        player:getRanch():setStorage(wareHouse)

        player.building:addWorkingQueue(buildingInfo.id, queueIndex, productId, needTime)

        local nextQueueStartTime = os.time()
        player.building:startFactoryQueueWorking(buildingInfo.id, nextQueueStartTime)
        player.building:startFarmingQueueWorking(buildingInfo.id)

        local ranchBuilding = player.building:getBuildingInfoById(buildingInfo.id)
        player.building:initRanchBuildingQueue(ranchBuilding)
        EngineUtil.disposePlatformOrder(player.userId, payId, true)
        return true
    end

    if extras[1] == RanchersWebRequestType.upgradeWareHouse then
        if player.wareHouse == nil then
            EngineUtil.disposePlatformOrder(player.userId, payId, false)
            return false
        end

        local wareHouseMaxLevel = WareHouseConfig:getMaxLevel()
        if player.wareHouse.level >= wareHouseMaxLevel then
            EngineUtil.disposePlatformOrder(player.userId, payId, false)
            return false
        end

        local upgradeMoney = WareHouseConfig:getUpgradeMoneyByLevel(player.wareHouse.level)
        if player.money < upgradeMoney then
            EngineUtil.disposePlatformOrder(player.userId, payId, false)
            return false
        end

        local upgradeItems = WareHouseConfig:getUpgradeItemsByLevel(player.wareHouse.level)
        if upgradeItems == nil then
            EngineUtil.disposePlatformOrder(player.userId, payId, false)
            return false
        end

        player:subMoney(upgradeMoney)
        player.wareHouse:subItemsViaUseCubeMoney(upgradeItems)
        player.wareHouse:upgradeWareHouse(player.userId, upgradeMoney)
        EngineUtil.disposePlatformOrder(player.userId, payId, true)
        return true
    end

    if extras[1] == RanchersWebRequestType.ranchHelpFinish and extras[2] ~= nil and extras[3] ~= nil and extras[4] ~= nil then
        local itemId = extras[2]
        local num = extras[3]
        local uniqueId = extras[4]
        local fullBoxNum = extras[5]
        local askForHelpId = extras[6]
        local data = {}
        data.itemId = tonumber(itemId)
        data.num = tonumber(num)
        local items = {}
        table.insert(items, data)
        local userId = player.userId
        RanchersWeb:finishHelp(player.userId, player.userId, player.name, player:getSex(), uniqueId, askForHelpId, function (result)
            local callBackPlayer = PlayerManager:getPlayerByUserId(userId)
            if callBackPlayer == nil then
                EngineUtil.disposePlatformOrder(0, payId, false)
                return false
            end

            if result ~= nil and callBackPlayer.wareHouse ~= nil then
                callBackPlayer.wareHouse:subItemsViaUseCubeMoney(items)
                local ranchWareHouse = callBackPlayer.wareHouse:initRanchWareHouse()
                callBackPlayer:getRanch():setStorage(ranchWareHouse)

                RanchersWeb:updateOrder(callBackPlayer.userId, uniqueId, fullBoxNum + 1)
                HostApi.sendRanchOrderHelpResult(rakssid, "ranch_tip_help_success")
                EngineUtil.disposePlatformOrder(callBackPlayer.userId, payId, true)
                if callBackPlayer.achievement ~= nil then
                    callBackPlayer.achievement:addCount(AchievementConfig.TYPE_SOCIAL, AchievementConfig.TYPE_SOCIAL_TASKS_HELP, 0, 1)
                    local ranchAchievements = callBackPlayer.achievement:initRanchAchievements()
                    callBackPlayer:getRanch():setAchievements(ranchAchievements)
                end
                return true
            else
                EngineUtil.disposePlatformOrder(callBackPlayer.userId, payId, false)
                return false
            end
        end)
    end

end

return CustomListener