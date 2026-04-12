PlayerListener = {}

function PlayerListener:init()

    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    SendGuideInfoEvent.registerCallBack(self.onSendGuideInfo)
    PlayerDropItemEvent.registerCallBack(function()
        return false
    end)
    AddExpEvent.registerCallBack(self.onAddExp)
    AddRuneEvent.registerCallBack(self.onAddRune)

    PlayerOpenRuneEvent:registerCallBack(self.onPlayerOpenRune)
    PlayerFirstShowStoreDressEvent:registerCallBack(self.onPlayerFirstShowStoreDress)
    PlayerBuyStoreDressEvent:registerCallBack(self.onPlayerBuyStoreDress)
    PlayerUseStoreDressEvent:registerCallBack(self.onPlayerUseStoreDress)
    PlayerUnloadStoreDressEvent:registerCallBack(self.onPlayerUnloadStoreDress)
    PlayerChangeRuneEvent:registerCallBack(self.onPlayerChangeRune)
    PlayerOneKeyUnloadRuneEvent:registerCallBack(self.onPlayerOneKeyUnloadRune)
    PlayerOneKeyEquipRuneEvent:registerCallBack(self.onPlayerOneKeyEquipRune)
    PlayerOneKeyReclaimRuneEvent:registerCallBack(self.onPlayerOneKeyReclaimRune)
    PlayerReclaimRuneEvent:registerCallBack(self.onPlayerReclaimRune)
    PlayerUnlockRuneEvent:registerCallBack(self.onPlayerUnlockRune)
    PlayerStartGameEvent:registerCallBack(self.onPlayerStartGame)
    PlayerLicenseReceiveRewardEvent:registerCallBack(self.onPlayerReceiveReward)
    PlayerLicenseBuyLevelEvent:registerCallBack(self.onPlayerBuyLevel)
    PlayerBuyLicenseEvent:registerCallBack(self.onPlayerBuyLicense)
    PlayerExchangeRewardEvent:registerCallBack(self.onPlayerExchangeReward)
    PlayerReceiveTaskReward:registerCallBack(self.onPlayerReceiveTaskReward)
    PlayerUseActionEvent:registerCallBack(self.onPlayerUseAction)
    PlayerSendChatEvent:registerCallBack(self.onPlayerSendChat)
end

function PlayerListener.onAddExp(rakssid, exp)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    player:addExp(exp)
end

function PlayerListener.onAddRune(rakssid, runeId, runeNum)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    player:addRune(runeId, runeNum)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    ReportUtil.reportNewPlayerHadOpenLicense(player)
    GameMatch:onPlayerQuit(player)
    GameInfoManager:clearInfo("PlayerInfo", player.userId)
end

function PlayerListener.onPlayerReady(player)
    DbUtil:getPlayerData(player)
    HostApi.sendPlaySound(player.rakssid, 10039)
    HostApi.setDisarmament(player.rakssid, true)
    GameInfoManager.Instance():onPlayerReady(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))
    CommonPacketSender:sendBedWarPlayerReady(player.rakssid)
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    return false
end

function PlayerListener.onPlayerOpenRune(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    player:sendOpenRune()
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end
    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)
    if player == nil then
        return true
    end
    if y <= GameConfig.deathLineY then
        HostApi.resetPos(player.rakssid, GameConfig.initPos.x, GameConfig.initPos.y, GameConfig.initPos.z)
    end
    player:onMove(x, y, z)
    return true
end

function PlayerListener.onSendGuideInfo(rakssid, guideId, guideStatus)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    local guide = { Id = guideId, Status = guideStatus }
    GameGuide:updateStatus(player, guide)
end

function PlayerListener.onPlayerFirstShowStoreDress(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if DbUtil:IsGetAllDataFinished(player) then
        local dressId = DataBuilder.new():fromData(data):getNumberParam("dressId")
        GameDressStore:onFirstShowStoreDress(player, dressId)
    end
end

function PlayerListener.onPlayerBuyStoreDress(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if DbUtil:IsGetAllDataFinished(player) then
        local dressId = DataBuilder.new():fromData(data):getNumberParam("dressId")
        GameDressStore:onBuyStoreDress(player, dressId)
    end
end

function PlayerListener.onPlayerUseStoreDress(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if DbUtil:IsGetAllDataFinished(player) then
        local dressId = DataBuilder.new():fromData(data):getNumberParam("dressId")
        GameDressStore:onUseStoreDress(player, dressId, false)
    end
end

function PlayerListener.onPlayerUnloadStoreDress(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if DbUtil:IsGetAllDataFinished(player) then
        local dressId = DataBuilder.new():fromData(data):getNumberParam("dressId")
        GameDressStore:onUnloadStoreDress(player, dressId)
    end
end

function PlayerListener.onPlayerChangeRune(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local builder = DataBuilder.new():fromData(data)
    local troughId = builder:getNumberParam("troughId")
    local curRuneId = builder:getNumberParam("curRuneId")
    local targetRuneId = builder:getNumberParam("targetRuneId")
    player:changeRune(troughId, curRuneId, targetRuneId)
end

function PlayerListener.onPlayerOneKeyUnloadRune(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local troughType = DataBuilder.new():fromData(data):getNumberParam("troughType")
    player:oneKeyUnload(troughType)
end

function PlayerListener.onPlayerOneKeyEquipRune(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local troughType = DataBuilder.new():fromData(data):getNumberParam("troughType")
    player:oneKeyAssemble(troughType)
end

function PlayerListener.onPlayerOneKeyReclaimRune(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local runeType = DataBuilder.new():fromData(data):getNumberParam("runeType")
    player:reclaimAll(runeType)
end

function PlayerListener.onPlayerReclaimRune(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local runeId = DataBuilder.new():fromData(data):getNumberParam("runeId")
    player:reclaim(runeId)
end

function PlayerListener.onPlayerUnlockRune(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    if player.yaoshi < GameConfig.unlockRuneTroughPrice then
        HostApi.sendCustomTip(player.rakssid, "lack_of_key", Define.LackOfKey)
        return
    end
    local runeType = DataBuilder.new():fromData(data):getNumberParam("runeType")
    player:subYaoShi(GameConfig.unlockRuneTroughPrice, "key_UnlockRune", runeType)
    player:runeUnlock(runeType)
end

function PlayerListener.onPlayerStartGame(userId, gameType)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    ReportUtil.reportStartGame(player)
end

function PlayerListener.onPlayerReceiveReward(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local Lv = DataBuilder.new():fromData(data):getNumberParam("Lv")
    local rewardId = DataBuilder.new():fromData(data):getNumberParam("rewardId")
    local rewardType = DataBuilder.new():fromData(data):getParam("rewardType")
    local getAll = DataBuilder.new():fromData(data):getBoolParam("getAll")
    local season = DataBuilder.new():fromData(data):getNumberParam("season")
    GameLicense:onPlayerGetReward(userId, Lv, getAll, season, rewardId, rewardType)
end

function PlayerListener.onPlayerBuyLevel(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local season = DataBuilder.new():fromData(data):getNumberParam("season")
    local level = DataBuilder.new():fromData(data):getNumberParam("level")
    local free = DataBuilder.new():fromData(data):getNumberParam("free")
    GameLicense:onPlayerBuyLevel(player, season, level, free)
end

function PlayerListener.onPlayerBuyLicense(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local season = DataBuilder.new():fromData(data):getNumberParam("season")
    local buyType = DataBuilder.new():fromData(data):getNumberParam("buyType")
    GameLicense:onPlayerBuy(player, season, buyType)
end

function PlayerListener.onPlayerExchangeReward(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local season = DataBuilder.new():fromData(data):getNumberParam("season")
    local itemId = DataBuilder.new():fromData(data):getNumberParam("itemId")
    local rewardNum = DataBuilder.new():fromData(data):getNumberParam("rewardNum")
    GameLicense:onPlayerExchange(userId, itemId, season, rewardNum)
end

function PlayerListener.onPlayerUseAction(userId, actionId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    CommonPacketSender:sendUseActionToAll(player:getEntityId(), actionId)
end

function PlayerListener.onPlayerReceiveTaskReward(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local taskId = DataBuilder.new():fromData(data):getNumberParam("taskId")
    GameLicense:onPlayerReceiveSeasonTaskReward(player, taskId)
end

function PlayerListener.onPlayerSendChat(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end
    local text = DataBuilder.new():fromData(data):getNumberParam("text")
    GamePacketSender:sendChatToAll(userId, text)
end

return PlayerListener
--endregion
