require "messages.Messages"
require "data.GamePlayer"
require "Match"
require "listener.BirdSimulatorListener"
require "manager.ChestManager"

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
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerChangeItemInHandEvent.registerCallBack(self.onChangeItemInHandEvent)
    PlayerPickupItemEvent.registerCallBack(self.onPlayerPickupItem)
    PlayerOpenGlideEvent.registerCallBack(self.onPlayerOpenGlide)
    PlayerUseCannonEvent.registerCallBack(self.onPlayerUseCannon)
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()
    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onDynamicAttrInit(attr)
    if attr.manorId < 0 then
        return
    end
    local userId = attr.userId
    local nestIndex = attr.manorId
    print(
            "==== LuaLog: PlayerListener.onDynamicAttrInit : " ..
                    " userId = " .. tostring(userId) .. ", manorIndex = " .. nestIndex
    )

    if tostring(userId) ~= "0" and nestIndex > 0 then
        SceneManager:initUserNestIndex(userId, nestIndex)
    else
        print(
                "PlayerListener.onDynamicAttrInit: player[" .. tostring(userId) .. "]'s manorIndex = 0"
        )
    end
end

function PlayerListener.onPlayerReady(player)
    if player == nil then
        return
    end

    local nestIndex = SceneManager:getUserNestIndexByUid(player.userId)

    if nestIndex == 0 then
        print("PlayerListener.onPlayerReady:player[" .. tostring(player.userId) .. "]  nestIndex = 0")
        HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
        HostApi.sendReleaseManor(player.userId)
        return
    end

    local nestInfo = NestConfig:getNestInfoById(nestIndex)
    if nestInfo == nil then
        print("PlayerListener.onPlayerReady:player[" .. tostring(player.userId) .. "]  nestInfo = nil")
        HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
        HostApi.sendReleaseManor(player.userId)
        return
    end

    HostApi.sendPlaySound(player.rakssid, 10035)

    player.custom_wing = player:getClothesId("custom_wing")
    player:setSpeedAddition(player.moveSpeed)
    player:teleportPosWithYaw(nestInfo.pos, nestInfo.yaw)

    player.userNest = GameNest.new(player.userId, nestIndex)
    player.userBird = GameBird.new(player.userId)
    player.userBirdBag = GameBirdBag.new(player.userId)
    player.userMonster = GameMonster.new(player.userId)
    player.taskControl = PlayerTask.new(player)

    for i, v in pairs(FieldConfig.door) do
        EngineWorld:getWorld():updateSessionNpc(v.entityId, player.rakssid, "", v.actorName, v.bodyName, v.bodyId1, "", "", 0, true)
    end

    DbUtil:getPlayerData(player)

    HostApi.ZScore(RankConfig:getTotalCollectKey(), tostring(player.userId))
    HostApi.ZScore(RankConfig:getDailyCollectKey(), tostring(player.userId))
    HostApi.ZScore(RankConfig:getTotalFightKey(), tostring(player.userId))

    BirdSimulatorListener:OnPlayerReady(player)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gamename()))

    local players = PlayerManager:getPlayers()
    for i, p in pairs(players) do
        if p.userNest ~= nil then
            local index = p.userNest.nestIndex
            local playerNestInfo = NestConfig:getNestInfoById(index)
            if playerNestInfo ~= nil then
                local y = playerNestInfo.pos.y
                if p.userNest.nestIndex % 2 == 0 then
                    y = y + 5
                else
                    y = y + 7
                end
                local textPos = VectorUtil.newVector3(playerNestInfo.pos.x + 2.5, y, playerNestInfo.pos.z)
                HostApi.deleteWallText(0, textPos)
                HostApi.addWallText(0, 0, 0, p.name, textPos, 5, -playerNestInfo.yaw, 0, { 1, 1, 1, 1 })
            end
        end
    end

    -- get app rank score
    local userId = player.userId
    WebService:GetUserAppRank(player.userId, "all", 0, function(data)
        local m_player = PlayerManager:getPlayerByUserId(userId)
        if m_player then
            m_player:setTotalFruitNum(tonumber(data.integral))
            m_player:setBirdSimulatorPlayerInfo()
        end
    end)
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    local dier = PlayerManager:getPlayerByPlayerMP(diePlayer)
    if dier ~= nil then
        if dier.userNest ~= nil then
            local nestInfo = NestConfig:getNestInfoById(dier.userNest.nestIndex)
            dier:teleportPosWithYaw(nestInfo.pos, nestInfo.yaw)
        end

        if dier.userMonster ~= nil then
            dier.userMonster:setMonsterLostTarget()
        end

        RespawnHelper.sendRespawnCountdown(dier, 3)

        dier:onRespawn()
    end
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

    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)
    if player == nil then
        return true
    end

    NestConfig:enterNestArea(player)
    FieldConfig:enterDoor(player)
    FieldConfig:enterFieldArea(player)
    FieldConfig:enterVipArea(player)
    FieldConfig:enterGuideShopArea(player)
    return true
end

function PlayerListener.onPlayerDropItem(rakssid, itemId, itemMeta)
    return false
end

function PlayerListener.onChangeItemInHandEvent(rakssid, itemId, itemMeta)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    player.inHandItemId = itemId
end

function PlayerListener.onPlayerPickupItem(rakssid, itemId, itemNum, itemEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    local entity = EngineWorld:getEntity(itemEntityId)
    if entity ~= nil then
        entity:setDead()
    end

    local giftInfo = BirdConfig:getGiftInfoByCoinId(itemId)
    if giftInfo ~= nil then
        player:addBuff(giftInfo.id, itemEntityId)

        if tostring(giftInfo.id) == "0" then
            return
        end

        if tostring(giftInfo.type) == "3" then
            return
        end

        if tostring(giftInfo.type) == "4" then
            return
        end

        HostApi.sendPlaySound(rakssid, 316)

        local name = "addBuff:" .. tostring(player.userId) .. "=buffId:" .. tostring(giftInfo.id)
        local removeBuff = function(key)
            local str = StringUtil.split(key, "=")
            if str[1] == nil or str[2] == nil then
                return
            end

            local p_str = StringUtil.split(str[1], ":")
            if p_str[2] == nil then
                return
            end

            local b_str = StringUtil.split(str[2], ":")
            if b_str[2] == nil then
                return
            end

            local p_player = PlayerManager:getPlayerByUserId(p_str[2])
            if p_player ~= nil then
                p_player:removeBuff(b_str[2])
            end
        end

        TimerManager.RemoveListenerById(name)
        TimerManager.AddDelayListener(name, 10, removeBuff, nil)
    end

    return false
end

function PlayerListener.onPlayerOpenGlide(userId, isOpenGlide)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if player.wingId == nil then
        return
    end

    local wingInfo = WingsConfig:getWingInfo(player.wingId)
    if wingInfo == nil then
        return
    end

    if isOpenGlide then
        local item = StoreHouseItems:getItemInfo(player.wingId)
        if item == nil then
            return
        end

        player:setGlide(true)
        player:changeClothes("custom_wing", item.actorId)
    else
        player:changeClothes("custom_wing", player.custom_wing)
    end

end

function PlayerListener.onPlayerUseCannon(userId, entityId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return false
    end

    for _, v in pairs(CannonConfig.cannons) do
        if tonumber(v.entityId) == tonumber(entityId) then
            if player.userBirdBag.maxCarryBirdNum < tonumber(v.level) then
                MsgSender.sendCenterTipsToTarget(player.rakssid, 3, tonumber(v.tipId))
                return false
            else
                return true
            end
        end
    end

    return false

end

return PlayerListener
--endregion
