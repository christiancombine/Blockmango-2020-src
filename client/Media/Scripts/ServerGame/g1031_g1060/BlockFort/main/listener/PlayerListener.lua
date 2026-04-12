require "messages.Messages"
require "data.GamePlayer"
require "Match"
require "manager.SceneManager"
require "manager.GuideManager"

PlayerListener = {}

function PlayerListener:init()
    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerDynamicAttrEvent, self.onDynamicAttrInit)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerUseInitSpawnPositionEvent.registerCallBack(self.onPlayerUseInitSpawnPosition)
    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerCallOnTargetManorEvent.registerCallBack(self.onPlayerCallOnTargetManor)
    PlayerDropItemEvent.registerCallBack(self.onPlayerDropItem)
    PlayerOperationEvent.registerCallBack(self.onPlayerOperation)
    PlayerAttackDecorationEvent.registerCallBack(self.onPlayerAttackDecoration)
    SendGuideInfoEvent.registerCallBack(self.onSendGuideInfo)
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
    if not player then
        return
    end

    local manorInfo = SceneManager:getUserManorInfo(player.userId)
    if manorInfo ~= nil then
        player.manor = manorInfo
        player.manor:masterLogin()
        player.manor:setManorName(player.name)
        player.manorIndex = manorInfo.manorIndex
        player:setManorLevel(manorInfo.level)

        local schematicInfo = SceneManager:getUserSchematicInfo(player.userId)
        if schematicInfo ~= nil then
            player.schematic = schematicInfo
        end

        local decorationInfo = SceneManager:getUserDecorationInfo(player.userId)
        if decorationInfo ~= nil then
            player.decoration = decorationInfo
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
    DBManager:getPlayerData(player.userId, DbUtil.TAG_PLAYER) -- 如果有缓存，直接 initDataFromDB
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
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

            local schematicInfo = GameSchematic.new(manorIndex, targetUserId)
            SceneManager:initUserSchematicInfo(targetUserId, schematicInfo)

            local decorationInfo = GameDecoration.new(manorIndex, targetUserId)
            SceneManager:initUserDecorationInfo(targetUserId, decorationInfo)

            local keys = {
                DbUtil.TAG_MANOR,
                DbUtil.TAG_SCHEMATIC,
                DbUtil.TAG_DECORATION
            }

            DBManager:getPlayerManyData(targetUserId, keys, function(uid, result)
                for subKey, data in pairs(result) do
                    DbUtil:initDataFromDB(uid, data, subKey)
                end
            end)
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

    ManorConfig:enterManorArea(player, VectorUtil.newVector3(x, y, z))
    return true
end

function PlayerListener.onPlayerCallOnTargetManor(userId, targetId, from, isFriend, score)
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return true
    end

    local manorInfo = SceneManager:getUserManorInfo(targetId)
    if not manorInfo then
        EngineUtil.sendEnterOtherGame(player.rakssid, "g1052", targetId)
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

function PlayerListener.onPlayerDropItem(rakssid, itemid, itemMeta)
    return false
end

function PlayerListener.onPlayerOperation(userId, targetUserId, type, picUrl, isFriend, from)
    -- type: 5:invite - 6:ratify -  7:unratify - 8:remove out
    local player = PlayerManager:getPlayerByUserId(userId)
    if not player then
        return
    end

    if type == 5 then
        local targetPlayer = PlayerManager:getPlayerByUserId(targetUserId)
        local content = {
            userId = tostring(player.userId),
            picUrl = picUrl,
            nickName = player.name,
            showTime = GameConfig.inviteShowTime
        }

        local targetIds = {
            tostring(targetUserId)
        }

        if targetPlayer then
            HostApi.sendBroadcastMessage(targetPlayer.rakssid, 5, json.encode(content))
        else
            WebService:SendMsg(targetIds, json.encode(content), 5, "user", "g1052")
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

function PlayerListener.onPlayerAttackDecoration(rakssid, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
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
        return
    end

    if player.manorIndex == manorIndex then
        if not player.decoration then
            return
        end

        PacketSender:getSender():showBlockFortBuildingInfo(player.rakssid, id, entityId)
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

return PlayerListener
