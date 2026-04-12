require "messages.Messages"
require "data.GamePlayer"
require "web.RanchersWeb"
require "Match"
require "config.SceneManager"
require "config.FieldLevelConfig"
require "config.MailLanguageConfig"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerDynamicAttrEvent, self.onDynamicAttrInit)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerCallOnTargetManorEvent.registerCallBack(self.onPlayerCallOnTargetManor)
    PlayerBuyRanchItemResultEvent.registerCallBack(self.onPlayerBuyRanchItemResult)
    PlayerRanchReceiveEvent.registerCallBack(self.onPlayerRanchReceive)
    PlayerRanchExpandEvent.registerCallBack(self.onPlayerRanchExpand)
    PlayerRanchUpgradeHouseEvent.registerCallBack(self.onPlayerRanchUpgradeHouse)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerRanchShortcutEvent.registerCallBack(self.onPlayerRanchShortcut)
    PlayerRanchDestroyHouseEvent.registerCallBack(self.onPlayerRanchDestroyHouse)
    PlayerRanchSellItemEvent.registerCallBack(self.onPlayerRanchSellItem)
    PlayerBuyRanchBuildItemResultEvent.registerCallBack(self.onPlayerBuyRanchBuildItemResult)
    PlayerRanchUpgradeStorageEvent.registerCallBack(self.onPlayerRanchUpgradeStorage)
    PlayerTakeAwayRanchBuildCargoEvent.registerCallBack(self.onPlayerTakeAwayRanchBuildCargo)
    PlayerRanchBuildingQueueProductEvent.registerCallBack(self.onPlayerRanchBuildingQueueProduct)
    PlayerRanchBuildingQueueSpeedUpEvent.registerCallBack(self.onPlayerRanchBuildingQueueSpeedUp)
    PlayerRanchBuildingQueueUnlockEvent.registerCallBack(self.onPlayerRanchBuildingQueueUnlock)
    PlayerAskForHelpRanchOrderEvent.registerCallBack(self.onPlayerAskForHelpRanchOrder)
    PlayerPutIntoRanchOrderCargoEvent.registerCallBack(self.onPlayerPutIntoRanchOrderCargo)
    PlayerReceiveRanchOrderCargoEvent.registerCallBack(self.onPlayerReceiveRanchOrderCargo)
    PlayerRefreshRanchOrderEvent.registerCallBack(self.onPlayerRefreshRanchOrder)
    PlayerRanchHelpFinishEvent.registerCallBack(self.onPlayerRanchHelpFinish)
    PlayerRanchReceiveMailRewardEvent.registerCallBack(self.onPlayerRanchReceiveMailReward)
    PlayerRanchSendEveryDayGiftEvent.registerCallBack(self.onPlayerRanchSendEveryDayGift)
    RefreshPlayerRanchOrderEvent.registerCallBack(self.onRefreshPlayerRanchOrder)
    PlayerRanchReceiveAchievementRewardEvent.registerCallBack(self.onPlayerRanchReceiveAchievementReward)
    PlayerRanchGoExploreEvent.registerCallBack(self.onPlayerRanchGoExplore)
    PlayerRanchSendInviteJoinGameEvent.registerCallBack(self.onPlayerRanchSendInviteJoinGame)
    PlayerBuyRanchTaskItemsResultEvent.registerCallBack(self.onPlayerBuyRanchTaskItemsResult)
    PlayerRanchBuildingProductionSpeedUpEvent.registerCallBack(self.onPlayerRanchBuildingProductionSpeedUp)
    PlayerRanchUseCubeUpgradeStorageEvent.registerCallBack(self.onPlayerRanchUseCubeUpgradeStorage)
    CheckFlyingPermissionEvent.registerCallBack(self.onCheckFlyingPermission)
    BuyFlyingPermissionResultEvent .registerCallBack(self.onBuyFlyingPermissionResult)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    if player == nil then
        return false
    end

    local manorInfo = SceneManager:getUserManorInfoByUid(player.userId)
    if manorInfo ~= nil then
        player.manor = manorInfo
        player.manorIndex = manorInfo.manorIndex
        player.isMaster = true
        local houseInfo = SceneManager:getUserHouseInfoByUid(player.userId)
        if houseInfo ~= nil then
            player.house = houseInfo
        end

        local fieldInfo = SceneManager:getUserFieldInfoByUid(player.userId)
        if fieldInfo ~= nil then
            player.field = fieldInfo
        end

        local wareHouseInfo = SceneManager:getUserWareHouseInfoByUid(player.userId)
        if wareHouseInfo ~= nil then
            player.wareHouse = wareHouseInfo
        end

        local buildingInfo = SceneManager:getUserBuildingInfoByUid(player.userId)
        if buildingInfo ~= nil then
            player.building = buildingInfo
        end

        local taskInfo = SceneManager:getUserTaskInfoByUid(player.userId)
        if taskInfo ~= nil then
            player.task = taskInfo
        end

        local achievementInfo = SceneManager:getUserAchievementInfoByUid(player.userId)
        if achievementInfo ~= nil then
            player.achievement = achievementInfo
        end

        local accelerateItemsInfo = SceneManager:getUserAccelerateItemsInfoByUid(player.userId)
        if accelerateItemsInfo ~= nil then
            player.accelerateItems = accelerateItemsInfo
        end

        local userId = player.userId
        DataBaseManager:getDataFromDB(player.userId, DataBaseManager.TAG_PLAYER)
        RanchersWeb:checkHasLand(player.userId, function(data)
            local callBackPlayer = PlayerManager:getPlayerByUserId(userId)
            if data ~= nil and callBackPlayer ~= nil then
                callBackPlayer.sendGiftCount = data.sendGiftCount
                local ranchInfo = callBackPlayer:initRanchInfo()
                callBackPlayer:getRanch():setInfo(ranchInfo)
            end
        end)

        RanchersWeb:getMyClanProsperity(player.userId, function(data)
            if data == nil then
                return
            end

            local callBackPlayer = PlayerManager:getPlayerByUserId(userId)
            if callBackPlayer ~= nil then
                callBackPlayer:setClanRankData(data)
            end
        end)

        player:getPlayerRanks()
        HostApi.sendPlaySound(player.rakssid, 10032)
        HostApi.log("player[" .. tostring(player.userId) .. "] is master")
    end

    local manorIndex = 0
    local targetUserId = SceneManager:getTargetByUid(player.userId)
    if targetUserId ~= nil then
        local targetManorInfo = SceneManager:getUserManorInfoByUid(targetUserId)
        if targetManorInfo ~= nil then
            manorIndex = targetManorInfo.manorIndex
            MsgSender.sendCenterTipsToTarget(player.rakssid, 5, Messages:welcomeManor(targetManorInfo.name))
        end
    end

    if manorIndex ~= 0 then
        local vec3 = ManorConfig:getInitPosByManorIndex(manorIndex)
        if vec3 ~= nil then
            player.targetManorIndex = manorIndex
            player:teleportPos(vec3)
        end
    end

    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
end

function PlayerListener.onDynamicAttrInit(attr)
    if attr.manorId < 0 then
        return
    end
    local userId = attr.userId
    local targetUserId = attr.targetUserId
    local manorIndex = attr.manorId
    HostApi.log("==== LuaLog: PlayerListener.onDynamicAttrInit : " ..
        " userId = " .. tostring(userId) ..
        ", targetUserId = " .. tostring(targetUserId) ..
        ", manorIndex = " .. manorIndex
    )
    if tostring(targetUserId) ~= "0" and manorIndex > 0 then
        SceneManager:initCallOnTarget(userId, targetUserId)

        local userManorInfo = SceneManager:getUserManorInfoByUid(targetUserId)
        if userManorInfo == nil then

            local manorInfo = GameManor.new(manorIndex, targetUserId)
            SceneManager:initUserManorInfo(targetUserId, manorInfo)

            local houseInfo = GameHouse.new(manorIndex)
            SceneManager:initUserHouseInfo(targetUserId, houseInfo)

            local fieldInfo = GameField.new(manorIndex)
            SceneManager:initUserFieldInfo(targetUserId, fieldInfo)

            local wareHouseInfo = GameWareHouse.new(manorIndex)
            SceneManager:initUserWareHouseInfo(targetUserId, wareHouseInfo)

            local buildingInfo = GameBuilding.new(manorIndex)
            SceneManager:initUserBuildingInfo(targetUserId, buildingInfo)

            local taskInfo = GameTasks.new(targetUserId)
            SceneManager:initUserTaskInfo(targetUserId, taskInfo)

            local achievementInfo = GameAchievement.new(targetUserId)
            SceneManager:initUserAchievementInfo(targetUserId, achievementInfo)

            local accelerateItemsInfo = GameAccelerateItems.new(targetUserId)
            SceneManager:initUserAccelerateItemsInfo(targetUserId, accelerateItemsInfo)

            DataBaseManager:getDataFromDB(targetUserId, DataBaseManager.TAG_MANOR)

        else
            HostApi.log("==== LuaLog: PlayerListener.onDynamicAttrInit : " ..
                " use memory data ... userManorIndex[".. tostring(targetUserId) .. "] = " .. manorIndex
            )
        end
    else
        HostApi.log("==== LuaLog: [error] PlayerListener.onDynamicAttrInit [targetUserId == 0] or [manorIndex <= 0] ")
    end

end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    return false
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end

    local player = PlayerManager:getPlayerByRakssid(movePlayer:getRaknetID())
    if player == nil then
        return true
    end
    ManorConfig:serviceCenterGateTeleport(player, x, y, z)
    ManorConfig:manorGateTeleport(player, x, y, z)

    return true
end

function PlayerListener.onPlayerCallOnTargetManor(userId, targetId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return true
    end

    local manorInfo = SceneManager:getUserManorInfoByUid(targetId)
    if manorInfo == nil then
        RanchersWeb:checkHasLand(targetId, function (data)
            local callBackPlayer = PlayerManager:getPlayerByUserId(userId)
            if callBackPlayer == nil then
                return true
            end
            if data ~= nil then
                EngineUtil.sendEnterOtherGame(callBackPlayer.rakssid, "g1031", targetId)
            else
                HostApi.sendCommonTip(callBackPlayer.rakssid, "ranch_tip_target_has_no_manor")
            end
        end)
        return true
    end

    local manorPos = ManorConfig:getInitPosByManorIndex(manorInfo.manorIndex)
    if manorPos ~= nil then
        SceneManager:initCallOnTarget(userId, targetId)
        player:teleportPos(manorPos)
        HostApi.sendGameStatus(player.rakssid, 1)
        MsgSender.sendCenterTipsToTarget(player.rakssid, 5, Messages:welcomeManor(manorInfo.name))
        player.targetManorIndex = manorInfo.manorIndex
    end

    return true
end

function PlayerListener.onPlayerBuyRanchItemResult(rakssid, isSuccess, items)
    if isSuccess == false then
        return false
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== Lualog: [error] PlayerListener.onPlayerBuyRanchItemResult:  player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    if player.wareHouse == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerBuyRanchItemResult: can not find wareHouseInfo for player[" .. tostring(player.userId).."]")
        return false
    end

    player.wareHouse:addItems(items)

    HostApi.sendRanchGain(rakssid, items)
    local wareHouse = player.wareHouse:initRanchWareHouse()
    player:getRanch():setStorage(wareHouse)
    return true

end

function PlayerListener.onPlayerRanchReceive(rakssid, referrerId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchReceive : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if referrerId ~= 0 and referrerId ~= player.userId then

        local fromUserGiftId = GiftConfig:getGiftIdByType(MailType.Invite)
        local toUserGiftId = GiftConfig:getGiftIdByType(MailType.BeInvited)

        if fromUserGiftId == nil or toUserGiftId == nil then
            return false
        end

        local fromUserReward = GiftConfig:getRewardById(fromUserGiftId)
        local toUserReward = GiftConfig:getRewardById(toUserGiftId)

        if fromUserReward == nil or toUserReward == nil then
            return false
        end

        local fromUserTitle = "xxx"
        local fromContent = {}
        fromContent.items = fromUserReward
        fromContent.msg = "xxx"
        local fromMailLang = MailLanguageConfig:getLangByType(MailType.Invite)
        if fromMailLang ~= nil then
            fromUserTitle = fromMailLang.title
            fromContent.msg = fromMailLang.name
        end

        local toUserTitle = "xxx"
        local toContent = {}
        toContent.items = toUserReward
        toContent.msg = "xxx"
        local toMailLang = MailLanguageConfig:getLangByType(MailType.BeInvited)
        if toMailLang ~= nil then
            toUserTitle = toMailLang.title
            toContent.msg = toMailLang.name
        end

        RanchersWeb:giftSend(player.userId,fromUserTitle, fromContent, MailType.Invite, referrerId, toUserTitle, toContent,  MailType.BeInvited, "g1031")

    end

    RanchersWeb:receiveLand(player.userId)
end

function PlayerListener.onPlayerRanchExpand(rakssid, areaId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchExpand : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    if player.manor == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchExpand: can not find manorInfo of player[" .. tostring(player.userId) .."]")
        return false
    end

    for _, v in pairs(player.manor.unlockedArea) do
        if v == areaId then
            HostApi.log("== LuaLog: This area Id[".. areaId .."] is already unlocked ... ")
            return false
        end
    end

    for i, v in pairs(player.manor.waitForUnlock) do
        if v.id == areaId then
            HostApi.log("== LuaLog: This area Id[".. areaId .."] is waiting for unlocked ... ")
            return false
        end
    end

    local expandInfo = AreaConfig:getExpandInfoByCount(player.manor.unlockedAreaCount)
    if expandInfo == nil then
        HostApi.log("=== LuaLog: PlayerListener.onPlayerRanchExpand : can not find expandInfo by count[" .. player.manor.unlockedAreaCount .. "]")
        return false
    end

    if player.money < expandInfo.money then
        HostApi.sendCommonTip(player.rakssid, "ranch_tip_not_enough_money")
        HostApi.log("=== LuaLog: PlayerListener.onPlayerRanchExpand : not enough money to unlock area player["..tostring(player.userId).."]")
        return false
    end

    if player.prosperity < expandInfo.prosperity then
        HostApi.sendCommonTip(player.rakssid, "ranch_tip_not_enough_prosperity")
        HostApi.log("=== LuaLog: PlayerListener.onPlayerRanchExpand : not enough prosperity to unlock area player["..tostring(player.userId).."]")
        return false
    end

    if player.wareHouse == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchExpand: can not find wareHouseInfo of player[" .. tostring(player.userId) .."]")
        return false
    end

    if player.wareHouse:isHasItems(expandInfo.expandItems) == false then
        HostApi.sendCommonTip(player.rakssid, "ranch_tip_not_enough_items")
        return false
    end

    if player.wareHouse:isEnoughCapacity(expandInfo.rewardItems) == false then
        HostApi.sendRanchWarehouseResult(player.rakssid, "gui_ranch_storage_full_operation_failure")
        return false
    end

    player:subMoney(expandInfo.money)
    player.achievement:addCount(AchievementConfig.TYPE_RESOURCE, AchievementConfig.TYPE_RESOURCE_MONEY_CONSUME, 0, expandInfo.money)

    player.wareHouse:subItems(expandInfo.expandItems)
    player.wareHouse:addItems(expandInfo.rewardItems)

    local items = {}
    local index = 1
    for i, v in pairs(expandInfo.rewardItems) do
        if v.itemId ~= 0 and v.num ~= 0 then
            local item = RanchCommon.new()
            item.itemId = v.itemId
            item.num = v.num
            items[index] = item
            index = index + 1
        end
    end
    HostApi.sendRanchGain(rakssid, items)
    local wareHouse = player.wareHouse:initRanchWareHouse()
    player:getRanch():setStorage(wareHouse)

    player.manor:addWaitForUnlockAreaById(areaId, expandInfo.time)
    player.manor:updateManorNpcInfo()

    player.achievement:addCount(AchievementConfig.TYPE_MANOR, AchievementConfig.TYPE_MANOR_EXPAND, 0, 1)

    local ranchAchievements = player.achievement:initRanchAchievements()
    player:getRanch():setAchievements(ranchAchievements)

    return true
end

function PlayerListener.onPlayerRanchUpgradeHouse(rakssid)

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchUpgradeHouse : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    if player.house == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchUpgradeHouse: can not find houseInfo of player[" .. tostring(player.userId) .."]")
        return false
    end

    local maxHouseLevel = HouseConfig:getMaxHouseLevel()
    if player.house.houseLevel >= maxHouseLevel then
        HostApi.log("=== LuaLog: PlayerListener.onPlayerRanchUpgradeHouse: already max houseLevel of player[" .. tostring(player.userId) .. "] ...")
        return false
    end

    local preHouseInfo = HouseConfig:getHouseInfoByLevel(player.house.houseLevel)
    if preHouseInfo == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchUpgradeHouse: can not find player[".. tostring(player.userId) .."]'s houseIno by level[".. player.house.houseLevel .."]")
        return false
    end

    local upgradeMoney = HouseConfig:getUpgradeMoneyByLevel(player.house.houseLevel)
    if player.money < upgradeMoney then
        return false
    end

    if player.wareHouse == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchUpgradeHouse: can not find wareHouseInfo of player[" .. tostring(player.userId) .."]")
        return false
    end

    local upgradeItems = HouseConfig:getUpgradeItemsByLevel(player.house.houseLevel)
    if upgradeItems == nil or player.wareHouse:isHasItems(upgradeItems) == false then
        HostApi.log("=== LuaLog: PlayerListener.onPlayerRanchUpgradeHouse : has not enough items to upgrade House player["..tostring(player.userId).."]")
        return false
    end

    player:subMoney(upgradeMoney)
    player.achievement:addCount(AchievementConfig.TYPE_RESOURCE, AchievementConfig.TYPE_RESOURCE_MONEY_CONSUME, 0, upgradeMoney)

    player.wareHouse:subItems(upgradeItems)

    local ranchWareHouse = player.wareHouse:initRanchWareHouse()
    player:getRanch():setStorage(ranchWareHouse)

    player.house:upgrade()

    local ranchHouse = player.house:initRanchHouse()
    player:getRanch():setHouse(ranchHouse)

    HostApi.sendCommonTip(rakssid, "ranch_tip_upgrade_successful")

    -- add player's achievement
    player.achievement:addCount(AchievementConfig.TYPE_FACTORY, AchievementConfig.TYPE_FACTORY_HOUSE_LEVEL, 0, 1)

    local ranchAchievements = player.achievement:initRanchAchievements()
    player:getRanch():setAchievements(ranchAchievements)

    local houseInfo = HouseConfig:getHouseInfoByLevel(player.house.houseLevel)
    if houseInfo == nil then
        HostApi.log("=== LuaLog: PlayerListener.onPlayerRanchUpgradeHouse: can not find houseIno by level[".. player.house.houseLevel .."] after upgrade of player["..tostring(player.userId).."]")
        return false
    end

    local offset = player.house.house.offset
    if offset ~= nil and (offset.x ~= 0 or offset.y ~= 0 or offset.z ~= 0) then
        -- house in the world
        if houseInfo.fileName ~= preHouseInfo.fileName then
            local houseOffset = player.house.house.offset
            player.house:removeHouseFromWorld(player.house.houseLevel - 1)
            player.house:setHouseOffset(houseOffset)
            player.house:setHouseToWorld()
        end

    else
        -- house not in the world
        if houseInfo.itemId ~= preHouseInfo.itemId then
            player:removeItem(preHouseInfo.itemId, 1)
            player:addItem(houseInfo.itemId, 1, 0)
        end
    end

end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    return false
end

function PlayerListener.onPlayerRanchShortcut(rakssid, isSuccess, areaId)

    if isSuccess == false then
        return false
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchShortcut : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    if player.manor == nil then
        return false
    end
    player.manor:updateUnlockedAreaStateById(areaId)
end

function PlayerListener.onPlayerRanchDestroyHouse(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchDestroyHouse : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.manorIndex == 0 then
        HostApi.sendCommonTip(player.rakssid, "ranch_tip_not_your_house")
        return false
    end

    local manorInfo = SceneManager:getUserManorInfoByUid(player.userId)
    if manorInfo == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchDestroyHouse : can not find manorInfo of player[".. tostring(player.userId).. "]")
        return false
    end

    local playerPosition = player:getPosition()
    if manorInfo:isInManor(playerPosition) == false then
        HostApi.sendCommonTip(player.rakssid, "ranch_tip_not_your_house")
        return false
    end

    if player.house == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchDestroyHouse: can not find houseInfo of player[" .. tostring(player.userId) .."]")
        return false
    end

    local startPos, endPos = player.house:getHouseStartPosAndEndPos()
    if startPos ~= nil and endPos ~= nil then
        if player.house.isInWorld == true then
            local itemId = player.house:getItemIdOfHouse()
            local inv = player:getInventory()
            if inv:isCanAddItem(itemId, 0, 1) == false then
                HostApi.sendCommonTip(rakssid, "ranch_tip_package_full")
                return false
            end
            player:addItem(itemId, 1, 0)
            player.house:removeHouseFromWorld(player.house.houseLevel)
            local ranchInfo = player:initRanchInfo()
            player:getRanch():setInfo(ranchInfo)
            return true
        end
    end

    return false
end

function PlayerListener.onPlayerRanchSellItem(rakssid, itemId, num)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchSellItem : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.wareHouse == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchSellItem: can not find wareHouseInfo of player[".. tostring(player.userId)"]")
        return false
    end

    if num <= 0 then
        return false
    end

    for i, v in pairs(player.wareHouse.wareHouseItems) do
        if i == itemId then
            local sellNum = 0
            if v > num then
                sellNum = num
            else
                sellNum = v
            end

            local money = player.wareHouse:getSellItemMoney(itemId, sellNum)
            player:addMoney(money)
            player.wareHouse:subItem(itemId, sellNum)
            player.achievement:addCount(AchievementConfig.TYPE_RESOURCE, AchievementConfig.TYPE_RESOURCE_MONEY_SELL, 0, money)

            local ranchAchievements = player.achievement:initRanchAchievements()
            player:getRanch():setAchievements(ranchAchievements)

            local ranchWareHouse = player.wareHouse:initRanchWareHouse()
            player:getRanch():setStorage(ranchWareHouse)
        end
    end

end

function PlayerListener.onPlayerBuyRanchBuildItemResult(rakssid, type, isSuccess, buildingId, num)

    if isSuccess == false then
        return false
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerBuyRanchBuildItemResult : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if type == 1 or type == 2 then
        local buildingInfo = BuildingConfig:getBuildingInfoById(buildingId)
        if buildingInfo == nil then
            HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerBuyRanchBuildItemResult :can not find buildingInfo by buildingId[".. buildingId .."]")
            return false
        end

        if type == 2 then
            player.achievement:addCount(AchievementConfig.TYPE_FACTORY, AchievementConfig.TYPE_FACTORY_NUM, 0, num)

            local ranchAchievements = player.achievement:initRanchAchievements()
            player:getRanch():setAchievements(ranchAchievements)
        end

        local itemId = ItemsMappingConfig:getSourceIdByMappingId(buildingInfo.id)
        player:addItem(itemId, num, 0)

        local totalMoney = buildingInfo.price * num
        player.achievement:addCount(AchievementConfig.TYPE_RESOURCE, AchievementConfig.TYPE_RESOURCE_MONEY_CONSUME, 0, totalMoney)

        local ranchAchievements = player.achievement:initRanchAchievements()
        player:getRanch():setAchievements(ranchAchievements)

        local items = {}
        local item = RanchCommon.new()
        item.itemId = buildingInfo.id
        item.num = num
        items[1] = item
        HostApi.sendRanchGain(rakssid, items)

        local config = {
            entityId = 0,
            id = buildingInfo.id,
            offset = VectorUtil.newVector3(0, 0, 0),
            yaw = 0,
            actorId = buildingInfo.actorId,
            type = buildingInfo.type,
            curQueueNum = buildingInfo.initQueueNum,
            maxQueueNum = buildingInfo.maxQueueNum,
            productCapacity = buildingInfo.productCapacity,
        }
        player.building:initBuilding(config)
        player.building:initAllQueueStatus(config.id, config.curQueueNum, config.maxQueueNum)

        local building = player.building:initRanchBuilding(buildingInfo.type)
        player:getRanch():setBuildByType(buildingInfo.type, building)

        return true
    end

    if type == 3 then

        if tostring(buildingId) == "500000" then
            local totalMoney = FieldLevelConfig:getMoneyByFieldLevel(player.fieldLevel) * num
            player.achievement:addCount(AchievementConfig.TYPE_RESOURCE, AchievementConfig.TYPE_RESOURCE_MONEY_CONSUME, 0, totalMoney)

            player.currentFieldNum = player.currentFieldNum + num
            player.achievement:addCount(AchievementConfig.TYPE_PLANT, AchievementConfig.TYPE_PLANT_FILED_NUM, 0, 1)

            player.fieldLevel = FieldLevelConfig:getFieldLevelByCurFieldNum(player.currentFieldNum)
            local ranchField = player.field:initRanchPlant(player.fieldLevel, player.currentFieldNum)
            player:getRanch():setBuildByType(BuildingConfig.TYPE_PLANT, ranchField)
        end

        if tostring(buildingId) ~= "500000" then
            local totalMoney = SeedLevelConfig:getMoneyBySeedId(buildingId) * num
            player.achievement:addCount(AchievementConfig.TYPE_RESOURCE, AchievementConfig.TYPE_RESOURCE_MONEY_CONSUME, 0, totalMoney)
        end

        local ranchAchievements = player.achievement:initRanchAchievements()
        player:getRanch():setAchievements(ranchAchievements)

        local itemId = ItemsMappingConfig:getSourceIdByMappingId(buildingId)
        player:addItem(itemId, num, 0)

        local items = {}
        local item = RanchCommon.new()
        item.itemId = buildingId
        item.num = num
        items[1] = item
        HostApi.sendRanchGain(player.rakssid, items)
        return true
    end

end

function PlayerListener.onPlayerRanchUpgradeStorage(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchUpgradeStorage : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    if player.wareHouse == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchUpgradeStorage: can not find wareHouseInfo of player["..tostring(player.userId).."]")
        return false
    end

    local wareHouseMaxLevel = WareHouseConfig:getMaxLevel()
    if player.wareHouse.level >= wareHouseMaxLevel then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchUpgradeStorage: already max wareHouseLevel of player[" .. tostring(player.userId) .. "]")
        return false
    end

    local upgradeMoney = WareHouseConfig:getUpgradeMoneyByLevel(player.wareHouse.level)
    if player.money < upgradeMoney then
        return false
    end

    local upgradeItems = WareHouseConfig:getUpgradeItemsByLevel(player.wareHouse.level)
    if upgradeItems == nil or player.wareHouse:isHasItems(upgradeItems) == false then
        --HostApi.log("=== LuaLog: PlayerListener.onPlayerRanchUpgradeStorage : has not enough items to upgrade wareHouse player["..tostring(player.userId).."]")
        return false
    end

    player:subMoney(upgradeMoney)
    player.wareHouse:subItems(upgradeItems)
    player.wareHouse:upgradeWareHouse(player.userId, upgradeMoney)
end

function PlayerListener.onPlayerTakeAwayRanchBuildCargo(rakssid, entityId, itemId, index)
    if index <= 0 then
        return false
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerTakeAwayRanchBuildCargo : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.building == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerTakeAwayRanchBuildCargo: can not find building Info of player["..tostring(player.userId).. "]")
        return false
    end

    if player.wareHouse == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerTakeAwayRanchBuildCargo: can not find wareHouse Info of player["..tostring(player.userId).. "]")
        return false
    end

    if player.wareHouse:isEnoughCapacityByNum(1) == false then
        HostApi.sendRanchWarehouseResult(player.rakssid, "gui_ranch_storage_full_operation_failure")
        return false
    end

    local buildingInfo = player.building:getBuildingInfoByEntityId(entityId)
    if buildingInfo == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerTakeAwayRanchBuildCargo: can not find ranch building Info of player["..tostring(player.userId).. "]")
        return false
    end

    if player.building:subProduct(buildingInfo.id, index) == false then
        return false
    end

    --  收获成就
    if buildingInfo.type == 1 then
        player.achievement:addCount(AchievementConfig.TYPE_ANIMAL, AchievementConfig.TYPE_ANIMAL_GET, itemId, 1)

        local ranchAchievements = player.achievement:initRanchAchievements()
        player:getRanch():setAchievements(ranchAchievements)
    end

    if buildingInfo.type == 2 then
        player.achievement:addCount(AchievementConfig.TYPE_FACTORY, AchievementConfig.TYPE_FACTORY_GET, itemId, 1)

        local ranchAchievements = player.achievement:initRanchAchievements()
        player:getRanch():setAchievements(ranchAchievements)
    end

    local nextQueueStartTime = os.time()
    player.building:startFactoryQueueWorking(buildingInfo.id, nextQueueStartTime)
    player.building:startFarmingQueueWorking(buildingInfo.id)

    local ranchBuilding = player.building:getBuildingInfoById(buildingInfo.id)
    player.building:initRanchBuildingQueue(ranchBuilding)

    -- add items to wareHouse
    local items = {}
    local item = RanchCommon.new()
    item.itemId = itemId
    item.num = 1
    items[1] = item
    HostApi.sendRanchGain(rakssid, items)

    player.wareHouse:addItem(itemId, 1)

    local wareHouse = player.wareHouse:initRanchWareHouse()
    player:getRanch():setStorage(wareHouse)

    local exp = ItemsConfig:getExpByItemId(itemId)
    player:addExp(exp)

end

function PlayerListener.onPlayerRanchBuildingQueueProduct(rakssid, entityId, queueId, productId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingQueueProduct : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.building == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingQueueProduct: can not find building Info of player["..tostring(player.userId).. "]")
        return false
    end

    local buildingInfo = player.building:getBuildingInfoByEntityId(entityId)
    if buildingInfo == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingQueueProduct: can not find ranch building Info of player["..tostring(player.userId).. "]")
        return false
    end

    local needTime = ItemsConfig:getNeedTimeByItemId(productId)
    local queueIndex = player.building:getFirstIdleQueueId(buildingInfo.id)

    if queueIndex == 0 then
        HostApi.sendCommonTip(rakssid, "ranch_tip_full_queue")
        return false
    end

    local recipes = ItemsConfig:getRecipeItemsById(productId)
    if player.wareHouse:isHasItems(recipes) == false then
        HostApi.sendCommonTip(rakssid, "ranch_tip_has_not_item")
        return false
    end

    player.wareHouse:subItems(recipes)
    local wareHouse = player.wareHouse:initRanchWareHouse()
    player:getRanch():setStorage(wareHouse)

    player.building:addWorkingQueue(buildingInfo.id, queueIndex, productId, needTime)

    local nextQueueStartTime = os.time()
    player.building:startFactoryQueueWorking(buildingInfo.id, nextQueueStartTime)
    player.building:startFarmingQueueWorking(buildingInfo.id)

    local ranchBuilding = player.building:getBuildingInfoById(buildingInfo.id)
    player.building:initRanchBuildingQueue(ranchBuilding)
end

function PlayerListener.onPlayerRanchBuildingQueueSpeedUp(rakssid, entityId, queueId, productId, isSuccess)
    if isSuccess == false then
        return false
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingQueueSpeedUp : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.building == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingQueueSpeedUp: can not find building Info of player["..tostring(player.userId).. "]")
        return false
    end

    local buildingInfo = player.building:getBuildingInfoByEntityId(entityId)
    if buildingInfo == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingQueueSpeedUp: can not find ranch building Info of player["..tostring(player.userId).. "]")
        return false
    end

    if player.accelerateItems ~= nil then
        local queue = buildingInfo.queue[queueId]
        if queue ~= nil then
            local isUseFree = player.accelerateItems:useFreeAccelerateByItemId(queue.productId)
            if isUseFree == true then
                local accelerateItemsInfo = player.accelerateItems:initRanchShortcutFree()
                player:getRanch():setShortcutFreeTimes(accelerateItemsInfo)
            end
        end
    end

    player.building:updateQueueStateById(buildingInfo.id, queueId, player.userId, true)
end

function PlayerListener.onPlayerRanchBuildingQueueUnlock(rakssid, entityId, queueId, productId, isSuccess)
    if isSuccess == false then
        return false
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingQueueUnlock : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.building == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingQueueUnlock: can not find building Info of player["..tostring(player.userId).. "]")
        return false
    end

    local buildingInfo = player.building:getBuildingInfoByEntityId(entityId)
    if buildingInfo == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingQueueUnlock: can not find ranch building Info of player["..tostring(player.userId).. "]")
        return false
    end

    local queueIndex = player.building:getFirstLockedQueueId(buildingInfo.id)
    if queueIndex == 0 then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingQueueUnlock: can not find queueInfo by ["..queueIndex.."]")
        return false
    end

    player.building:unlockQueueById(buildingInfo.id, queueIndex)
end

function PlayerListener.onPlayerAskForHelpRanchOrder(rakssid, orderId, index)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerAskForHelpRanchOrder : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.task == nil then
        return false
    end

    if player.task.tasks[orderId] == nil then
        return false
    end

    if player.task.tasks[orderId].tasks[index] == nil then
        return false
    end

    local finishCount = 0
    local totalCount = 0
    for i, v in pairs(player.task.tasks[orderId].tasks) do
        totalCount = totalCount + 1
        if v.isDone == true then
            finishCount = finishCount + 1
        end
    end

    if finishCount < 1 then
        HostApi.sendCommonTip(rakssid, "ranch_tip_must_finish_one_box")
        return false
    end

    local task = player.task.tasks[orderId].tasks[index]
    local uniqueId = player.task.tasks[orderId].uniqueId
    local isHot = player.task.tasks[orderId].isHot

    local itemId = task.expandItemId
    local num = task.expandNum
    local itemLevel = ItemsConfig:getItemLevelByItemId(itemId)

    local rewards = {}
    local reward = {}
    reward.itemId = task.rewardItemId
    reward.count = task.rewardNum
    rewards[1] = reward

    local boxAmount = 0
    local fullBoxNum = 0

    for i, v in pairs(player.task.tasks[orderId].tasks) do
        boxAmount = boxAmount + 1
        if v.isDone == true then
            fullBoxNum = fullBoxNum + 1
        end
    end

    RanchersWeb:askForHelp(player.userId, player.name, uniqueId, boxAmount, fullBoxNum, orderId, index, num, itemId, itemLevel, rewards, isHot)

end

function PlayerListener.onPlayerPutIntoRanchOrderCargo(rakssid, orderId, index)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerPutIntoRanchOrderCargo : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.task == nil then
        return false
    end

    if player.wareHouse == nil then
        return false
    end

    local item = player.task:getTaskItemByIndex(orderId, index)
    if item == nil then
        return false
    end

    local itemId = item.itemId
    local num = item.num
    if player.wareHouse:isHasItem(itemId, num) == false then
        HostApi.sendCommonTip(rakssid, "ranch_tip_has_not_item")
        return false
    end

    local uniqueId = player.task:getUniqueIdByOrderId(orderId)
    local taskInfo = player.task:getTaskInfoByOrderIdAndIndex(orderId, index)
    if taskInfo.askForHelpId ~= 0 then
        local userId = player.userId
        -- 这个是求助的任务，告诉http服务器这个任务被填充了，取消求助。
        RanchersWeb:finishHelp(player.userId, player.userId, player.name, player:getSex(), uniqueId, taskInfo.askForHelpId, function(result)
            local callBackPlayer = PlayerManager:getPlayerByUserId(userId)
            if callBackPlayer == nil then
                return false
            end
            if result ~= nil and callBackPlayer.wareHouse ~= nil then
                callBackPlayer.wareHouse:subItem(itemId, num)
                local ranchWareHouse = callBackPlayer.wareHouse:initRanchWareHouse()
                callBackPlayer:getRanch():setStorage(ranchWareHouse)

                if callBackPlayer.achievement ~= nil then
                    callBackPlayer.achievement:addCount(AchievementConfig.TYPE_TASK, AchievementConfig.TYPE_TASK_COMMIT, itemId, num)
                    local ranchAchievements = callBackPlayer.achievement:initRanchAchievements()
                    callBackPlayer:getRanch():setAchievements(ranchAchievements)
                end

                RanchersWeb:myHelpList(callBackPlayer.userId)
                local fullBoxNum = 0
                if callBackPlayer.task ~= nil then
                    fullBoxNum = callBackPlayer.task:getFullBoxNumByOrderId(orderId)
                end
                RanchersWeb:updateOrder(callBackPlayer.userId, uniqueId, fullBoxNum + 1)
            end
        end)
    else
        player.wareHouse:subItem(itemId, num)
        local wareHouse = player.wareHouse:initRanchWareHouse()
        player:getRanch():setStorage(wareHouse)

        if player.achievement ~= nil then
            player.achievement:addCount(AchievementConfig.TYPE_TASK, AchievementConfig.TYPE_TASK_COMMIT, itemId, num)
            local ranchAchievements = player.achievement:initRanchAchievements()
            player:getRanch():setAchievements(ranchAchievements)
        end

        player.task:updateTaskStateByOrderIdAndIndex(orderId, index)
        local ranchTask = player.task:initRanchTask()
        player:getRanch():setOrders(ranchTask)

        local fullBoxNum = player.task:getFullBoxNumByOrderId(orderId)
        RanchersWeb:updateOrder(player.userId, uniqueId, fullBoxNum)
    end
end

function PlayerListener.onPlayerReceiveRanchOrderCargo(rakssid, orderId, index)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerReceiveRanchOrderCargo : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.task == nil then
        return false
    end

    if player.wareHouse == nil then
        return false
    end

    if player.task.tasks[orderId] == nil then
        return false
    end

    if player.task.tasks[orderId].status ~= 4 then
        return false
    end

    local rewards = player.task:getRewardsByOrderId(orderId, player.userId)
    local helperRewards = rewards.helperRewards
    local selfRewards = rewards.selfRewards

    local money = 0
    local exp = 0
    for i, v in pairs(selfRewards) do
        --  检测900001
        if tostring(v.itemId) == "900001" then
           money = money + v.num
        end

        -- 检测900003
        if tostring(v.itemId) == "900003" then
            exp = exp + v.num
        end
    end

    local finishReward = TasksConfig:getFinishRewardById(player.task.tasks[orderId].id)
    for i, v in pairs(finishReward) do
        --  检测900001
        if tostring(v.itemId) == "900001" then
            money = money + v.num
        end

        -- 检测900003
        if tostring(v.itemId) == "900003" then
            exp = exp + v.num
        end
    end

    local newSelfRewards = {}
    local newIndex = 1
    for i, v in pairs(selfRewards) do
        if tostring(v.itemId) ~= "900001" and tostring(v.itemId) ~= "900003" and v.itemId ~= 0 and v.num ~= 0 then
            local item = RanchCommon.new()
            item.itemId = v.itemId
            item.num = v.num
            newSelfRewards[newIndex] = item
            newIndex = newIndex + 1
        end
    end

    if player.wareHouse:isEnoughCapacity(newSelfRewards) == false then
        HostApi.sendRanchWarehouseResult(rakssid, "gui_ranch_storage_full_operation_failure")
        return false
    end

    player:addMoney(money)

    if exp > 0 then
        player:addExp(exp)
    end

    player.wareHouse:addItems(newSelfRewards)

    local wareHouse = player.wareHouse:initRanchWareHouse()
    player:getRanch():setStorage(wareHouse)
    HostApi.sendRanchGain(rakssid, newSelfRewards)

    if player.achievement ~= nil then
        player.achievement:addCount(AchievementConfig.TYPE_TASK, AchievementConfig.TYPE_TASK_COMPLETE, 0, 1)
        player.achievement:addCount(AchievementConfig.TYPE_RESOURCE, AchievementConfig.TYPE_RESOURCE_MONEY_EARN, 0, money)
        local ranchAchievements = player.achievement:initRanchAchievements()
        player:getRanch():setAchievements(ranchAchievements)
    end

    -- 告诉http服务器这个任务完成了
    RanchersWeb:finishOrder(player.userId, player.task.tasks[orderId].uniqueId)

    -- 刷新一个新的任务
    player.task:refreshTaskByIndex(orderId, player.level)
    local ranchTask = player.task:initRanchTask()
    player:getRanch():setOrders(ranchTask)

    for i, v in pairs(helperRewards) do
        if v.helperId ~= nil then
            local helperId = v.helperId
            local itemId = v.itemId
            local num = v.num

            -- 发送邮件给帮助者，发放奖励
            local items = {}
            local item = {}
            item.itemId = itemId
            item.num = num
            items[1] = item

            local title = "xxx"
            local data = {}
            data.items = items
            data.msg = "xxx"

            local lang = MailLanguageConfig:getLangByType(MailType.Task)
            if lang ~= nil then
                title = lang.title
                data.msg = lang.name
            end

            WebService:SendMails(player.userId, helperId, MailType.Task, title, data, "g1031")

        end
    end

end

function PlayerListener.onPlayerRefreshRanchOrder(rakssid, orderId, index)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRefreshRanchOrder : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.task == nil then
        return false
    end

    if player.task.tasks[orderId] == nil then
        return false
    end

    if player.task:isTaskCanRefreshById(orderId) == false then
        local ranchTask = player.task:initRanchTask()
        player:getRanch():setOrders(ranchTask)
        return false
    end

    local refreshTime = player.task:getRefreshTimeById(orderId)
    if refreshTime <= 0 then
        player.task:refreshTaskByIndex(orderId, player.level)
        player.task:updateRefreshTimeById(orderId)
        local ranchTask = player.task:initRanchTask()
        player:getRanch():setOrders(ranchTask)
        return false
    end
    local coolDownTime = player.task:getCoolDownTimeById(orderId)
    local deltaTime = coolDownTime - (os.time() - refreshTime)

    if deltaTime <= 0 then
        player.task:refreshTaskByIndex(orderId, player.level)
        player.task:updateRefreshTimeById(orderId)
        local ranchTask = player.task:initRanchTask()
        player:getRanch():setOrders(ranchTask)
        return false
    end
    local money = TimePaymentConfig:getPaymentByTime(deltaTime)
    player:consumeDiamonds(ItemsConfig.DIAMOND_REFRESH_RANCH_ORDER, money, RanchersWebRequestType.refreshOrder .. ":" .. orderId, false)

end

function PlayerListener.onPlayerRanchHelpFinish(rakssid, askForHelpId, type)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchHelpFinish : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.wareHouse == nil then
        return false
    end

    local userId = player.userId
    RanchersWeb:helpListById(player.userId, askForHelpId, function(item)
        if item == nil then
            return false
        end
        local callBackPlayer = PlayerManager:getPlayerByUserId(userId)
        if type == 1 then
            if callBackPlayer == nil or callBackPlayer.wareHouse == nil then
                HostApi.sendCommonTip(rakssid, "ranch_tip_somebody_already_finish_help")
                return false
            end

            local helperId = item.helperId
            if helperId ~= 0 then
                HostApi.sendCommonTip(rakssid, "ranch_tip_somebody_already_finish_help")
                return false
            end
            local itemId = item.itemId
            local num = item.itemCount
            local uniqueId = item.uniqueId
            local fullBoxNum = item.fullBoxNum
            if callBackPlayer.wareHouse:isHasItem(itemId, num) == false then
                HostApi.sendCommonTip(rakssid, "ranch_tip_has_not_item")
                return false
            end
            RanchersWeb:finishHelp(callBackPlayer.userId, callBackPlayer.userId, callBackPlayer.name, callBackPlayer:getSex(), uniqueId, askForHelpId, function(result)
                local _callBackPlayer = PlayerManager:getPlayerByUserId(userId)
                if _callBackPlayer == nil then
                    return false
                end
                if result ~= nil and _callBackPlayer.wareHouse ~= nil then
                    _callBackPlayer.wareHouse:subItem(itemId, num)
                    local ranchWareHouse = _callBackPlayer.wareHouse:initRanchWareHouse()
                    _callBackPlayer:getRanch():setStorage(ranchWareHouse)

                    RanchersWeb:updateOrder(_callBackPlayer.userId, uniqueId, fullBoxNum + 1)
                    HostApi.sendRanchOrderHelpResult(rakssid, "ranch_tip_help_success")

                    if _callBackPlayer.achievement ~= nil then
                        _callBackPlayer.achievement:addCount(AchievementConfig.TYPE_SOCIAL, AchievementConfig.TYPE_SOCIAL_TASKS_HELP, 0, 1)
                        local ranchAchievements = _callBackPlayer.achievement:initRanchAchievements()
                        _callBackPlayer:getRanch():setAchievements(ranchAchievements)
                    end
                end
            end)
        end

        if type == 2 then
            if callBackPlayer == nil or callBackPlayer.wareHouse == nil then
                return false
            end

            local data = {}
            data.itemId = tonumber(item.itemId)
            data.num = tonumber(item.itemCount)
            local items = {}
            table.insert(items, data)
            local totalMoney = callBackPlayer.wareHouse:getCubeMoneyByItems(items)
            callBackPlayer:consumeDiamonds(ItemsConfig.DIAMOND_FINISH_HELP, totalMoney, RanchersWebRequestType.ranchHelpFinish .. ":" .. item.itemId .. ":" .. item.itemCount .. ":" .. item.uniqueId .. ":" .. item.fullBoxNum .. ":" .. tostring(askForHelpId), false)
        end
    end)
end

function PlayerListener.onPlayerRanchReceiveMailReward(rakssid, mailId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchReceiveMailReward : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    local userId = player.userId
    WebService:CheckMails(player.userId, "g1031", function (items)
        local callBackPlayer = PlayerManager:getPlayerByUserId(userId)
        if callBackPlayer == nil then
            return false
        end
        for i, v in pairs(items) do
            if tostring(v.id) == tostring(mailId) then
                if v.status == WebRequestType.EMAIL_STATUS_UNREAD then
                    local content = json.decode(v.content)

                    -- 检测 900001 的情况
                    local money = 0
                    for i, v in pairs(content.items) do
                        if tostring(v.itemId) == "900001" then
                            money = money + v.num
                        end
                    end

                    -- 检测 900003 的情况
                    local exp = 0
                    for i, v in pairs(content.items) do
                        if tostring(v.itemId) == "900003" then
                            exp = exp + v.num
                        end
                    end

                    local cItems = {}
                    local iItems = {}
                    local itemIndex = 1
                    local iItemIndex = 1
                    local iItemTotalNum = 0
                    for i, v in pairs(content.items) do
                        if tostring(v.itemId) ~= "900001" and tostring(v.itemId) ~= "900003" and v.itemId > 100000 and v.num >= 0 then
                            local item = RanchCommon.new()
                            item.itemId = v.itemId
                            item.num = v.num
                            cItems[itemIndex] = item
                            itemIndex = itemIndex + 1
                        end
                    end

                    for i, v in pairs(content.items) do
                        if v.itemId < 100000 and v.num >= 0 then
                            local item = {}
                            item.itemId = v.itemId
                            item.num = v.num
                            iItems[iItemIndex] = item
                            iItemIndex = iItemIndex + 1
                            iItemTotalNum = iItemTotalNum + v.num
                        end
                    end
                    if callBackPlayer.wareHouse == nil then
                        HostApi.log("=== LuaLog: PlayerListener.onPlayerRanchReceiveMailReward : player wareHouse is nil")
                        return false
                    end

                    if callBackPlayer.wareHouse:isEnoughCapacity(cItems) == false then
                        HostApi.sendRanchWarehouseResult(rakssid, "gui_ranch_storage_full_operation_failure")
                        return false
                    end

                    if iItemTotalNum >= 0 then
                        local inv = callBackPlayer:getInventory()
                        if inv:isCanAddItem(1, 0, iItemTotalNum) == false then
                            return false
                        end
                    end

                    WebService:UpdateMails(v.id, WebRequestType.EMAIL_STATUS_RECEIVED, function(result, code)
                        if result == nil then
                            return false
                        end
                        local _callBackPlayer = PlayerManager:getPlayerByUserId(userId)
                        if _callBackPlayer == nil then
                            return false
                        end

                        local moneyType = AchievementConfig.MONEY_TYPE_GIFT
                        if v.mailType == MailType.Task then
                            moneyType = AchievementConfig.MONEY_TYPE_EARN
                        end

                        if v.mailType == MailType.EveryDayBeSend then

                            _callBackPlayer.giftNum = _callBackPlayer.giftNum + 1

                            local ranchInfo = _callBackPlayer:initRanchInfo()
                            _callBackPlayer:getRanch():setInfo(ranchInfo)
                            RanchersWeb:updateLandInfo(_callBackPlayer.userId, _callBackPlayer.exp, _callBackPlayer.giftNum, _callBackPlayer.level, _callBackPlayer.building.prosperity)
                            if _callBackPlayer.achievement ~= nil then
                                _callBackPlayer.achievement:addCount(AchievementConfig.TYPE_SOCIAL, AchievementConfig.TYPE_SOCIAL_GIFT_RECEIVE, 0, 1)
                                local ranchAchievements = _callBackPlayer.achievement:initRanchAchievements()
                                _callBackPlayer:getRanch():setAchievements(ranchAchievements)
                            end
                        end

                        _callBackPlayer:addMoney(money, moneyType)
                        if exp > 0 then
                            _callBackPlayer:addExp(exp)
                        end
                        if _callBackPlayer.wareHouse ~= nil then
                            _callBackPlayer.wareHouse:addItems(cItems)
                            local ranchWareHouse = _callBackPlayer.wareHouse:initRanchWareHouse()
                            _callBackPlayer:getRanch():setStorage(ranchWareHouse)
                        end

                        for i, v in pairs(iItems) do
                            _callBackPlayer:addItem(v.itemId, v.num, 0)
                        end

                        HostApi.sendRanchGain(rakssid, cItems)
                        HostApi.sendMailReceiveResult(rakssid, "ranch_tip_get_mail_receive_success")
                    end)
                end
            end
        end
    end)
end

function PlayerListener.onPlayerRanchSendEveryDayGift(rakssid, toUserId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchSendEveryDayGift : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    local fromUserGiftId = GiftConfig:getGiftIdByType(MailType.EveryDaySend)
    local toUserGiftId = GiftConfig:getGiftIdByType(MailType.EveryDayBeSend)

    if fromUserGiftId == nil or toUserGiftId == nil then
        HostApi.sendCommonTip(rakssid, "ranch_tip_send_gift_fail")
        return false
    end

    local fromUserReward = GiftConfig:getRewardById(fromUserGiftId)
    local toUserReward = GiftConfig:getRewardById(toUserGiftId)

    if fromUserReward == nil or toUserReward == nil then
        HostApi.sendCommonTip(rakssid, "ranch_tip_send_gift_fail")
        return false
    end

    local fromUserTitle = "xxx"
    local fromContent = {}
    fromContent.items = fromUserReward
    fromContent.msg = "xxx"
    local fromMailLang = MailLanguageConfig:getLangByType(MailType.EveryDaySend)
    if fromMailLang ~= nil then
        fromUserTitle = fromMailLang.title
        fromContent.msg = fromMailLang.name
    end

    local toUserTitle = "xxx"
    local toContent = {}
    toContent.items = toUserReward
    toContent.msg = "xxx"
    local toMailLang = MailLanguageConfig:getLangByType(MailType.EveryDayBeSend)
    if toMailLang ~= nil then
        toUserTitle = toMailLang.title
        toContent.msg = toMailLang.name
    end

    local userId = player.userId
    RanchersWeb:giftSend(player.userId,fromUserTitle, fromContent, MailType.EveryDaySend, toUserId, toUserTitle,toContent,  MailType.EveryDayBeSend, "g1031", function(data, code)
        if code == 0 then
            return false
        end

        local callBackPlayer = PlayerManager:getPlayerByUserId(userId)
        if callBackPlayer == nil then
            return false
        end

        if code == 1 then
            HostApi.sendGiveGiftResult(rakssid, "ranch_tip_send_gift_success")
            if callBackPlayer.achievement ~= nil then
                callBackPlayer.achievement:addCount(AchievementConfig.TYPE_SOCIAL, AchievementConfig.TYPE_SOCIAL_GIFT_SEND, 0, 1)
                local ranchAchievements = callBackPlayer.achievement:initRanchAchievements()
                callBackPlayer:getRanch():setAchievements(ranchAchievements)
            end

            RanchersWeb:checkHasLand(callBackPlayer.userId, function(data)
                local _callBackPlayer = PlayerManager:getPlayerByUserId(userId)
                if data ~= nil and _callBackPlayer ~= nil then
                    _callBackPlayer.sendGiftCount = data.sendGiftCount
                    local ranchInfo = _callBackPlayer:initRanchInfo()
                    _callBackPlayer:getRanch():setInfo(ranchInfo)
                end
            end)

        elseif code == 2017 then
            if callBackPlayer.level < GiftLevelConfig:getTheFirstMinLevel() then
                HostApi.sendCommonTip(callBackPlayer.rakssid, "ranch_tip_not_enough_level")
            else
                HostApi.sendCommonTip(callBackPlayer.rakssid, "ranch_tip_not_enough_send_gift_num")
            end
        elseif code == 2018 then
            HostApi.sendCommonTip(callBackPlayer.rakssid, "ranch_tip_dont_repeat_send_gift")
        elseif code == 8 then
            HostApi.sendCommonTip(callBackPlayer.rakssid, "ranch_tip_too_fast_send_gift")
        else
            HostApi.sendCommonTip(callBackPlayer.rakssid, "ranch_tip_send_gift_fail")
        end
    end)
end

function PlayerListener.onRefreshPlayerRanchOrder(rakssid)
    -- 接受广播刷新订单
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onRefreshPlayerRanchOrder : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    RanchersWeb:myHelpList(player.userId)
end

function PlayerListener.onPlayerRanchReceiveAchievementReward(rakssid, achievementId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchReceiveAchievementReward : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    local id = tonumber(tostring(achievementId))
    local achievementInfo = AchievementConfig:getAchievementInfoById(id)
    if achievementInfo == nil then
        return false
    end

    if player.achievement:updateAchievementLevelById(id) then
        local achievementPoint = achievementInfo.achievementPoint
        local exp = achievementInfo.exp
        player:addAchievementPoint(achievementPoint)
        player:addExp(exp)
        local ranchAchievements = player.achievement:initRanchAchievements()
        player:getRanch():setAchievements(ranchAchievements)
    end

end

function PlayerListener.onPlayerRanchGoExplore(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchGoExplore : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    --if player.manorIndex == 0 then
    --    HostApi.sendCommonTip(rakssid, "ranch_tip_need_get_manor")
    --    return false
    --end

    if player.level < GameConfig.exploreStartLevel then
        HostApi.sendCommonTip(rakssid, "ranch_tip_start_at_x_level")
        return false
    end

    EngineUtil.sendEnterOtherGame(rakssid, "g1035", player.userId)

end

function PlayerListener.onPlayerRanchSendInviteJoinGame(rakssid, targetUserId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchSendInviteJoinGame : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    RanchersWeb:sendInvitation(player.userId, targetUserId, player.name, function(code)
        if code == 1 then
            HostApi.sendCommonTip(rakssid, "ranch_tip_send_invitation_success")
        else
            HostApi.sendCommonTip(rakssid, "ranch_tip_something_error")
        end
    end)

end

function PlayerListener.onPlayerBuyRanchTaskItemsResult(rakssid, orderId, index, payId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        EngineUtil.disposePlatformOrder(0, payId, false)
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerBuyRanchTaskItemsResult : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.task == nil then
        EngineUtil.disposePlatformOrder(player.userId, payId, false)
        return false
    end

    if player.wareHouse == nil then
        EngineUtil.disposePlatformOrder(player.userId, payId, false)
        return false
    end

    local item = player.task:getTaskItemByIndex(orderId, index)
    if item == nil then
        EngineUtil.disposePlatformOrder(player.userId, payId, false)
        return false
    end

    local itemId = item.itemId
    local num = item.num

    local wareHouseItemNum = player.wareHouse:getNumByItemId(itemId)

    local needNum = num
    if num >= wareHouseItemNum then
        needNum = wareHouseItemNum
    end

    local uniqueId = player.task:getUniqueIdByOrderId(orderId)
    local taskInfo = player.task:getTaskInfoByOrderIdAndIndex(orderId, index)
    if taskInfo.askForHelpId ~= 0 then
        local userId = player.userId
        -- 这个是求助的任务，告诉http服务器这个任务被填充了，取消求助。
        RanchersWeb:finishHelp(player.userId, player.userId, player.name, player:getSex(), uniqueId, taskInfo.askForHelpId, function(result)
            local callBackPlayer = PlayerManager:getPlayerByUserId(userId)
            if callBackPlayer == nil then
                EngineUtil.disposePlatformOrder(0, payId, false)
                return false
            end
            if result ~= nil and callBackPlayer.wareHouse ~= nil then
                callBackPlayer.wareHouse:subItem(itemId, needNum)
                local ranchWareHouse = callBackPlayer.wareHouse:initRanchWareHouse()
                callBackPlayer:getRanch():setStorage(ranchWareHouse)

                if callBackPlayer.achievement ~= nil then
                    callBackPlayer.achievement:addCount(AchievementConfig.TYPE_TASK, AchievementConfig.TYPE_TASK_COMMIT, itemId, num)
                    local ranchAchievements = callBackPlayer.achievement:initRanchAchievements()
                    callBackPlayer:getRanch():setAchievements(ranchAchievements)
                end

                RanchersWeb:myHelpList(callBackPlayer.userId)
                local fullBoxNum = 0
                if callBackPlayer.task ~= nil then
                    fullBoxNum = callBackPlayer.task:getFullBoxNumByOrderId(orderId)
                end
                RanchersWeb:updateOrder(callBackPlayer.userId, uniqueId, fullBoxNum + 1)
                EngineUtil.disposePlatformOrder(callBackPlayer.userId, payId, true)
            else
                EngineUtil.disposePlatformOrder(callBackPlayer.userId, payId, false)
                return false
            end
        end)
    else
        player.wareHouse:subItem(itemId, needNum)
        local wareHouse = player.wareHouse:initRanchWareHouse()
        player:getRanch():setStorage(wareHouse)

        player.achievement:addCount(AchievementConfig.TYPE_TASK, AchievementConfig.TYPE_TASK_COMMIT, itemId, num)

        local ranchAchievements = player.achievement:initRanchAchievements()
        player:getRanch():setAchievements(ranchAchievements)

        player.task:updateTaskStateByOrderIdAndIndex(orderId, index)
        local ranchTask = player.task:initRanchTask()
        player:getRanch():setOrders(ranchTask)

        local fullBoxNum = player.task:getFullBoxNumByOrderId(orderId)
        RanchersWeb:updateOrder(player.userId, uniqueId, fullBoxNum)
        EngineUtil.disposePlatformOrder(player.userId, payId, true)
    end
end

function PlayerListener.onPlayerRanchBuildingProductionSpeedUp(rakssid, entityId, queueId, productId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingProductionSpeedUp : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.building == nil then
        return false
    end

    local buildingInfo = player.building:getBuildingInfoByEntityId(entityId)
    if buildingInfo == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchBuildingProductionSpeedUp: can not find ranch building Info of player["..tostring(player.userId).. "]")
        return false
    end

    if player.wareHouse == nil then
        return false
    end

    local queueIndex = player.building:getFirstIdleQueueId(buildingInfo.id)
    if queueIndex == 0 then
        HostApi.sendCommonTip(rakssid, "ranch_tip_full_queue")
        return false
    end

    local recipeItems = ItemsConfig:getRecipeItemsById(productId)
    local totalMoney = player.wareHouse:getCubeMoneyByItems(recipeItems)

    player:consumeDiamonds(ItemsConfig.DIAMOND_BUILDING_SPEED_UP, totalMoney, RanchersWebRequestType.productionSpeedUp .. ":" .. tostring(entityId) .. ":" .. tostring(productId), false)
end

function PlayerListener.onPlayerRanchUseCubeUpgradeStorage(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchUseCubeUpgradeStorage : player rakssid[".. tostring(rakssid) .. "] is nil")
        return false
    end

    if player.manorIndex == 0 then
        return false
    end

    if player.wareHouse == nil then
        HostApi.log("=== LugLog: [error] PlayerListener.onPlayerRanchUseCubeUpgradeStorage: can not find wareHouseInfo of player["..tostring(player.userId).."]")
        return false
    end

    local wareHouseMaxLevel = WareHouseConfig:getMaxLevel()
    if player.wareHouse.level >= wareHouseMaxLevel then
        HostApi.log("=== LuaLog: [error] PlayerListener.onPlayerRanchUseCubeUpgradeStorage: already max wareHouseLevel of player[" .. tostring(player.userId) .. "]")
        return false
    end

    local upgradeMoney = WareHouseConfig:getUpgradeMoneyByLevel(player.wareHouse.level)
    if player.money < upgradeMoney then
        return false
    end

    local upgradeItems = WareHouseConfig:getUpgradeItemsByLevel(player.wareHouse.level)
    if upgradeItems == nil then
        return false
    end

    local totalMoney = player.wareHouse:getCubeMoneyByItems(upgradeItems)
    player:consumeDiamonds(ItemsConfig.DIAMOND_WAREHOUSE_UPGRADE, totalMoney, RanchersWebRequestType.upgradeWareHouse .. ":", false)
end

function PlayerListener.onCheckFlyingPermission(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if player.isCanFlying == true then
        player:setAllowFlying(true)
        return true
    end

    HostApi.sendBuyFlying(rakssid)
    return false
end

function PlayerListener.onBuyFlyingPermissionResult(rakssid, isSuccess)
    if isSuccess == false then
        return false
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    player.isCanFlying = true
    player:setAllowFlying(true)
    local ranchInfo = player:initRanchInfo()
    player:getRanch():setInfo(ranchInfo)
end


return PlayerListener
--endregion
