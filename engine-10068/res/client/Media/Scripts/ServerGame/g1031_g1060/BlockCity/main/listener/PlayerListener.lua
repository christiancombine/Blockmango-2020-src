require "messages.Messages"
require "data.GamePlayer"
require "Match"
require "manager.SceneManager"
require "manager.AreaInfoManager"
require "config.WingsConfig"
require "manager.GuideManager"
require "manager.ShopHouseManager"
require "manager.TradeManager"

PlayerListener = {}

function PlayerListener:init()

    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerDynamicAttrEvent, self.onDynamicAttrInit)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerUseInitSpawnPositionEvent.registerCallBack(self.onPlayerUseInitSpawnPosition)
    PlayerCallOnTargetManorEvent.registerCallBack(self.onPlayerCallOnTargetManor)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerUseCannonEvent.registerCallBack(self.onPlayerUseCannon)
    PlayerOpenGlideEvent.registerCallBack(self.onPlayerOpenGlide)
    CheckFlyingPermissionEvent.registerCallBack(self.onCheckFlyingPermission)
    PlayerAttackDecorationEvent.registerCallBack(self.onPlayerAttackDecoration)
    PlayerCheckinEvent.registerCallBack(self.onPlayerCheckin)
    PlayerOperationEvent.registerCallBack(self.onPlayerOperation)
    SendGuideInfoEvent.registerCallBack(self.onSendGuideInfo)
    PlayerTakeOnOrOffVehicleEvent.registerCallBack(self.onPlayerTakeOnOrOffVehicle)
    PlayerTakeOnDecorationEvent.registerCallBack(self.onPlayerTakeOnDecoration)
    PlayerSetCameraLockedEvent.registerCallBack(self.onPlayerSetCameraLocked)
    ElevatorArriveEvent.registerCallBack(self.onElevatorArrive)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
end

function PlayerListener.onPlayerUseInitSpawnPosition(userId, position)
    local targetUserId = SceneManager:getTargetByUid(userId)
    if targetUserId ~= nil then
        local targetManorInfo = SceneManager:getUserManorInfo(targetUserId)
        if targetManorInfo ~= nil and targetManorInfo.manorIndex ~= 0 then
            local pos  = ManorConfig:getInitPosByIndex(targetManorInfo.manorIndex, true)
            position.x = pos.x
            position.y = pos.y
            position.z = pos.z
            return false
        end
    end

    return true
end

function PlayerListener.onPlayerLogin(clientPeer)
    local player = GamePlayer.new(clientPeer)
    player:init()

    local targetUserId = SceneManager:getTargetByUid(player.userId)
    if targetUserId ~= nil then
        local targetManorInfo = SceneManager:getUserManorInfo(targetUserId)
        if targetManorInfo ~= nil and targetManorInfo.manorIndex ~= 0 then
            local vec3 = ManorConfig:getInitPosByIndex(targetManorInfo.manorIndex, false)
            player:setRespawnPos(vec3)
        end
    end

    return GameOverCode.Success, player, 1
end

function PlayerListener.onPlayerLogout(player)
    GameMatch:onPlayerQuit(player)
end

function PlayerListener.onPlayerReady(player)
    if player == nil then
        return
    end

    local manorInfo = SceneManager:getUserManorInfo(player.userId)
    if manorInfo ~= nil then
        player.manor = manorInfo
        player.manor:masterLogin()
        player.manor:setManorName(player.name)
        --player.manor:pushMailBoxAreaInfo()
        player.manorIndex = manorInfo.manorIndex
        player:setManorLevel(manorInfo.level)

        local blockInfo = SceneManager:getUserBlockInfo(player.userId)
        if blockInfo ~= nil then
            player.block = blockInfo
        end

        local decorationInfo = SceneManager:getUserDecorationInfo(player.userId)
        if decorationInfo ~= nil then
            player.decoration = decorationInfo
        end

        local archiveInfo = SceneManager:getUserArchiveInfo(player.userId)
        if archiveInfo ~= nil then
            player.archive = archiveInfo
        end

        print("userId = ", tostring(player.userId), " is master")
    end

    local manorIndex = 0
    local targetUserId = SceneManager:getTargetByUid(player.userId)
    if targetUserId ~= nil then
        local targetManorInfo = SceneManager:getUserManorInfo(targetUserId)
        if targetManorInfo ~= nil then
            manorIndex = targetManorInfo.manorIndex
        end
    end

    if manorIndex ~= 0 then
        local yaw = ManorConfig:getInitYawByIndex(manorIndex)
        local vec3 = ManorConfig:getInitPosByIndex(manorIndex, false)
        player:teleportPosWithYaw(vec3, yaw)
    else
        --player:teleInitPos()
        HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
        return
    end

    for i, v in pairs(SceneManager.userManorInfo) do
        v:addShowName(player.rakssid)
    end

    HostApi.sendPlaySound(player.rakssid, 10040)
    MsgSender.sendMsgToTarget(player.rakssid, IMessages:msgWelcomePlayer(Messages:gameName()))

    for _, decoration in pairs(SceneManager.userDecorationInfo) do
        decoration:pushDecorationAreaInfo()
    end

    player.custom_wing = player:getClothesId("custom_wing")
    AreaInfoManager:SyncAreaInfo(player.rakssid)
    DBManager:getPlayerData(player.userId, DbUtil.TAG_PLAYER) -- 如果有缓存，直接 initDataFromDB
    player:setManorArea()
    player:setElevatorArea()
    player:setWeekReward()
end

function PlayerListener.onDynamicAttrInit(attr)
    if attr.manorId < 0 then
        return
    end
    local userId = attr.userId
    local targetUserId = attr.targetUserId
    local manorIndex = attr.manorId
    print("PlayerListener.onDynamicAttrInit : userId =",
        tostring(userId), "&targetUserId = ", tostring(targetUserId), "&manorIndex = ", manorIndex)

    if tostring(targetUserId) ~= "0" and manorIndex > 0 then
        SceneManager:initCallOnTarget(userId, targetUserId)

        local userManorInfo = SceneManager:getUserManorInfo(targetUserId)
        if userManorInfo == nil then

            local manorInfo = GameManor.new(manorIndex, targetUserId)
            SceneManager:initUserManorInfo(targetUserId, manorInfo)

            local blockInfo = GameBlock.new(manorIndex, targetUserId)
            SceneManager:initUserBlockInfo(targetUserId, blockInfo)

            local decorationInfo = GameDecoration.new(manorIndex, targetUserId)
            SceneManager:initUserDecorationInfo(targetUserId, decorationInfo)

            local archiveInfo = GameArchive.new(manorIndex, targetUserId)
            SceneManager:initUserArchiveInfo(targetUserId, archiveInfo)

            DbUtil:getPlayerData(targetUserId)

            GameMatch:pushManorArea(manorIndex, targetUserId)
        else
            userManorInfo:refreshTime()
            print("PlayerListener.onDynamicAttrInit : use memory data userId = ",
                tostring(userId), "&targetUserId = ", tostring(targetUserId), "&&manorIndex = ", manorIndex)
        end

    else
        print("[error] PlayerListener.onDynamicAttrInit:  userId =",
            tostring(userId), "&targetUserId = ", tostring(targetUserId), "&manorIndex = ", manorIndex)
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


    ManorConfig:checkFlyArea(player, VectorUtil.newVector3(x, y, z))
    ManorConfig:enterManorArea(player, VectorUtil.newVector3(x, y, z))
    ManorConfig:enterDoorArea(player, VectorUtil.newVector3(x, y, z))
    player:checkShutDownEditMode()

    if player.isCameraLocked then
        player:setAllowFlying(true)
    end

    return true
end

function PlayerListener.onPlayerCallOnTargetManor(userId, targetId, from, isFriend, score)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return true
    end

    if from == 1 or from == 2 then
        -- 1 排行榜   2 好友
        player:visitAnalytics(from, score)
    end

    if from == 3 then
        -- 3 点击人物
        local targetPlayer = PlayerManager:getPlayerByUserId(targetId)
        if targetPlayer then
            player:visitAnalytics(from, targetPlayer:getThisScore())
        end
    end

    if from == 4 then
        -- 接受邀请
        player:acceptInvitationAnalytics(isFriend)
    end
    if from == 5 then
        -- 收花记录
        -- todo:
    end

    local manorInfo = SceneManager:getUserManorInfo(targetId)
    if not manorInfo then
        EngineUtil.sendEnterOtherGame(player.rakssid, "g1047", targetId)
        return true
    end

    local pos = ManorConfig:getInitPosByIndex(manorInfo.manorIndex, false)
    local yaw = ManorConfig:getInitYawByIndex(manorInfo.manorIndex)
    if pos then
        SceneManager:initCallOnTarget(userId, targetId)
        player:teleportPosWithYaw(pos, yaw)
    end

    return true

end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    if damageType == "player.gun" then
        return false
    end

    if not hurtFrom or not hurtPlayer then
        return false
    end

    local targetPlayer = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if targetPlayer then
        local rakssId = hurtFrom:getRaknetID()
        local targetId = targetPlayer.userId
        if rakssId and targetId then
            HostApi.showPlayerOperation(rakssId, targetId)
        end
    end

    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    return false
end

function PlayerListener.onPlayerDropItem(rakssid, itemid, itemMeta)
    return false
end

function PlayerListener.onPlayerUseCannon(userId, entityId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return true
    end

    local cannonId = CannonConfig:getCannonIdByEntityId(entityId)
    if cannonId ~= 0 then
        player:useCannonAnalytics(cannonId)
    end

    return true
end

function PlayerListener.onPlayerCheckin(rakssid, type)
    -- type : 0 -> send checkIn information to player
    --        1 -> checkIn
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if type == 1 then
        player:doCheckIn()
    end

    player:sendCheckInData()
end

function PlayerListener.onPlayerOpenGlide(userId, isOpenGlide)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player ~= nil and player.wingId ~= nil then
        local wingInfo = WingsConfig:getWingInfo(player.wingId)
        if wingInfo == nil then
            return
        end

        if isOpenGlide then
            player:setGlide(true)
            player:changeClothes("custom_wing", wingInfo.actorId)
        else
            player:changeClothes("custom_wing", player.custom_wing)
        end
    end
end

function PlayerListener.onCheckFlyingPermission(rakssid)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return false
    end

    if player.isCameraLocked then
        return true
    end

    if not player:getEditMode() then
        return false
    end

    if player:getIsCanFlying() then
        return true
    end

    player:setPlayerInfo()

    return false
end

function PlayerListener.onPlayerAttackDecoration(rakssid, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return false
    end

    if not player:isHeldItem() then
        return false
    end

    local manorIndex = 0
    local id = 0
    for _, v in pairs(SceneManager.userDecorationInfo) do
        if v.decoration[entityId] then
            manorIndex = v.manorIndex
            id = v.decoration[entityId].id
        end
    end

    if manorIndex == 0 or id == 0 then
        return false
    end

    if player.manorIndex == manorIndex then
        -- master
        if not player.decoration then
            return false
        end
        player.decoration:removeDecoration(entityId)
        player:addDecoration(id, 1)

        local decorationInfo = DecorationConfig:getDecorationInfo(id)
        if decorationInfo then
            player:addTempHotBar(decorationInfo.itemId, 0, player.hotBar.edit)
        end

        if decorationInfo and decorationInfo.isInteraction == 1 then
            GameMatch.isDecorationChange = true
        end
    else
        -- visitor
        return false
    end

    return true
end

function PlayerListener.onPlayerOperation(userId, targetUserId, type, picUrl, isFriend, from)
    -- type: 5:invite - 6:ratify -  7:unratify - 8:remove out - 9:race
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    type = tonumber(type)
    if type == 5 or type == 9 then
        local targetPlayer = PlayerManager:getPlayerByUserId(targetUserId)
        if not targetPlayer then
            return
        end

        local content = {
            userId = tostring(player.userId),
            picUrl = picUrl,
            nickName = player.name,
            showTime = GameConfig.inviteShowTime,
            type = type,
        }

        local targetIds = {
            tostring(targetUserId)
        }
        --todo:数据埋点未识别type==9
        player:invitationAnalytics(isFriend, from)
        if targetPlayer then
            if type == 9 then
                local drivers = {
                    [player] = {},
                    [targetPlayer] = {},
                }

                if player.race and player.race.competitorUId then
                    local competitor = PlayerManager:getPlayerByUserId(player.race.competitorUId)
                    if not competitor then
                        return
                    end

                    if player.race.inviterUId == userId then
                        VehicleConfig:sendRaceFailMsgPacket(player.rakssid, RaceFailType.iWaitAns, competitor.name)

                    else
                        VehicleConfig:sendRaceFailMsgPacket(player.rakssid, RaceFailType.iHaveNotAns, competitor.name)
                    end
                    return
                end

                if targetPlayer.race and targetPlayer.race.competitorUId then
                    VehicleConfig:sendRaceFailMsgPacket(player.rakssid, RaceFailType.targetWaitAns)
                    return
                end

                local tab = VehicleConfig:checkFreeRaceLines()
                if not tab then --没有空闲的跑道
                    VehicleConfig:sendRaceFailMsgPacket(player.rakssid, RaceFailType.notFreeLines)
                    return
                end

                for driver, _ in pairs(drivers) do
                    local competitorUId = (driver == player and {targetUserId} or {userId})[1]
                    local competitorName = targetPlayer.name
                    driver:setRaceData(false, competitorUId, competitorName, nil, nil, nil, os.time(), userId)
                end
            end
            HostApi.sendBroadcastMessage(targetPlayer.rakssid, type, json.encode(content))
        else
            WebService:SendMsg(targetIds, json.encode(content), type, "user", "g1047")
        end
        return
    end

    if type == 8 then
        local visitor = PlayerManager:getPlayerByUserId(targetUserId)
        if not visitor then
            return
        end
        if player.curManorIndex == player.manorIndex
                and visitor.curManorIndex == player.manorIndex then
            local vec3 = ManorConfig:getInitPosByIndex(player.manorIndex, false)
            if vec3 ~= nil then
                local yaw = ManorConfig:getInitYawByIndex(player.manorIndex)
                visitor:teleportPosWithYaw(vec3, yaw)
            end
        end
        return
    end
end

function PlayerListener.onSendGuideInfo(rakssid, guideId, guideStatus)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    local guide = { Id = guideId, Status = guideStatus }
    GuideManager:updateStatus(player, guide)
end

function PlayerListener.onPlayerTakeOnOrOffVehicle(rakssid, entityId, onOrOff)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end

    if onOrOff then
        local type = VehicleConfig:getVehicleTypeByEntityId(entityId)
        if type ~= 0 then
            player:driveCarAnalytics(type)
        end
    else
        if player.race and player.race.isRacing then
            player.race.isTakeOffVehicle = not onOrOff
        end
    end
end

function PlayerListener.onPlayerTakeOnDecoration(userId, entityId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    local itemId = PublicFacilitiesConfig:getPublicChairByEntityId(entityId)
    if itemId ~= 0 then
        player:userDecorationAnalytics(itemId)
    else
        for _, v in pairs(SceneManager.userDecorationInfo) do
            if v.decoration[entityId] then
                player:userDecorationAnalytics(v.decoration[entityId].id)
                break
            end
        end
    end
end

function PlayerListener.onPlayerSetCameraLocked(rakssid, isLocked)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end

    local isCanFlying = isLocked
    if not isLocked then
        isCanFlying = player:getIsCanFlying() and player:getEditMode()
    end
    
    player:setAllowFlying(isCanFlying)

    if not isLocked and player.isCameraLocked then
        player:setFlying(false)
        player:changeClothes("custom_wing", player.custom_wing)
        player:setEditMode(false)

        if not player.race or not player.race.isRacing then
            player:teleportPosWithYaw(player:getPosition(), player:getYaw())
        end
    end

    if isLocked and not player.isCameraLocked then
        player:setGlide(false)
        player:setFlying(true)
    end

    player.isCameraLocked = isLocked
end

function PlayerListener.onElevatorArrive(rakssid, floorId, elevatorId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end

    local endPos, yaw = ElevatorConfig:getEndPosByFloorId(floorId, elevatorId)
    if not endPos then
        return
    end

    player:teleportPosWithYaw(endPos, yaw)
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

return PlayerListener
--endregion
