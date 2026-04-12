--region *.lua

require "messages.Messages"
require "data.GamePlayer"
require "Match"

PlayerListener = {}

function PlayerListener:init()

    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerRespawnEvent, self.onPlayerRespawn)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)
    BaseListener.registerCallBack(PlayerDieTipEvent, self.onPlayerDiedTip)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerBackHallEvent.registerCallBack(self.onPlayerBackHall)
    PlayerPickupItemEndEvent.registerCallBack(self.onPlayerPickupItemEnd)
    PlayerAttackCreatureEvent.registerCallBack(self.onPlayerAttackCreature)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    WatchAdvertFinishedEvent.registerCallBack(self.onWatchAdvertFinished)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerChangeItemInHandEvent.registerCallBack(self.onChangeItemInHandEvent)

    SkyBlockReceiveTaskRewardEvent:registerCallBack(self.onReceiveTaskReward)
    PlayerSellItemEvent:registerCallBack(self.onPlayerSellItem)
    PlayerSkyBlocShopOperateEvent:registerCallBack(self.onPlayerSkyBlocBuyGoods)
    SkyBlockDareTaskOperateEvent:registerCallBack(self.SkyBlockDareTaskOperateEvent)
    PlayerExchangeItemEvent.registerCallBack(self.onPlayerExchangeItem)
    PlayerAttackSessionNpcEvent.registerCallBack(self.onPlayerAttackSessionNpc)
    PlayerCancelGuideArrowEvent:registerCallBack(self.onPlayerCancelGuideArrow)
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
    DbUtil:getPlayerData(player.userId)
    HostApi.sendPlaySound(player.rakssid, 10042)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    player:initHintText()
    local players = PlayerManager:getPlayers()
    for _, other_player in pairs(players) do
        if tostring(other_player.rakssid) ~= tostring(player.rakssid) then
            print("onPlayerReady other_player.rakssid : "..tostring(other_player.rakssid) .."   player.rakssid : "..tostring(player.rakssid))
            other_player.readyShowTreasure = false
            other_player:checkTreasureInInvetory(player.rakssid)
        end
    end
    LuaTimer:scheduleTimer(function( )
        ChestConfig.isPlayerReadyFinish = true
    end,15000, 1)
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)

    if hurter == nil then
        return false
    end

    local pos = hurter:getPosition()
    local canPvp = GameConfig:isInArea(pos, GameConfig.pvpArea, true)
    if not canPvp then
        if damageType == "lava" or damageType == "inFire" then
            local currentHp = hurtPlayer:getMaxHealth()
            hurtValue.value = (currentHp * GameConfig.lavaDamageCoe)
            return true
        end
        if damageType == "onFire" then
            hurtValue.value = GameConfig.onFireDamage
            return true
        end
        return true
    end

    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier then
        RespawnHelper.sendRespawnCountdown(dier, 0)
        local killer = PlayerManager:getPlayerByPlayerMP(killPlayer)
        dier.isLife = false
        dier:setInventory()
        --dier.inventory = {}
        --BackHallManager:addPlayer(dier)
        if iskillByPlayer and killer then
            killer.taskController:packingProgressData(killer, Define.TaskType.kill_player, GameMatch.gameType, 0, 1, 0)
            GameAnalytics:design(dier.userId, 0, { "g1049PlayerKilledByPlayer", tostring(dier.userId), tostring(killer.userId)} )
            GameAnalytics:design(killer.userId, 0, { "g1049PlayerKillOtherPlayer", tostring(killer.userId), tostring(dier.userId)} )
        end

        if not iskillByPlayer then
            GameAnalytics:design(dier.userId, 0, { "g1049PlayerKilledByMonster", tostring(dier.userId)} )
        end

        local pos = dier:getPosition()
        -- HostApi.log("onPlayerDied  diedPos : "..pos.x.." y : "..pos.y.." z : "..pos.z)
        dier.diedPos = pos--VectorUtil.toVector3i(pos)--toBlockVector3(pos.x, pos.y, pos.z)
        local canPvp = GameConfig:isInArea(pos, GameConfig.pvpArea, true)
        if not canPvp then
            dier:changeGuideArrow(false)
            GameAnalytics:design(dier.userId, 0, { "g1049PlayerDieInPvpArea", tostring(dier.userId)} )
        end
        dier:DropMoney()
    end
    return true
end

function PlayerListener.onPlayerRespawn(player)
    player.isLife = true
    player:changeMaxHealth(GameConfig.initHP)
    player:fillInvetoryBySlot(player.inventory, true)
    player.readyShowTreasure = false
    player:checkTreasureInInvetory()
    local maxHp = player:getMaxHealth()
    player:changeMaxHealth(maxHp)
    player:setFoodLevel(20)
    player:teleInitPos()
    player:addNightVisionPotionEffect(99999)
    PrivilegeConfig:checkSpePri(player)
    player.isNeedGuideArrow = false
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end

    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)
    if player == nil then
        return false
    end
    local playerPos = VectorUtil.newVector3(x, y, z)
    GemStoneConfig:inGemStoneArea(player, playerPos)
    if player.isNeedGuideArrow then
        if GameConfig:isInArea(playerPos, GameConfig.ArrowArea, false) then
            player:changeGuideArrow(false)
        end
    end
    local canPvp = GameConfig:isInArea(playerPos, GameConfig.pvpArea, true)
    if not canPvp then
        player.isInPvp = true
    else
        player.isInPvp = false
    end
    return true
end

function PlayerListener.onPlayerBackHall(rakssid)
    HostApi.log("onPlayer leave for SkyBlockMain")
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local interval =  GameConfig.backHomeCD + player.backHomeTime - os.time()
    if interval > 0 then
        MsgSender.sendBottomTipsToTarget(player.rakssid, 1, Messages:msgUIButtonCountDown(interval))
        return
    else
        player.backHomeTime = os.time()
    end
    local pos = player:getPosition()
    local canPvp = GameConfig:isInArea(pos, GameConfig.pvpArea, true)
    if not canPvp then
        MsgSender.sendCenterTipsToTarget(rakssid, 1, Messages:msgCannotBackHome())
        player:changeGuideArrow(true)
        if player.enableIndie then
            GameAnalytics:design(player.userId, 0, { "g1049pvp_click_home_indie", "g1049pvp_click_home_indie"} )
        else
            GameAnalytics:design(player.userId, 0, { "g1049pvp_click_home", "g1049pvp_click_home"} )
        end

        return
    end
    BackHallManager:addPlayer(player)
    MsgSender.sendTopTipsToTarget(player.rakssid, 1000, Messages:msgTransmitting())
    if player.enableIndie then
        GameAnalytics:design(player.userId, 0, { "g1049pve_click_home_indie", "g1049pve_click_home_indie"} )
    else
        GameAnalytics:design(player.userId, 0, { "g1049pve_click_home", "g1049pve_click_home"} )
    end
end

function PlayerListener.onPlayerAttackCreature(rakssid, entityId, attackType, vec3, damage, currentItemId)

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    MonsterManager:onPlayerAttackCreature(player, entityId, vec3, damage, currentItemId)

    return true
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

    PrivilegeConfig:setPriImgById(player, itemId)
    
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

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta, stackSize, itemIndex)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    -- print("onPlayerDropItem itemId is : "..itemId.." itemMeta is : "..itemMeta.." itemIndex : "..itemIndex)
    HostApi.log("onPlayerDropItem userId: " .. tostring(player.userId)
            .. " itemId: " .. itemId .. " itemMeta: " .. itemMeta .. " stackSize: " .. stackSize .. " itemIndex: " .. itemIndex)
    if itemId == 511 and itemMeta == 0 then
        --player:removeItem(itemId, 1 ,itemMeta)
        local inv = player:getInventory()
        if inv then
            local info = inv:getItemStackInfo(itemIndex)
            if info and info.num >= 1 then
                inv:decrStackSize(itemIndex, 1)
                GemStoneConfig:initGemStone(player:getPosition(), player:getYaw(), 3)
                LuaTimer:scheduleTimer(function(rakssid)
                    local player = PlayerManager:getPlayerByRakssid(rakssid)
                    if player then
                        player:checkTreasureInInvetory()
                    end
                end, 500, 1, rakssid)
            end
        end
        return false
    end
    return true
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

function PlayerListener.SkyBlockDareTaskOperateEvent(userId, data)
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


function PlayerListener.onWatchAdvertFinished(rakssid, adType, params, code)
   if code ~= 1 then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    HostApi.log("param =="..params.."  code == "..code )
    if code == 1 then
        local info = StringUtil.split(params, "#")
        if #info == 2 then
            local param1 = tostring(info[1])
            local param2 = tonumber(info[2])

            if adType == Define.AdType.Task then
                if param1 == "TaskAd" then
                    HostApi.log("onPlayAdResult_task_suc")
                    if player.enableIndie then
                        GameAnalytics:design(player.userId, 0, { "g1049task_lookAd_success_indie", param2})
                    else
                        GameAnalytics:design(player.userId, 0, { "g1049task_lookAd_success", param2})
                    end

                    player.taskController:packingProgressData(player, Define.TaskType.look_ad, GameMatch.gameType, 0, 1, -1, param2)
                end
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

    if player.isInPvp then
        GamePacketSender:sendNotifySellItemFail(player.rakssid)
        return
    end

    local builder = DataBuilder.new():fromData(data)
    local selectIndex = builder:getNumberParam("selectIndex")
    local itemId = builder:getNumberParam("itemId")
    local itemMeta = builder:getNumberParam("itemMeta")
    local num = builder:getNumberParam("num")

    local item = SellItemConfig:getSellItem(itemId, itemMeta)
    local cur_num = player:getItemNumById(itemId, itemMeta)

    HostApi.log("onPlayerSellItem userId: " .. tostring(player.userId) .. " " .. selectIndex .. " " .. itemId .. " " .. itemMeta .. " " .. num)

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


function PlayerListener.onPlayerDiedTip(diePlayer, killPlayer)
    return false
end


function PlayerListener.onChangeItemInHandEvent(rakssid, itemId, itemMeta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player.inHandItemId = itemId
    end
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerExchangeItem(rakssid, type, itemId, itemMeta, itemNum, bullets, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    -- print("onPlayerExchangeItem type is : "..type.." itemId : "..itemId.. " itemNum : "..itemNum.. " itemMeta : "..itemMeta)
    LuaTimer:scheduleTimer(function(rakssid)
        local player = PlayerManager:getPlayerByRakssid(rakssid)
        if player then
            if itemId == 511 and itemMeta == 0 then
                player:checkTreasureInInvetory()
            end
        end
    end, 1000, 1, rakssid)

    PrivilegeConfig:setPriImgById(player, itemId)
    HostApi.log("onPlayerExchangeItem userId: " .. tostring(player.userId)
            .. " itemId: " .. itemId .. " itemMeta: " .. itemMeta .. " itemNum: " .. itemNum .. " type: " .. type)
    return true
end

function PlayerListener.onPlayerAttackSessionNpc(rakssid, npcEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    ChestConfig:onPlayerClickChest(player)
    HostApi.log("onPlayerAttackSessionNpc player.rakssid is :"..tostring(rakssid).." the npcEntityId is : "..npcEntityId)
end

function PlayerListener.onPlayerCancelGuideArrow(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    player:changeGuideArrow(false)
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
