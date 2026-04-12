--region *.lua

require "messages.Messages"
require "data.GamePlayer"
require "Match"
require "data.GameSignIn"


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

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerCallOnTargetManorEvent.registerCallBack(self.onPlayerCallOnTargetManor)
    PlayerChangeItemInHandEvent.registerCallBack(self.onChangeItemInHandEvent)
    BlockStationaryNotStationaryEvent.registerCallBack(self.onBlockStationaryNotStationary)
    PlayerPlaceBlockBoxJudgeEvent.registerCallBack(self.onPlayerPlaceBlockBoxJudgeEvent)
    PlayerPickupItemEndEvent.registerCallBack(self.onPlayerPickupItemEnd)
    PlayerBackHallEvent.registerCallBack(self.onPlayerLeaveForMiningArea)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerExchangeItemEvent.registerCallBack(self.onPlayerExchangeItem)
    WatchAdvertFinishedEvent.registerCallBack(self.onWatchAdvertFinished)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerCloseContainerEvent.registerCallBack(self.onPlayerCloseContainer)
    PlayerUseInitSpawnPositionEvent.registerCallBack(self.onPlayerUseInitSpawnPosition)
    UserManorReleaseEvent.registerCallBack(self.onUserManorRelease)
    PlayerAttackSessionNpcEvent.registerCallBack(self.onPlayerAttackSessionNpc)

    PlayerSkyBlocShopOperateEvent:registerCallBack(self.onPlayerSkyBlocBuyGoods)
    SkyBlockReceiveTaskRewardEvent:registerCallBack(self.onReceiveTaskReward)
    PlayerSellItemEvent:registerCallBack(self.onPlayerSellItem)
    PlayerClickResourceIslandEvent:registerCallBack(self.onPlayerClickResourceIsland)
    PlayerExtendAreaEvent:registerCallBack(self.onPlayerExtendArea)
    PlayerSetHomeEvent:registerCallBack(self.onPlayerSetHome)
    PlayerBackHomeEvent:registerCallBack(self.onPlayerBackHome)
    SkyBlockDareTaskOperateEvent:registerCallBack(self.SkyBlockDareTaskOperateEvent)
    PlayerReciveSignInRewardEvent:registerCallBack(self.onPlayerReciveSignInReward)
    SkyBlockReciveNewPriEvent:registerCallBack(self.SkyBlockReciveNewPri)

    SkyBlockCreatePartyEvent:registerCallBack(self.SkyBlockCreateParty)
    SkyBlockApplyPartyEvent:registerCallBack(self.SkyBlockApplyParty)
    SkyBlockGiveLoveEvent:registerCallBack(self.SkyBlockGiveLove)
    ToolBarGiveLoveEvent:registerCallBack(self.ToolBarGiveLove)
    ApplySignPostEvent:registerCallBack(self.onApplySignPost)

    GetTogetherBuildDataEvent:registerCallBack(self.onGetTogetherBuildData)
    KickOutPlayerEvent:registerCallBack(self.onKickOutPlayer)
    OtherPlayerUpdateDataEvent:registerCallBack(self.onOtherPlayerUpdateData)
    PlayerGotoHallEvent:registerCallBack(self.onPlayerGotoHall)
    PlayerShowTipsEvent:registerCallBack(self.onPlayerShowTips)
    PlayerClickBulletinEvent:registerCallBack(self.onClickBulletin)
    PlayerBuyMoneyEvent:registerCallBack(self.onBuyMoney)
    PlayerBuyChirstmasGiftEvent:registerCallBack(self.PlayerBuyChirstmasGiftEvent)
    PlayerGetChristmasRewardEvent:registerCallBack(self.onPlayerGetChristmasReward)
    PlayerGetGiftNumEvent:registerCallBack(self.onPlayerGetGiftNumEvent)
    PlayerOpenGiftEvent:registerCallBack(self.onPlayerOpenGiftEvent)
    PlayerCommonClickEvent:registerCallBack(self.onPlayerCommonClick)
    SkyBlockRedSwitchClickEvent:registerCallBack(self.onRedSwitchClick)
    ReceiveSumRechargeEvent:registerCallBack(self.onReceiveSumRecharge)
    ReceiveSumRechargeEndEvent:registerCallBack(self.onReceiveSumRechargeEnd)
    SkyBlockRedRichBtnClickEvent:registerCallBack(self.onRedRichBtnClickEvent)


    SkyBlockPartyAlmostOverEvent.registerCallBack(self.onSkyBlockPartyAlmostOver)
    SkyBlockPartyOverEvent.registerCallBack(self.onSkyBlockPartyOver)
    SkyBlockFriendUpdateEvent.registerCallBack(self.SkyBlockFriendUpdate)
    PlayerAttackActorNpcEvent.registerCallBack(self.onPlayerAttackActorNpc)
    PlayerRecycleChristmastreeEvent:registerCallBack(self.onPlayerRecycleChristmastree)
    OtherSessionNpcInfoEvent.registerCallBack(self.onOtherSessionNpcInfo)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    GameParty:partyAddPlayer(player.userId)
    GameMultiBuild:onPlayerLogin(player)
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    if player == nil then
        return 0
    end

    if player:visitorCheckCanGetBlockData() == false then
        HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
        return 0
    end

    if player.manorIndex <= 0 or player.manorIndex > #GameConfig.manorStart then
        HostApi.log("PlayerListener.onPlayerReady manorIndex error userId: " .. tostring(player.userId) .. " " .. player.manorIndex)
        HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
        return 0
    end

    local manorInfo = GameMatch:getUserManorInfo(player.userId)
    if manorInfo ~= nil then
        manorInfo:masterLogin()
        manorInfo:refreshTime()
        manorInfo:initAllTree()
        manorInfo:initAllDirt()
        if not DBManager:isLoadDataFinished(player.userId, DbUtil.TAG_CHEST) then
            DbUtil.getPlayerData(player.userId, DbUtil.TAG_CHEST)
        end
        if not DBManager:isLoadDataFinished(player.userId, DbUtil.TAG_FURNACE) then
            DbUtil.getPlayerData(player.userId, DbUtil.TAG_FURNACE)
        end
        if not DBManager:isLoadDataFinished(player.userId, DbUtil.TAG_MANOR) then
            DbUtil.getPlayerData(player.userId, DbUtil.TAG_MANOR)
        end

        player.manorIsExistWhenReady = true
    else
        player.manorIsExistWhenReady = false
    end

    DbUtil.getPlayerData(player.userId, DbUtil.TAG_SHAREDATA)
    DbUtil.getPlayerData(player.userId, DbUtil.TAG_PLAYER)
    DbUtil.getPlayerData(player.userId, DbUtil.TAG_MIAN_LINE)
    DbUtil.getPlayerData(player.userId, DbUtil.DIALOG_TIPS)
    RankConfig:getPlayerRankData(player)

    HostApi.sendPlaySound(player.rakssid, 10041)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))

    -- go home
    if GameMatch:isMaster(player.userId) then
        HostApi.log("PlayerListener.onPlayerReady go home isMaster" .. tostring(player.userId))
        if GameMatch:hasErrorDataInManor(player.userId) then
            HostApi.log("PlayerListener.onPlayerReady master: other has the same manorIndex sendGameover " .. tostring(player.userId))
            HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
            return 0
        end

        local selfManorInfo = GameMatch:getUserManorInfo(player.userId)
        if selfManorInfo and selfManorInfo.isNotInPartyCreateOrOver == false then
            HostApi.log("PlayerListener.onPlayerReady go home manor is in partyCreateOrOver " .. tostring(player.userId))
            HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
            return 0
        end
    else
        HostApi.log("PlayerListener.onPlayerReady is vistor" .. tostring(player.userId) .. " " .. tostring(player.target_user_id))
        if GameMatch:hasErrorDataInManor(player.target_user_id) then
            HostApi.log("PlayerListener.onPlayerReady vistor: other has the same manorIndex sendGameover " .. tostring(player.target_user_id))
            HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
            return 0
        end

        local targetManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
        if targetManorInfo then
            if targetManorInfo.isNotInPartyCreateOrOver == false then
                HostApi.log("PlayerListener.onPlayerReady vistor targetManor is in partyCreateOrOver " .. tostring(player.userId) .. " manorId: " .. tostring(targetManorInfo.userId))
                HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
                return 0
            else
                local targetPlayer = PlayerManager:getPlayerByUserId(player.target_user_id)
                if targetPlayer then
                    local data = {}
                    data.targetName = targetPlayer.name
                    data.show = true
                    GamePacketSender:sendWelcomeVisitorTip(player.rakssid, json.encode(data))
                else
                    local items = {}
                    local key = ""
                    local userIds = {}
                    table.insert(userIds, player.target_user_id)
                    UserInfoCache:GetCacheByUserIds(userIds, function(items)
                        local targetPlayer = UserInfoCache:GetCache(player.target_user_id)
                        if targetPlayer then
                            local data = {}
                            data.targetName = targetPlayer.name
                            data.show = true
                            GamePacketSender:sendWelcomeVisitorTip(player.rakssid, json.encode(data))
                        end
                    end, key, items)
                end

                HostApi.log("PlayerListener.onPlayerReady visitorInHouse userId: " .. tostring(player.userId) .. " manorId: " .. tostring(targetManorInfo.userId))
                targetManorInfo:visitorInHouse(player.userId)
            end
        end
    end

    GameParty:getPartyInfo(player.userId)
    GameParty:getLoveNum(player.userId)
    GameParty:getFriendPartyList(player.userId)
    GameParty:sendAllPartyData(player.rakssid)
    GameParty:getFreeCreateNum(player.userId)
    GameMultiBuild:upDataBuildGroupData(player.userId, Define.visitMasterTips.onShow)
    player:getOwnPicUrl()

    local players = PlayerManager:getPlayers()
    for _, other_player in pairs(players) do
        if tostring(other_player.rakssid) ~= tostring(player.rakssid) then
            print("PlayerListener.onPlayerReady other_player.rakssid : "..tostring(other_player.rakssid) .."   player.rakssid : "..tostring(player.rakssid))
            other_player.readyShowTreasure = false
            other_player:checkTreasureInInvetory(player.rakssid)
        end
    end
    player:initWelcomeText()
    return 0
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if damageType == "fall"
            or damageType == "arrow" then
        return false
    end
    if damageType == "player" then
        local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
        local hurtFrom = PlayerManager:getPlayerByPlayerMP(hurtFrom)
        if hurter and hurtFrom then
            local data = {}
            data.targetUserId = tostring(hurter.userId)
            data.name = tostring(hurter.name)
            data.isShowExpel = GameMultiBuild:clickPlayerisShowExpel(hurtFrom.userId, hurter.userId)
            GamePacketSender:sendFriendOperation(hurtFrom.rakssid, json.encode(data))
            return false
        end
    end

    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if hurter == nil then
        return false
    end
    if os.time() - hurter.invinciblyTime < GameConfig.invinciblyCD then
        return false
    end
    if damageType == "lava" or damageType == "inFire" then
        local currentHp = hurtPlayer:getMaxHealth()
        hurtValue.value = (currentHp * GameConfig.lavaDamageCoe)
        --print("hurter currentHp is " .. currentHp .. " hurtValue " ..  hurtValue.value)
        return true
    end
    if damageType == "onFire" then
        hurtValue.value = GameConfig.onFireDamage
    end
    --print("damageType is " .. damageType .. " " .. hurtValue.value)
    return true
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
    player.invinciblyTime = os.time()
    player:changeMaxHealth(GameConfig.initHP)
    player:fillInvetoryBySlot(player.inventory)
    player.readyShowTreasure = false
    player:checkTreasureInInvetory()
    local maxHp = player:getMaxHealth()
    player:changeMaxHealth(maxHp)
    player:setFoodLevel(20)
    player:teleInitPos()
    PrivilegeConfig:checkSpePri(player)
    return 43200
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
    FuncNpcConfig:onPlayerMove(player, x, y, z)
    if y <= GameConfig.deathLineY then
        -- print("deathLineY" .. GameConfig.deathLineY)
        player.invinciblyTime = os.time()
        local maxHp = player:getMaxHealth()
        player:setHealth(maxHp)
        player:setFoodLevel(20)
        -- player:setInventory()
        -- player:fillInvetoryBySlot(player.inventory)
        player:teleInitPos()
        return false
    end
    player:onMove(x, y, z)
    return true
end

function PlayerListener.onDynamicAttrInit(attr, gamePartyUserId)
    if attr.manorId < 0 then
        return
    end
    local platformId = attr.userId
    local targetUserId = attr.targetUserId
    local manorIndex = attr.manorId
    HostApi.log("==== LuaLog: PlayerListener.onDynamicAttrInit  --- start --- : platformId = "
            .. tostring(platformId) .. ", targetUserId = " .. tostring(targetUserId) .. ", manorIndex = " .. manorIndex .. ", gamePartyUserId = " .. tostring(gamePartyUserId))
    if tostring(targetUserId) ~= "0" and manorIndex > 0 then
        GameMatch:initCallOnTarget(platformId, targetUserId)

        local userManorInfo = GameMatch.userManorInfo[tostring(targetUserId)]
        if userManorInfo == nil then

            local isRepeatManor, repeatManor = GameMatch:isRepeatManorIndex(manorIndex)
            if isRepeatManor == true then
                if repeatManor and repeatManor.isInRoomForceRelease == false then
                    HostApi.log("it may be exist repeat manorIndex when a new manor is create check it !")
                end
            end

            HostApi.log("==== LuaLog: GameManor.new : " .. tostring(targetUserId))
            local manorInfo = GameManor.new(manorIndex, targetUserId)
            GameMatch:initUserManorInfo(targetUserId, manorInfo)
            manorInfo:initChunkIndex()

        else
            HostApi.log("==== LuaLog: userManorInfo:refreshTime : " .. tostring(targetUserId) .. " " .. userManorInfo.release_status)

            userManorInfo:refreshTime()
            if userManorInfo.release_status ~= Define.manorRelese.Not then
                userManorInfo:initChunkIndex()
            end

            --if tostring(platformId) ~= tostring(targetUserId) then
            --    userManorInfo:visitorInHouse(platformId)
            --end
        end
    else
        HostApi.log("==== LuaLog: PlayerListener.onDynamicAttrInit [targetUserId == 0] or [manorIndex <= 0] ")
    end

    if tostring(platformId) == tostring(targetUserId) and tostring(platformId) == tostring(gamePartyUserId) then
        GameParty:getAllPartyList()
    end
end

function PlayerListener.onPlayerCallOnTargetManor(userId, targetId, from, isFriend, score)
    HostApi.log("onPlayerCallOnTargetManor userId: " .. tostring(userId) .. " targetId: " .. tostring(targetId) .. " from: " .. from)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return true
    end

    if tostring(userId) == tostring(targetId) then
        HostApi.log("onPlayerCallOnTargetManor self " .. tostring(userId))
        return true
    end

    if player.isNotInPartyCreateOrOver == false then
        return true
    end

    local targetPlayer = PlayerManager:getPlayerByUserId(targetId)
    if targetPlayer then
        local data = {}
        data.targetName = targetPlayer.name
        data.show = true
        GamePacketSender:sendWelcomeVisitorTip(player.rakssid, json.encode(data))
    end
    -- target player is not online, see as target manor is not in this server,
    -- because target player may be in party server, but his common manor is in this server
    -- if targetPlayer == nil then
    --     EngineUtil.sendEnterOtherGame(player.rakssid, "g1048", targetId)
    --     return true
    -- end

    if targetPlayer and targetPlayer.isNotInPartyCreateOrOver == false then
        return true
    end

    if from == 1001 or from == 1002 or from == 1003 or from == 1004 then
        if player.enableIndie then
            GameAnalytics:design(player.userId, 0, { "g1048join_party_suc_indie", from})
        else
            GameAnalytics:design(player.userId, 0, { "g1048join_party_suc", from})
        end
    end

    if from == 2000 then
        if player.enableIndie then
            GameAnalytics:design(player.userId, 0, { "g1048join_build_group_indie", from})
        else
            GameAnalytics:design(player.userId, 0, { "g1048join_build_group", from})
        end
    end

    local manorInfo = GameMatch:getUserManorInfo(targetId)
    if not manorInfo then
        -- vistor is not in this server
        EngineUtil.sendEnterOtherGame(player.rakssid, "g1048", targetId)
        return true
    else
        if manorInfo.isNotInPartyCreateOrOver == false then
            HostApi.log("onPlayerCallOnTargetManor manor is in createPartyOrOver userId: " .. tostring(player.userId) .. " targetId: " .. tostring(targetId))
            EngineUtil.sendEnterOtherGame(player.rakssid, "g1048", targetId)
            return true
        end
    end

    -- out of vistors list of current manor
    GameMatch:leaveCurrentManor(userId)
    local notSameTarget = true
    if tostring(player.target_user_id) == tostring(targetId) then
        notSameTarget = false
    end

    -- vistor is in this server
    local manorIndex = 0
    local targetManorInfo = GameMatch:getUserManorInfo(targetId)
    if targetManorInfo ~= nil then
        manorIndex = targetManorInfo.manorIndex
        local pos = GameConfig:getInitPosByManorIndex(targetManorInfo.manorIndex)
        if pos then
            HostApi.log("callon in this server userId: " .. tostring(player.userId) .. " targetId: " .. tostring(targetManorInfo.userId))
            if notSameTarget then
                GameParty:partySubPlayer(userId)
            end
            GameMatch:initCallOnTarget(userId, targetId)
            player:teleportPos(pos)
            GameMultiBuild:upDataBuildGroupData(userId, Define.visitMasterTips.onShow)
            player:setRespawnPos(pos)
            player.isGenisland = false
            PrivilegeConfig:checkSpePri(player)
            player.manorIndex = targetManorInfo.manorIndex
            player.target_user_id = targetManorInfo.userId
            player:initChunkIndex()
            targetManorInfo:refreshTime()
            if targetManorInfo.release_status ~= Define.manorRelese.Not then
                targetManorInfo:initChunkIndex()
            end
            targetManorInfo:visitorInHouse(userId)
            if notSameTarget then
                GameParty:partyAddPlayer(userId)
            end
            GameParty:getLoveNum(userId)
            player:changeShowData()
        end
    end
    -- end

    return true

end

function PlayerListener.onChangeItemInHandEvent(rakssid, itemId, itemMeta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player.inHandItemId = itemId
    end
end

function PlayerListener.onBlockStationaryNotStationary(blockId)
    return false
end

function PlayerListener.onPlayerPlaceBlockBoxJudgeEvent(blockId)
    return false
end

function PlayerListener.onReceiveTaskReward(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local taskId = builder:getNumberParam("taskId")
    local status = builder:getNumberParam("status")

    if DbUtil.CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE) and DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        HostApi.log(" taskId =   "..taskId.." status = "..status)
        player.taskController:receiveTaskReward(player, taskId, status)
    end
end

function PlayerListener.onPlayerPickupItemEnd(rakssid, itemId, itemNum, meta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    if not player:CanSavePlayerShareData() then
        return false
    end

    PrivilegeConfig:setPriImgById(player, itemId)

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
    HostApi.log("onPlayerPickupItemEnd userId: " .. tostring(player.userId) .. " targetId: " .. tostring(player.target_user_id)
            .. " itemId: " .. itemId .. " itemNum: " .. itemNum .. " meta: " .. meta)
    return true
end

function PlayerListener.onPlayerLeaveForMiningArea(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        return
    end
    if player.isNextGameStatus == "initGameStatus" then
        player:gotoNextGame("g1049", true)
    end
end

function PlayerListener.onPlayerClickResourceIsland(userId,data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil or player.backProductTime == nil then
        return
    end

    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        return
    end
    if player.isNextGameStatus == "initGameStatus" then
        player:gotoNextGame("g1050", true)
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
    if DbUtil.CanSavePlayerData(player, DbUtil.TAG_MIAN_LINE) and DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
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

function PlayerListener.onPlayerExtendArea(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    player:checkExtendArea()
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta, stackSize, itemIndex)
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
    end, 500, 1, rakssid)
    -- print("onPlayerDropItem itemId is : "..itemId.." itemMeta is : "..itemMeta.." itemIndex : "..itemIndex)
	HostApi.log("onPlayerDropItem userId: " .. tostring(player.userId) .. " targetId: " .. tostring(player.target_user_id)
            .. " itemId: " .. itemId .. " itemMeta: " .. itemMeta .. " stackSize: " .. stackSize .. " itemIndex: " .. itemIndex)
    if itemId == 511 and itemMeta == 0 then
        --player:removeItem(itemId, 1 ,itemMeta)
        local inv = player:getInventory()
        if inv then
            local info = inv:getItemStackInfo(itemIndex)
            if info and info.num >= 1 then
                inv:decrStackSize(itemIndex, 1)
                GemStoneConfig:initGemStone(player:getPosition(), player:getYaw(), 3)
            end
        end
        return false
    end
    return true
end

function PlayerListener.onPlayerExchangeItem(rakssid, type, itemId, itemMeta, itemNum, bullets, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if player.isNotInPartyCreateOrOver == false then
        return false
    end

    local targetManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if targetManorInfo == nil then
        return false
    end

    if not targetManorInfo:isIn(vec3) then
        return false
    end

    if type == 2 then
        local inv = player:getInventory()
        if inv then
            if not inv:isCanAddItem(itemId, itemMeta, itemNum) then
                HostApi.sendCommonTip(rakssid, "gui_str_app_shop_inventory_is_full")
                return false
            end
        end
    end

    PrivilegeConfig:setPriImgById(player, itemId)

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

    HostApi.log("onPlayerExchangeItem userId: " .. tostring(player.userId) .. " targetId: " .. tostring(player.target_user_id)
            .. " itemId: " .. itemId .. " itemMeta: " .. itemMeta .. " itemNum: " .. itemNum .. " type: " .. type)

    return true
end

function PlayerListener.onPlayerSetHome(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    if not player.isLife then
        HostApi.log("onPlayerSetHome player.userId : .."..tostring(player.userId).." player.isLife == false ")
        return
    end
    if player.isGenisland then
        MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgSetHomeFailed())
        return
    end
    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_PLAYER) then
        return
    end

    if not GameMatch:isMaster(player.userId) then
        HostApi.sendCommonTip(player.rakssid, "you_must_back_to_use")
        return
    end

    if player:getEntityPlayer() ~= nil then
        if not player:getEntityPlayer():canFall() then
            local Pos = player:getPosition()
            local underBlockPos = VectorUtil.toBlockVector3(Pos.x, Pos.y - 1, Pos.z)
            local blockId = EngineWorld:getBlockId(underBlockPos)
            if blockId ~= BlockID.AIR and blockId ~= 916 then
                local manorInfo = GameMatch:getUserManorInfo(player.userId)
                local Pos = manorInfo:aPosToRPos(VectorUtil.newVector3(underBlockPos.x + 0.5, underBlockPos.y + 1, underBlockPos.z + 0.5))
                if Pos then
                    player.respawnPos = Pos
                    -- HostApi.log("onPlayerSetHome respawnPos aPos  " .. tostring(player.userId) .. " " .. underBlockPos.x .. " " .. underBlockPos.y .. " " .. underBlockPos.z )
                    -- HostApi.log("onPlayerSetHome respawnPos rPos  " .. tostring(player.userId) .. " " .. player.respawnPos.x .. " " .. player.respawnPos.y .. " " .. player.respawnPos.z )
                    MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgSetHomeSuccess())
                    player.yaw = player:getYaw()
                    if player.enableIndie then
                        GameAnalytics:design(player.userId, 0, { "g1048set_home_indie", "g1048set_home_indie"})
                    else
                        GameAnalytics:design(player.userId, 0, { "g1048set_home", "g1048set_home"})
                    end
                    return
                end
            end
        end
    end
    MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgForbidSetHome())
end

function PlayerListener.onPlayerBackHome(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    if not player.isLife then
        HostApi.log("onPlayerBackHome player.userId : .."..tostring(player.userId).." player.isLife == false ")
        return
    end
    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_PLAYER) then
        return
    end
    if player.isNextGameStatus == "initGameStatus" then
        player:gotoNextGame("g1048", true)
    end
    local data = {}
    data.targetName = player.name
    data.show = false
    GamePacketSender:sendWelcomeVisitorTip(player.rakssid, json.encode(data))
end

function PlayerListener.onPlayerCloseContainer(rakssid, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    local targetManorInfo = GameMatch:getUserManorInfo(player.userId)
    if targetManorInfo ~= nil then
        targetManorInfo:updateAllChestData(player)
        targetManorInfo:updateAllFurnaceData(player)
    end

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

    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
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
                        GameAnalytics:design(player.userId, 0, { "g1048task_lookAd_success_indie", param2})
                    else
                        GameAnalytics:design(player.userId, 0, { "g1048task_lookAd_success", param2})
                    end

                    player.taskController:packingProgressData(player, Define.TaskType.look_ad, GameMatch.gameType, 0, 1, -1, param2)
                end
                return
            end

            if adType == Define.AdType.OpenGift then
                if param1 == "OpenGift" then
                    HostApi.log("onPlayAdResult_openGift_suc")
                    if player.enableIndie then
                        GameAnalytics:design(player.userId, 0, { "g1048openGift_lookAd_success_indie", param2})
                    else
                        GameAnalytics:design(player.userId, 0, { "g1048openGift_lookAd_success", param2})
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


function PlayerListener.onPlayerDiedTip(diePlayer, killPlayer)
    return false
end

function PlayerListener.onPlayerUseInitSpawnPosition(userId, position)
    local targetUserId = GameMatch:getTargetByUid(userId)
    if targetUserId ~= nil then
        local targetManorInfo = GameMatch:getUserManorInfo(targetUserId)
        if targetManorInfo ~= nil and targetManorInfo.manorIndex >= 1 and targetManorInfo.manorIndex <= #GameConfig.manorStart then
            local pos  = GameConfig:getInitPosByManorIndex(targetManorInfo.manorIndex)
            position.x = pos.x
            position.y = pos.y
            position.z = pos.z
            return false
        end
    end

    return true
end

function PlayerListener.onUserManorRelease(userId)
    HostApi.log("==== LuaLog: PlayerListener.onUserManorRelease packet start " .. tostring(userId))

    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        HostApi.log("==== LuaLog: PlayerListener.onUserManorRelease is not online " .. tostring(userId))
        local targetManorInfo = GameMatch:getUserManorInfo(userId)
        if targetManorInfo then
            HostApi.log("==== LuaLog: PlayerListener.onUserManorRelease is not online releaseManor " .. tostring(userId) .. " manorIndex: " .. targetManorInfo.manorIndex)
            GameMatch:releaseManor(userId)
            targetManorInfo.isInRoomForceRelease = true
        end
    else
        HostApi.log("==== LuaLog: PlayerListener.onUserManorRelease is online " .. tostring(userId))
        local targetManorInfo = GameMatch:getUserManorInfo(userId)
        if targetManorInfo then
            HostApi.log("==== LuaLog: PlayerListener.onUserManorRelease is online releaseManor " .. tostring(userId) .. " manorIndex: " .. targetManorInfo.manorIndex)
            GameMatch:releaseManor(userId)
            targetManorInfo.isInRoomForceRelease = true
        end
        HostApi.log("==== LuaLog: PlayerListener.onUserManorRelease is online kickout " .. tostring(userId))
        HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
    end
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerReciveSignInReward(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    if not DBManager:isLoadDataFinished(player.userId, DbUtil.TAG_SHAREDATA) then
        return
    end

    if GameSignIn:isInTheTime() == false then
        return
    end

    local signInId = DataBuilder.new():fromData(data):getNumberParam("rewardId")
    GameSignIn:receiveSignInReward(player, signInId)
end

function PlayerListener.SkyBlockReciveNewPri(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if player.isLife == false then
        print("SkyBlockReciveNewPri player is die userid == "..tostring(userId))
        return
    end

    if player.recive_new_prive then
        PrivilegeConfig:onReciveNewPriGift(player)
        return
    end
    PrivilegeConfig:onReciveNewPri(player)
end

function PlayerListener.SkyBlockCreateParty(userId, data)
    print("SkyBlockCreateParty userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if not GameMatch:isMaster(userId) then
        HostApi.sendCommonTip(player.rakssid, "please_back_home_try_again")
        return
    end

    if player.isGenisland == true then
        HostApi.sendCommonTip(player.rakssid, "please_back_home_try_again")
        return
    end

    local curTable = json.decode(data)
    if curTable then
        if curTable.index == 1 and curTable.createType == Define.CreatePartyType.FREE 
            and player.freeCreatePartyTimes < Define.freeCreateTimes and player.freeCreatePartyTimes >= 0 then
            local cfg = PartyConfig:getPriceDataById(curTable.index)
            if cfg then
                player.msgTips = curTable.msgTips or "0"
                local info = {
                        userId = userId,
                        currency = Define.Money.GOLD_DIAMOND,
                        quantity = 0,
                        partyName = curTable.partyName,
                        partyDesc = curTable.partyDesc,
                        time = cfg.Time,
                        maxPlayerNum = GameConfig.partyMaxPlayer,
                        index = cfg.Id,
                        createType = Define.CreatePartyType.FREE,
                    }
                GameParty:createParty(info)
                return
            end
        end

        if curTable.createType == Define.CreatePartyType.NOT_FREE then
            local cfg = PartyConfig:getPriceDataById(curTable.index)
            if cfg then          
                if player:checkMoney(Define.Money.DIAMOND, cfg.Price) then
                    player.msgTips = curTable.msgTips or "0"
                    local info = {
                        userId = userId,
                        currency = Define.Money.GOLD_DIAMOND,
                        quantity = cfg.Price,
                        partyName = curTable.partyName,
                        partyDesc = curTable.partyDesc,
                        time = cfg.Time,
                        maxPlayerNum = GameConfig.partyMaxPlayer,
                        index = cfg.Id,
                        createType = Define.CreatePartyType.NOT_FREE,
                    }

                    GameParty:createParty(info)
                else
                    HostApi.sendCommonTip(player.rakssid, "lack_of_money")
                end
            end
        end
    end
end

function PlayerListener.SkyBlockApplyParty(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local curTable = json.decode(data)
    if curTable then
        GameParty:applyParty(userId, curTable.partyName, curTable.partyDesc)
    end
end


function PlayerListener.SkyBlockGiveLove(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if DbUtil.CanSavePlayerData(player, DbUtil.TAG_PLAYER) and DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA) then
        local builder = DataBuilder.new():fromData(data)
        local targetUserId = builder:getParam("targetUserId")
        if player:checkCanGiveLove(targetUserId) then
            GameParty:giveLoveInfo(player.userId, targetUserId)
        end
    end
end

function PlayerListener.ToolBarGiveLove(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local targetUserId = GameMatch:getTargetByUid(userId)
    if player:checkCanGiveLove(targetUserId) then
        GameParty:giveLoveInfo(userId, targetUserId)
    end
end

function PlayerListener.onSkyBlockPartyOver(userId)
    print("onSkyBlockPartyOver userId == "..tostring(userId))
    local manorInfo = GameMatch:getUserManorInfo(userId)
    if manorInfo then
        for k, v in pairs(manorInfo.visitor) do
            local visitor_player = PlayerManager:getPlayerByUserId(v)
            if visitor_player and tostring(visitor_player.userId) ~= tostring(userId) then
                visitor_player.tipsUserId = tostring(visitor_player.userId)
                visitor_player.tips = "party_is_over"
                -- HostApi.sendCommonTip(visitor_player.rakssid, "party_is_over_goto_home")
                if GameMultiBuild:checkIsMember(v, userId) then
                    visitor_player.tips = "party_over_back_master_islang"
                    visitor_player.tipsUserId = tostring(userId)
                    EngineUtil.sendEnterOtherGame(visitor_player.rakssid, "g1048", userId)
                    visitor_player.isNotInPartyCreateOrOver = false
                    GameMatch:partyGameOverToPlayer(v)
                elseif visitor_player:checkBackHome() then
                    visitor_player.isNotInPartyCreateOrOver = false
                    GameMatch:partyGameOverToPlayer(v)
                end
            end
        end

        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            player.tips = "party_is_over"
            player.tipsUserId = tostring(userId)
            player.isNotInPartyCreateOrOver = false
        end

        manorInfo.time = Define.startReleaseTime
        manorInfo.isNotInPartyCreateOrOver = false
        manorInfo:masterLogout()
        GameMatch:releaseManor(userId)
        if player then
            EngineUtil.sendEnterOtherGame(player.rakssid, "g1048", player.userId)
            -- HostApi.sendCommonTip(player.rakssid, "party_is_over_goto_home")
        end

        GameMatch:partyGameOverToPlayer(userId)
    end
end

function PlayerListener.onSkyBlockPartyAlmostOver(userId)
    print("onSkyBlockPartyAlmostOver userId == "..tostring(userId))
    local manorInfo = GameMatch:getUserManorInfo(userId)
    if manorInfo then
        for k, v in pairs(manorInfo.visitor) do
            local player = PlayerManager:getPlayerByUserId(v)
            if player then
                HostApi.sendCommonTip(player.rakssid, "party_almost_over")
            end
        end
    end

    local player = PlayerManager:getPlayerByUserId(userId)
    if player then
        HostApi.sendCommonTip(player.rakssid, "party_almost_over")
    end
end


function PlayerListener.onApplySignPost(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local tab = json.decode(data)
    print("onApplySignPost tab.editText 1 : "..tostring(tab.editText))
    print("onApplySignPost tab.cur_pos  2 : "..tostring(tab.cur_pos))
    if tab and tab.editText and tab.cur_pos then
        local x, y, z = GameConfig:decodeHash(tab.cur_pos)
        local vec3i = VectorUtil.newVector3i(x, y, z)
        local blockId = EngineWorld:getBlockId(vec3i)
        if blockId == BlockID.SIGN_POST or blockId == BlockID.SIGN_WALL then
            if not player:checkChunkLoad() then
                return
            end
            local master = PlayerManager:getPlayerByUserId(player.target_user_id)
            if master == nil then
                return
            end
            if master.isNotInPartyCreateOrOver == false then
                return false
            end
            if not DbUtil.CanSavePlayerData(master, DbUtil.TAG_SIGN) then
                return
            end
            local targetManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
            if targetManorInfo == nil then
                return
            end
            if targetManorInfo:isLoadingBlock() then
                return
            end
            -- do not have power
            if not targetManorInfo:checkPower(player.userId, vec3i, "onApplySignPost") then
                return
            end
            if targetManorInfo:checkSignPostTextLimit() and targetManorInfo:checkSignPostIsNew(vec3i) == nil and tab.editText ~= "" then
                MsgSender.sendCenterTipsToTarget(player.rakssid, 1, Messages:msgSignPostTextLimit(GameConfig.signPostNum))
            else
                targetManorInfo:recordSignPostText(vec3i, tostring(tab.editText), false)
                if player.enableIndie then
                    GameAnalytics:design(player.userId, 0, { "g1048write_sign_indie", "g1048write_sign_indie"})
                else
                    GameAnalytics:design(player.userId, 0, { "g1048write_sign", "g1048write_sign"})
                end
            end
        else
            print("onApplySignPost userId and blockId : "..tostring(userId)..tostring(blockId))
        end
    end
end

function PlayerListener.onGetTogetherBuildData(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    HostApi.log("onGetTogetherBuildData userId: " .. tostring(player.userId))
    GameMultiBuild:upDataBuildGroupData(userId, Define.visitMasterTips.notShow)
end

function PlayerListener.onOtherPlayerUpdateData(userId, data)
    print("OtherPlayerUpdateDataEvent userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local curTable = json.decode(data)
    if curTable then
        local player = PlayerManager:getPlayerByUserId(curTable.targetUserId)
        if player then
            GamePacketSender:sendMultiBuildUpdate(player.rakssid, data)
        else
            WebService:SendMsg({ tostring(curTable.targetUserId) }, data, 104803, "user", "g1048")
        end
    end
end

function PlayerListener.SkyBlockFriendUpdate(userId, data)
    print("SkyBlockFriendUpdate userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if data then
        GamePacketSender:sendMultiBuildUpdate(player.rakssid, data)
    end
end

function PlayerListener.onKickOutPlayer(userId, data)
    print("onKickOutPlayer  userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local curTable = json.decode(data)
    if curTable then
        local targetPlayer = PlayerManager:getPlayerByUserId(curTable.targetUserId)
        if targetPlayer == nil then
            return
        end

        if tostring(targetPlayer.target_user_id) == tostring(userId) then
            if curTable.tips ~= "" then
                targetPlayer.tips = curTable.tips
                targetPlayer.tipsUserId = tostring(targetPlayer.userId)
                targetPlayer:checkBackHome()
            end
        end
    end
end


function PlayerListener.onPlayerShowTips(userId, data)
    print("onPlayerShowTips  userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local curTable = json.decode(data)
    if curTable then
        local targetPlayer = PlayerManager:getPlayerByUserId(curTable.targetUserId)
        if targetPlayer == nil then
            return
        end

        if tostring(targetPlayer.target_user_id) == tostring(userId) then
            HostApi.sendCommonTip(targetPlayer.rakssid, curTable.tips)
        end
    end
end

function PlayerListener.onPlayerGotoHall(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    if player.isNextGameStatus ~= "initGameStatus" then
        return
    end
    local interval =  GameConfig.hallCD + player.backHallTime - os.time()
    if  interval > 0 then
        MsgSender.sendTopTipsToTarget(player.rakssid, 1, Messages:msgUIButtonCountDown(interval))
        return
    end
    -- monar is not in this server
    local selfManor = GameMatch:getUserManorInfo(player.userId)
    if selfManor == nil then
        HostApi.log("onPlayerGotoHall monar is not in this server and userId is : " .. tostring(player.userId))
    end

    if player.target_user_id == nil then
        return
    end
    local data = {}
    data.targetName = player.name
    data.show = false
    GamePacketSender:sendWelcomeVisitorTip(player.rakssid, json.encode(data))

    player:teleportHall()
    HostApi.log("onPlayerGotoHall userId and target_user_id: " .. tostring(player.userId) .. " " .. tostring(player.target_user_id))
end

function PlayerListener.onClickBulletin(userId, data)
    print("onClickBulletin  userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local curTable = json.decode(data)
    if curTable then
        player:CheckClickBulletin(curTable)
    end
end

function PlayerListener.onBuyMoney(userId, data)
    print("onBuyMoney  userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local curTable = json.decode(data)
    GameAnalytics:design(player.userId, 0, { "g1048click_money", curTable.level})
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

function PlayerListener.onPlayerAttackActorNpc(rakssid, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if not GameMatch:isMaster(player.userId) then
        return
    end

    local inv = player:getInventory()
    if inv and inv:getFirstEmptyStackInInventory() == -1 then
        HostApi.sendCommonTip(player.rakssid, "gui_str_app_shop_inventory_is_full")
        return
    end

    local targetManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
    if targetManorInfo:checkManorActorById(entityId) then
        local data = {
                EntityId = entityId
            }
        GamePacketSender:sendShowChristmastree(player.rakssid, data)
    end
end

function PlayerListener.onPlayerRecycleChristmastree(userId, data)
    print("onPlayerRecycleChristmastree  userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    local inv = player:getInventory()
    if inv and inv:getFirstEmptyStackInInventory() == -1 then
        HostApi.sendCommonTip(player.rakssid, "gui_str_app_shop_inventory_is_full")
        return
    end

    local tab = json.decode(data)
    if tab then
        local entityId = tonumber(tab.EntityId)
        if entityId == 0 then
            return
        end
        local targetManorInfo = GameMatch:getUserManorInfo(player.target_user_id)
        if targetManorInfo and targetManorInfo:checkManorActorById(entityId) then
            local itemId = targetManorInfo:removeActorById(entityId)
            if itemId > 0 then
                player:addItem(itemId, 1)
            end
        end
    end
end


function PlayerListener.onPlayerAttackSessionNpc(rakssid, npcEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if not GameMatch:checkIfChristmasFinish() then
        return
    end

    ChristmasNpcConfig:getNpcIdByEntityId(player, npcEntityId)
end

function PlayerListener.onOtherSessionNpcInfo(rakssid, timeValue)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    timeValue.value = player:checkCanGetRewardTime()
    print("onOtherSessionNpcInfo" .. timeValue.value)
    return false
end

function PlayerListener.onPlayerGetChristmasReward(userId, data)
    print("onPlayerGetChristmasReward  userId == "..tostring(userId))
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if InventoryUtil:isPlayerInvHasEnoughSlot(player, 2) then
        player:addItem(2802, 1, 0)
        player:addItem(2803, 1, 0)
        player.getRewardTick = os.time() + GameConfig.waitTime
        local time = player:checkCanGetRewardTime()
        local npc = json.decode(data)
        EngineWorld:getWorld():updateSessionNpcHeadInfoTime(player.rakssid, npc.entityId, npc.headInfo, 0, time)
        local day = DateUtil.getDate()
        GameAnalytics:design(player.userId, 0, { "g1048getSeniorBoxEveryDay", tostring(day)})
        GameAnalytics:design(player.userId, 0, { "g1048getCommonBoxEveryDay", tostring(day)})
    else
        HostApi.sendCommonTip(player.rakssid, "gui_str_app_shop_inventory_is_full")
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
