--region *.lua
--Date
--此文件由[BabeLua]插件自动生成
require "messages.Messages"
require "data.GamePlayer"
require "Match"

PlayerListener = {}

function PlayerListener:init()

    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerDynamicAttrEvent, self.onDynamicAttrInit)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)
    BaseListener.registerCallBack(PlayerDieTipEvent, self.onPlayerDiedTip)

    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerPickupItemEndEvent.registerCallBack(self.onPlayerPickupItemEnd)
    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerBackHallEvent.registerCallBack(self.onPlayerBackSkyBlock)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerPlaceBlockBoxJudgeEvent.registerCallBack(self.onPlayerPlaceBlockBoxJudgeEvent)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerExchangeItemEvent.registerCallBack(self.onPlayerExchangeItem)
    PlayerAttackSessionNpcEvent.registerCallBack(self.onPlayerAttackSessionNpc)
    WatchAdvertFinishedEvent.registerCallBack(self.onWatchAdvertFinished)
    PlayerUseInitSpawnPositionEvent.registerCallBack(self.onPlayerUseInitSpawnPosition)

    PlayerClickBuyUpdateEvent:registerCallBack(self.onPlayerClickBuyUpdate)
    PlayerWatchAdUpdateInfoEvent:registerCallBack(self.onPlayerWatchAdUpdateInfo)
    PlayerSkyBlocShopOperateEvent:registerCallBack(self.onPlayerSkyBlocBuyGoods)
    SkyBlockReceiveTaskRewardEvent:registerCallBack(self.onReceiveTaskReward)
    PlayerSellItemEvent:registerCallBack(self.onPlayerSellItem)
    PlayerClickResourceIslandEvent:registerCallBack(self.onPlayerLeaveForMiningArea)
    SkyBlockDareTaskOperateEvent:registerCallBack(self.SkyBlockDareTaskOperate)
    PlayerBuyMoneyEvent:registerCallBack(self.onBuyMoney)
    PlayerBuyChirstmasGiftEvent:registerCallBack(self.PlayerBuyChirstmasGiftEvent)
    PlayerGetGiftNumEvent:registerCallBack(self.onPlayerGetGiftNumEvent)
    PlayerOpenGiftEvent:registerCallBack(self.onPlayerOpenGiftEvent)
    PlayerCommonClickEvent:registerCallBack(self.onPlayerCommonClick)
    SkyBlockRedSwitchClickEvent:registerCallBack(self.onRedSwitchClick)
    ReceiveSumRechargeEvent:registerCallBack(self.onReceiveSumRecharge)
    ReceiveSumRechargeEndEvent:registerCallBack(self.onReceiveSumRechargeEnd)
    SkyBlockRedRichBtnClickEvent:registerCallBack(self.onRedRichBtnClickEvent)
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
        return 0
    end

    local manorInfo = GameMatch:getUserManorInfo(player.userId)
    if manorInfo ~= nil then
        manorInfo:masterLogin()
    end
    player:addNightVisionPotionEffect(99999)
    local players = PlayerManager:getPlayers()
    for _, other_player in pairs(players) do
        if tostring(other_player.rakssid) ~= tostring(player.rakssid) then
            print("onPlayerReady other_player.rakssid : "..tostring(other_player.rakssid) .."   player.rakssid : "..tostring(player.rakssid))
            other_player.readyShowTreasure = false
            other_player:checkTreasureInInvetory(player.rakssid)
        end
    end
    LuaTimer:scheduleTimer(function( )
        player.isUpdateAllNpc = true
        player.isUpdateAllName = true
    end,3000, 1)
    DbUtil:getPlayerData(player.userId, DbUtil.TAG_SHAREDATA)
    DbUtil:getPlayerData(player.userId, DbUtil.TAG_MIAN_LINE)
    HostApi.sendPlaySound(player.rakssid, 10043)
    NotificationConfig:onPlayerReady(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    return 0
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if damageType == "fall" then
        return false
    end
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if not dier then
        return true
    end
    RespawnHelper.sendRespawnCountdown(dier, 0)
    dier.isLife = false
    dier:setInventory()
    return true
end

function PlayerListener.onPlayerRespawn(player)
    player.isLife = true
    player:teleHomeWithYaw()
    player:changeMaxHealth(GameConfig.initHP)
    player:fillInvetoryBySlot(player.inventory)
    player.readyShowTreasure = false
    player:checkTreasureInInvetory()
    local maxHp = player:getMaxHealth()
    player:changeMaxHealth(maxHp)
    player:setFoodLevel(20)
    PrivilegeConfig:checkSpePri(player)
    return 43200
end

function PlayerListener.onDynamicAttrInit(attr)
    if attr.manorId < 0 then
        return
    end
    local platformId = attr.userId
    local targetUserId = attr.targetUserId
    local manorIndex = attr.manorId
    HostApi.log("==== LuaLog: PlayerListener.onDynamicAttrInit  --- start --- : platformId = " .. tostring(platformId) .. ", targetUserId = " .. tostring(targetUserId) .. ", manorIndex = " .. manorIndex)
    if tostring(targetUserId) ~= "0" and manorIndex > 0 then
        GameMatch:initCallOnTarget(platformId, targetUserId)

        local userManorInfo = GameMatch.userManorInfo[tostring(targetUserId)]
        if userManorInfo == nil then

            HostApi.log("==== LuaLog: GameManor.new : " .. tostring(targetUserId))
            local manorInfo = GameManor.new(manorIndex, targetUserId)
            GameMatch:initUserManorInfo(targetUserId, manorInfo)
            -- DbUtil:getPlayerData(targetUserId, DbUtil.TAG_MANOR)

        else
            HostApi.log("==== LuaLog: userManorInfo:refreshTime : " .. tostring(targetUserId))

            userManorInfo:refreshTime()
        end
    else
        HostApi.log("==== LuaLog: PlayerListener.onDynamicAttrInit [targetUserId == 0] or [manorIndex <= 0] ")
    end
end

function PlayerListener.onPlayerPlaceBlockBoxJudgeEvent(blockId)
    return false
end

function PlayerListener.onPlayerBackSkyBlock(rakssid)
    HostApi.log("onPlayer leave for SkyBlock")
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    MsgSender.sendTopTipsToTarget(player.rakssid, 1000, Messages:msgTransmitting())
    BackHallManager.nextGameType = "g1048"
    BackHallManager:addPlayer(player)
end

function PlayerListener.onPlayerLeaveForMiningArea(userId, data)
    HostApi.log("onPlayer leave for SkyBlockMiningArea")

    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil or player.backMiningAreaTime == nil then
        return
    end

    if not DbUtil:CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        return
    end
    MsgSender.sendTopTipsToTarget(player.rakssid, 1000, Messages:msgTransmitting())
    -- local interval = os.time() - player.backMiningAreaTime
    -- if  interval < GameConfig.miningAreaCD then
    --     MsgSender.sendTopTipsToTarget(player.rakssid, 1, Messages:msgUIButtonCountDown(GameConfig.miningAreaCD - interval))
    --     HostApi.log("GameConfig.miningAreaCD - interval :"..GameConfig.miningAreaCD - interval)
    --     return
    -- end
    player.backMiningAreaTime = os.time()
    BackHallManager.nextGameType = "g1049"
    BackHallManager:addPlayer(player)
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta, stackSize, itemIndex)
    return false
end

function PlayerListener.onPlayerExchangeItem(rakssid, type, itemId, itemMeta, itemNum, bullets, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    LuaTimer:scheduleTimer(function(rakssid)
        local player = PlayerManager:getPlayerByRakssid(rakssid)
        if player then
            if itemId == 511 and itemMeta == 0 then
                player:checkTreasureInInvetory()
            end
            local num = player:getItemNumById(itemId, itemMeta)
            player.taskController:packingProgressData(player, Define.TaskType.collect, GameMatch.gameType, itemId, num, itemMeta)
        end
    end, 1000, 1, rakssid)

    HostApi.log("onPlayerExchangeItem userId: " .. tostring(player.userId)
            .. " itemId: " .. itemId .. " itemMeta: " .. itemMeta .. " itemNum: " .. itemNum .. " type: " .. type)
    return true
end

function PlayerListener.onPlayerSkyBlocBuyGoods(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    
    if player.isLife == false then
        print("onPlayerSkyBlocBuyGoods player is die userid == "..tostring(userId))
        return
    end

    local builder = DataBuilder.new():fromData(data)
    local num = builder:getNumberParam("num")
    local indexId = builder:getNumberParam("indexId")
    local type = builder:getNumberParam("type")

    if not DbUtil:CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        return
    end

    if type == AppShopConfig.OperateType.BUY_GOOD then
        HostApi.log("SkyBlocBuyGoodsEvent goodId == ".. indexId .. " num == " .. num)
        if num >= 1 and num <= 64 then
            AppShopConfig:BuyGoods(player.rakssid, indexId, num)
        end
    elseif type == AppShopConfig.OperateType.UPDATE_DATA then
        player:sendSkyBlockShopInfo()
    end
end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum, itemEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    local inv = player:getInventory()
    if not inv:isCanAddItem(itemId, 0, itemNum) then
        return false
    end

    local pos = player:getPosition()
    local can = GameConfig:canPickupItemInProductArea(player.manorIndex, pos)
    if not can then
        return false
    end

    local entity = EngineWorld:getEntity(itemEntityId)
    for i, k in pairs(player.world_products) do
        if k == itemEntityId then
            table.remove(player.world_products, i)
        end
    end
    if entity ~= nil then
        player:removeItemFromHadProductList(itemId, itemEntityId, itemNum)
        HostApi.log("onPlayerPickupItem userId: " .. tostring(player.userId)
                .. " itemId: " .. itemId .. " itemNum: " .. itemNum)
        return true
    end

    return false
end

function PlayerListener.onPlayerPickupItemEnd(rakssid, itemId, itemNum, meta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if not player:CanSavePlayerShareData() then
        return false
    end
    if itemId == ItemID.CURRENCY_MONEY then
        local inv = player:getInventory()
        if inv and itemNum * GameConfig.monetaryUnit > 0 and inv:consumeInventoryItem(itemId, itemNum) then
            -- player.money = player:getCurrency() + itemNum * GameConfig.monetaryUnit
            local money = player:getCurrency() + itemNum * GameConfig.monetaryUnit
            player:setCurrency(money)
            GameAnalytics:design(player.userId, 0, { "g1048addMoney", tostring(player.userId), itemNum * GameConfig.monetaryUnit})
        end
    end

    LuaTimer:scheduleTimer(function(rakssid)
        local player = PlayerManager:getPlayerByRakssid(rakssid)
        if player then
            local num = player:getItemNumById(itemId, meta)
            player.taskController:packingProgressData(player, Define.TaskType.collect, GameMatch.gameType, itemId, num, meta)
        end
    end, 1000, 1, rakssid)

    if itemId == Define.ChristmasBox.COMMON then
        local day = DateUtil.getDate()
        GameAnalytics:design(player.userId, 0, { "g1048getCommonBoxEveryDay", tostring(day)})
    end
    if itemId == Define.ChristmasBox.FINE then
        local day = DateUtil.getDate()
        GameAnalytics:design(player.userId, 0, { "g1048getSeniorBoxEveryDay", tostring(day)})
    end
    HostApi.log("onPlayerPickupItemEnd userId: " .. tostring(player.userId)
            .. " itemId: " .. itemId .. " itemNum: " .. itemNum .. " meta: " .. meta)
    return true
end

function PlayerListener.onPlayerAttackSessionNpc(rakssid, npcEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local id = GameConfig:getNpcIdByEntityId(player.manorIndex, npcEntityId)
    if id == Define.Sell then
        HostApi.sendCustomTip(player.rakssid, "a_key_selling", Define.AKeySell)
        return
    end
    if id == 0 then
        HostApi.log("the npc is not your own")
    else
        player:clickUpdateLevel(id)
    end
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return false
    end
    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)
    if player == nil then
        return false
    end
    GemStoneConfig:inGemStoneArea(player, VectorUtil.newVector3(x, y, z))
    if y <= GameConfig.deathLineY then
        player:teleHomeWithYaw()
        local maxHp = player:getMaxHealth()
        player:setHealth(maxHp)
        player:setFoodLevel(20)
        --player:setInventory()
        --player:fillInvetoryBySlot(player.inventory)
        return false
    end
    return true
end

function PlayerListener.onPlayerClickBuyUpdate(userId,data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local type = builder:getNumberParam("type")
    local id = builder:getNumberParam("id")
    player:updateLevel(type, id)

end

function PlayerListener.onPlayerWatchAdUpdateInfo(userId,data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local id = builder:getNumberParam("id")
    local level = builder:getNumberParam("level")
    if player.enableIndie then
        GameAnalytics:design(player.userId, 0, {"g1050product_click_watch_Ad_indie", id, level})
    else
        GameAnalytics:design(player.userId, 0, {"g1050product_click_watch_Ad", id, level})
    end
end


function PlayerListener.onWatchAdvertFinished(rakssid, adType, params, code)
    if code ~= 1 then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if code == 1 then
        local info = StringUtil.split(params, "#")
        if #info == 2 then
            local param1 = tostring(info[1])
            local param2 = tonumber(info[2])
            if param1 == "TaskAd" and adType == Define.AdType.Task then
                if param1 == "TaskAd" then
                    HostApi.log("onPlayAdResult_task_suc")
                    if player.enableIndie then
                        GameAnalytics:design(player.userId, 0, { "g1050task_lookAd_success_indie", param2})
                    else
                        GameAnalytics:design(player.userId, 0, { "g1050task_lookAd_success", param2})
                    end

                    player.taskController:packingProgressData(player, Define.TaskType.look_ad, GameMatch.gameType, 0, 1, -1, param2)
                end
                return
            end

            if param1 == "npcLevelUp" and adType == Define.AdType.Product then
                player:showWatchAdUpdateProductInfo()
                return
            end

            if adType == Define.AdType.OpenGift then
                if param1 == "OpenGift" then
                    HostApi.log("onPlayAdResult_openGift_suc")
                    if player.enableIndie then
                        GameAnalytics:design(player.userId, 0, { "g1049openGift_lookAd_success_indie", param2})
                    else
                        GameAnalytics:design(player.userId, 0, { "g1049openGift_lookAd_success", param2})
                    end

                    local cfg = ChristmasConfig:getGiftdataByType(param2)
                    if cfg == nil  then
                        return
                    end

                    if cfg.OpenType == Define.openGiftType.LOOK_AD then
                        ChristmasConfig:openChirstmasGiftSuccess(player, param2, Define.DrawGiftType.Advertise)
                    end

                end
                return
            end
        end
    end
end

function PlayerListener.onPlayerSellItem(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if player.isLife == false then
        HostApi.log("onPlayerSellItem when player is die userId: " .. tostring(player.userId))
        return
    end

    local builder = DataBuilder.new():fromData(data)
    local selectIndex = builder:getNumberParam("selectIndex")
    local itemId = builder:getNumberParam("itemId")
    local itemMeta = builder:getNumberParam("itemMeta")
    local num = builder:getNumberParam("num")

    local item = SellItemConfig:getSellItem(itemId, itemMeta)
    local cur_num = player:getItemNumById(itemId, itemMeta)

    HostApi.log("onPlayerSellItem userId: " .. tostring(player.userId) .. " ".. selectIndex .. " " .. itemId .. " " .. itemMeta .. " " .. num)

    if item then
        if num >= 1 and num <= 64 and cur_num >= num then
            local inv = player:getInventory()
            if inv then
                local info = inv:getItemStackInfo(selectIndex)
                if info and info.num >= num and item.money > 0 then
                    -- player.money = player.money + num * item.money
                    inv:decrStackSize(selectIndex, num)
                    local money = player.money + num * item.money
                    player:setCurrency(money)
                    GameAnalytics:design(player.userId, 0, { "g1048addMoney", tostring(player.userId), num * item.money})
                    HostApi.notifyGetItem(player.rakssid, 10000, 0, num * item.money)
                    player.taskController:packingProgressData(player, Define.TaskType.collect, GameMatch.gameType, itemId, num, itemMeta)
                    LuaTimer:scheduleTimer(function (userId)
                        HostApi.refreshSellItemSuc(player.rakssid)
                        if itemId == 511 and itemMeta == 0 then
                            player:checkTreasureInInvetory()
                        end
                    end, 1000, 1, tostring(player.userId))
                end
            end
        end
    end

end

function PlayerListener.onReceiveTaskReward(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local taskId = builder:getNumberParam("taskId")
    local status = builder:getNumberParam("status")

    if DbUtil:CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE) and DbUtil:CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        HostApi.log(" taskId =   "..taskId.." status = "..status)
        player.taskController:receiveTaskReward(player, taskId, status)
    end
end

function PlayerListener.SkyBlockDareTaskOperate(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end
    local builder = DataBuilder.new():fromData(data)
    local type = builder:getNumberParam("type")
    local has_ad = builder:getBoolParam("has_ad")

    player.has_ad = has_ad

    if DbUtil:CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE) and DbUtil:CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        if type == Define.dareTaskOperate.AddTimes then
            if (player:checkTaskTimesIsFull()) then
                HostApi.sendCommonTip(player.rakssid, "check_task_times_is_full")
            else
                local cfg = GameConfig.buyDaretask
                HostApi.showConsumeCoinTip(player.rakssid, cfg.buyMsg, tonumber(cfg.coinId), tonumber(cfg.coinNum), Define.dareTaskTip.AddTimes.. "#" .. tostring(player.rakssid))
            end
        elseif type == Define.dareTaskOperate.Refresh then
            if player.free_refresh_times > 0 then
                player.free_refresh_times = player.free_refresh_times - 1
                player:refreshDareTaskSuccess()
            else
                local cfg = GameConfig.refreshDareTask
                HostApi.showConsumeCoinTip(player.rakssid, cfg.msg, tonumber(cfg.coinId), tonumber(cfg.coinNum), Define.dareTaskTip.Refresh.. "#" .. tostring(player.rakssid))
            end
        elseif type == Define.dareTaskOperate.Updata then
            player:sendDareTaskInfo()
        end
    end
    return true
end

function PlayerListener.onPlayerDiedTip(diePlayer, killPlayer)
    return false
end

function PlayerListener.onPlayerUseInitSpawnPosition(userId, position)
    local targetUserId = GameMatch:getTargetByUid(userId)
    if targetUserId ~= nil then
        local targetManorInfo = GameMatch:getUserManorInfo(targetUserId)
        if targetManorInfo ~= nil and targetManorInfo.manorIndex ~= 0 then
            local pos  = GameConfig:getInitPosByManorIndex(targetManorInfo.manorIndex)
            position.x = pos.x
            position.y = pos.y
            position.z = pos.z
            return false
        end
    end

    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onBuyMoney(userId, data)
    print("onBuyMoney  userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local curTable = json.decode(data)
    if curTable then
        player:buyMoney(curTable.level)
    end
end


function PlayerListener.PlayerBuyChirstmasGiftEvent(userId, data)
    print("PlayerBuyChirstmasGiftEvent  userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if not GameMatch:checkIfChristmasFinish() then
        return
    end

    local curTable = json.decode(data)
    if curTable then
        ChristmasConfig:buyChirstmasGift(userId, curTable.giftType)
    end
end

function PlayerListener.onPlayerGetGiftNumEvent(userId, data)
    print("onPlayerGetGiftNumEvent  userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    player:sendGiftNum()
end

function PlayerListener.onPlayerOpenGiftEvent(userId, data)
    print("onPlayerOpenGiftEvent  userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local curTable = json.decode(data)
    if curTable then
        ChristmasConfig:openChirstmasGift(player,curTable)
    end

end

function PlayerListener.onPlayerCommonClick(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local click = json.decode(data)
    if player.enableIndie then
        GameAnalytics:design(player.userId, 0, { "g1048common_click_indie", click.type})
    else
        GameAnalytics:design(player.userId, 0, { "g1048common_click", click.type})
    end
    --if click.type == Define.ClickType.TYPE11 then
    --    GameAnalytics:design(player.userId, 0, { "g1048common_click", Define.ClickType.TYPE11})
    --end
    --if click.type == Define.ClickType.TYPE12 then
    --    GameAnalytics:design(player.userId, 0, { "g1048common_click", Define.ClickType.TYPE12})
    --end
    --if click.type == Define.ClickType.TYPE5 then
    --    GameAnalytics:design(player.userId, 0, { "g1048common_click", Define.ClickType.TYPE5})
    --end
end

function PlayerListener.onRedSwitchClick(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    print(" onRedSwitchClick userId == "..tostring(userId))
    local curTable = json.decode(data)
    if curTable then
        player.notShowSwitchRed = true
    end
end

function PlayerListener.onRedRichBtnClickEvent(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    print(" onRedRichBtnClickEvent userId == "..tostring(userId))
    local curTable = json.decode(data)
    if curTable then
        player.notShowRichRed = true
        HostApi.sendSkyblockShowAppShopPage(player.rakssid, AppShopConfig.TabType.new_block)
    end
end

function PlayerListener.onReceiveSumRecharge(player, type, id, count, meta)
    print("onReceiveSumRecharge userId == "..tostring(player.userId))
    RechargeUtil:doSumRechargeRewards(player, type, id, count, meta)
end

function PlayerListener.onReceiveSumRechargeEnd(player, id)
    print("onReceiveSumRechargeEnd userId == "..tostring(player.userId))
end

return PlayerListener
--endregion
