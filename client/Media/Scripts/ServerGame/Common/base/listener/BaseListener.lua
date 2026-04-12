---
--- Created by Jimmy.
--- DateTime: 2018/9/13 0028 11:37
---
BaseListener = {}
BaseListener.callbacks = {}

local function IsWatch(userId)
    local manager = GameServer:getRoomManager()
    if not manager:isUserAttrExisted(userId) then
        return false
    end
    local attr = manager:getUserAttrInfo(userId)
    if attr.followEnterType == 2 then
        return true
    end
    return false
end

local function setFollowModeAndTeleport(player, follower)
    LuaTimer:schedule(function()
        player:setAllowFlying(true)
        player:setFlying(true)
    end, 10)
    PacketSender:sendIntoFollowMode(player.rakssid, tostring(follower.userId))
    PacketSender:sendLuaCommonData(player.rakssid, "JoinGameStatus", tostring(PlayerManager:getJoinGameStatus()))
end

local function intoFollowMode(player, targetUserId)
    local _player = PlayerManager:getPlayerByUserId(targetUserId)
    if _player then
        player:addNightVisionPotionEffect(43200)
        player:teleportPos(_player:getPosition())
        PlayerIntoFollowModeEvent:invoke(player, _player)
        setFollowModeAndTeleport(player, _player)
    end
end

function BaseListener.registerCallBack(event, func)
    BaseListener.callbacks[event] = func
end

function BaseListener:init()
    GameInitEvent.registerCallBack(self.onGameInit)
    PlayerLoginEvent.registerCallBack(self.onPlayerLogin)
    PlayerLogoutEvent.registerCallBack(self.onPlayerLogout)
    PlayerReadyEvent.registerCallBack(self.onPlayerReady)
    PlayerAttackedEvent.registerCallBack(self.onPlayerHurt)
    PlayerFirstSpawnEvent.registerCallBack(self.onPlayerFirstSpawn)
    PlayerRespawnEvent.registerCallBack(self.onPlayerRespawn)
    PlayerDieEvent.registerCallBack(self.onPlayerDied)
    PlayerDieTipEvent.registerCallBack(self.onPlayerDiedTip)
    PlayerAfterRespawnEvent.registerCallBack(self.onPlayerAfterRespawn)
    GetDataFromDBEvent:registerCallBack(self.onGetDataFromDB)
    ClientCommonDataEvent.registerCallBack(self.onClientCommonData)
    SpawnEntityEvent.registerCallBack(self.onSpawnEntity)
    RemoveEntityEvent.registerCallBack(self.onRemoveEntity)
    PlayerReconnectSuccessEvent.registerCallBack(self.onPlayerReconnectSuccess)
    SavePlayerDBDataEvent.registerCallBack(self.onSavePlayerDBData)
    PlayerDynamicAttrEvent.registerCallBack(self.onPlayerDynamicAttr)
    GameStatusChangeEvent.registerCallBack(self.onGameStatusChange)
end

function BaseListener.onGameInit(GamePath, ver, serverworld, config)
    GameType = config.gameType
    HostApi.log(VarDump(ServerWorld))
    BaseMain:setIsChina(config.isChina)
    FileUtil:setMapPath(GamePath)
    EngineWorld:setWorld(serverworld)
    GameServer:setServerConfig(config)
    PacketSender:init()
    AppProps:init()
    WebService:init(ver, config)
    CommonTask:start()
    PlayerManager:setMaxPlayer(config.maxPlayers)
    PlayerManager:setRoomMaxPlayer(config.maxPlayers)
    PlayerManager:setWatchMode(config.watchMode)
    require "base.helper.GMHelper"

    local callback = BaseListener.callbacks[GameInitEvent]
    if callback ~= nil then
        callback(FileUtil.getConfigFromYml("config"))
    end

    WebService:GetExpRule()
    PlayerIdentityCache:init()
    GameAnalyticsRequest.asyncInit(3)
    SumRechargeConfig:init()
    MaxRecordHelper:init()
    ExchangeHelper:init()
    require "base.config.CommonActivityChestConfig"
    require "base.config.CommonActivityConfig"
    require "base.config.CommonActivityRewardConfig"
    require "base.config.MustLotteryPriceConfig"
    require "base.config.LuckyLotteryPrice"
    require "base.config.LuckyValueConfig"

    if Platform.isWindow() then
        local path = string.gsub(FileUtil:getScriptDir(), "ServerGame", "BaseGame")
        HU.Init("base.hotupdate.hotupdatelist", { ".\\Media\\Scripts\\ServerGame\\Common", FileUtil:getScriptDir(), path })
    end
end

function BaseListener.onPlayerLogin(clientPeer, clientInfo, enableIndie, gameTimeToday)
    ---玩家进入游戏前发送服务器配置
    local builder = DataBuilder.new()
    builder:addParam("isChina", BaseMain:isChina())
    PacketSender:sendLuaCommonData(clientPeer:getRakssid(), "ServerInfo", builder:getData())

    if IsWatch(clientPeer:getPlatformUserId()) then
        ---好友跟随进入观战模式
        local player = BasePlayer.new(clientPeer)
        PlayerManager:addWatcher(player)
        PacketSender:sendPlayerDBReady(player.rakssid)
        player:subHealth(player:getHealth() + 1)
        player:sendLoginResult(true, player.dynamicAttr.team, "",
                PlayerManager:getPlayerCount(), PlayerManager:getMaxPlayer())
        return false
    end

    local player = PlayerManager:getPlayerByUserId(clientPeer:getPlatformUserId())
    if player then
        ---顶号登录，直接使用现有玩家，不重置玩家数据
        local entityPlayerMP = clientPeer:getEntityPlayer()
        local oldPosition = player.entityPlayerMP:getPosition()
        local oldYaw = player.entityPlayerMP:getYaw()
        entityPlayerMP:clonePlayer(player.entityPlayerMP, true)
        player:initStaticAttr(player.entityPlayerMP:getTeamId())
        player.clientPeer = clientPeer
        player.entityPlayerMP = entityPlayerMP
        player.rakssid = clientPeer:getRakssid()
        player.isKickOut = false
        LuaTimer:schedule(function()
            player:teleportPosWithYaw(oldPosition, oldYaw)
        end, 100)
        ExchangeHelper:onPlayerLogin(player)
        if player:checkReady() then
            ---顶号登录需要保存一次数据，否则会拉取到旧数据
            DBManager:callSavePlayerData(player)
        end
        return DBManager:onPlayerLogin(player)
    end
    ---------------正常登录流程---------------
    if PlayerManager:isPlayerFull() then
        HostApi.sendGameover(clientPeer:getRakssid(), IMessages:msgGamePlayerIsEnough(), GameOverCode.PlayerIsEnough)
        return true
    end
    GameAnalyticsCache:addClientInfo(clientPeer:getPlatformUserId(), clientInfo)
    local code, status, c_callback
    local callback = BaseListener.callbacks[PlayerLoginEvent]
    if callback == nil then
        code = GameOverCode.Success
        player = BasePlayer.new(clientPeer)
        player:initStaticAttr(player.dynamicAttr.team)
    else
        code, player, status, c_callback = callback(clientPeer)
        if code == GameOverCode.GameStarted then
            HostApi.sendGameover(clientPeer:getRakssid(), IMessages:msgGameHasStarting(), GameOverCode.GameStarted)
            return true
        end
        if code ~= GameOverCode.Success then
            HostApi.sendGameover(clientPeer:getRakssid(), IMessages:msgGameOver(), GameOverCode.GameOver)
            return true
        end
        if player == nil then
            code = GameOverCode.Success
            player = BasePlayer.new(clientPeer)
            player:initStaticAttr(player.dynamicAttr.team)
        end
    end
    PlayerManager:addPlayer(player)
    if BaseMain:isChina() and Platform.isLinux() then
        player.gameTimeToday = gameTimeToday or 0
    else
        player.gameTimeToday = -1
    end
    player.enableIndie = enableIndie or false
    if PlayerManager:isPlayerFull() then
        HostApi.sendStartGame(PlayerManager:getPlayerCount())
    end
    if status ~= nil and status >= 1 and status <= 2 then
        HostApi.sendGameStatus(clientPeer:getRakssid(), status)
    end
    if c_callback ~= nil then
        c_callback()
    end
    ExchangeHelper:onPlayerLogin(player)
    return DBManager:onPlayerLogin(player)
end

function BaseListener.onPlayerLogout(rakssid)
    local watcher = PlayerManager:getWatcherByRakssid(rakssid)
    if watcher then
        PlayerManager:subWatcher(watcher)
        return true
    end

    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return false
    end
    GameGroupManager:removeUser(player.userId)
    SumRechargeHelper:onPlayerLogout(player.userId)
    ExchangeHelper:onPlayerLogout(player.userId)
    CommonActivity:onPlayerLogout(player)
    if player ~= nil then
        PlayerManager:subPlayer(player)
        local clear = function(player)
            ---玩家正常退出游戏后，保存一次数据
            if not player.isKickOut then
                DBManager:callSavePlayerData(player)
            end
            player:onPlayerLogout()
            if DBManager:isAutoCache() then
                DBManager:removeCache(player.userId)
            end
            RewardManager:removeRewardCache(player.userId)
            GameAnalyticsCache:removeCache(player.userId)
            MaxRecordHelper:onPlayerLogout(player.userId)
        end
        local callback = BaseListener.callbacks[PlayerLogoutEvent]
        if callback == nil then
            clear(player)
            return
        end
        callback(player)
        clear(player)
    else
        RewardManager:removeRewardCache(player.userId)
        GameAnalyticsCache:removeCache(player.userId)
        MaxRecordHelper:onPlayerLogout(player.userId)
    end
    SumRechargeHelper:removeCache(player)
    return true
end

function BaseListener.onPlayerReady(rakssid)
    local player = PlayerManager:getWatcherByRakssid(rakssid)
    if player then
        PacketSender:sendDataReport(rakssid, "follow_watch", GameType)
        intoFollowMode(player, player.dynamicAttr.targetUserId)
        return
    end

    local callback = BaseListener.callbacks[PlayerReadyEvent]
    if callback == nil then
        return
    end
    player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        HostApi.sendGameover(rakssid, IMessages:msgGameOver(), GameOverCode.ReadyNoPlayer)
        return
    end
    player.isReady = true
    local time = callback(player)
    DBManager:onPlayerReady(player)
    SumRechargeHelper:onPlayerReady(player)
    ExchangeHelper:onPlayerReady(player)
    if time ~= nil and time > 0 then
        player:addNightVisionPotionEffect(time)
    end
    if player.dynamicAttr.followEnterType == 1 then
        PacketSender:sendDataReport(rakssid, "follow_game", GameType)
    end
    CommonActivity:onPlayerReady(player)
end

function BaseListener.onPlayerFirstSpawn(clientPeer)
    local player = PlayerManager:getPlayerByRakssid(clientPeer:getRakssid())
    if player == nil then
        return
    end
    PlayerIdentityCache:onPlayerSpawn(player)
    local callback = BaseListener.callbacks[PlayerFirstSpawnEvent]
    if callback == nil then
        return
    end
    callback(player)
end

function BaseListener.onPlayerAfterRespawn(clientPeer)
    local player = PlayerManager:getPlayerByRakssid(clientPeer:getRakssid())
    if player == nil then
        return
    end
    local callback = BaseListener.callbacks[PlayerAfterRespawnEvent]
    if callback == nil then
        return
    end
    callback(player)
end

function BaseListener.onPlayerRespawn(clientPeer)
    local player = PlayerManager:getPlayerByRakssid(clientPeer:getRakssid())
    if player == nil then
        return
    end
    PlayerIdentityCache:onPlayerSpawn(player)
    local callback = BaseListener.callbacks[PlayerRespawnEvent]
    if callback == nil then
        return
    end
    player:respawn(clientPeer)
    local time = callback(player)
    if time ~= nil and time > 0 then
        player:addNightVisionPotionEffect(time)
    end
end

function BaseListener.onGetDataFromDB(userId, subKey, data)
    local callback = BaseListener.callbacks[GetDataFromDBEvent]
    if callback == nil then
        return
    end
    local player = PlayerManager:getPlayerByUserId(userId)
    if player ~= nil then
        if DBManager:isAutoCache() then
            DBManager:addCache(userId, subKey, data)
        end
        callback(player, subKey, data)
    end
end

function BaseListener.onClientCommonData(userId, key, data)
    CommonDataEvents:dispatchEvent(userId, key, data)
end

function BaseListener.onSpawnEntity(entity, type)
    if not EngineWorld:getWorld() then
        return
    end
    local lEntity = EntityCache:createEntityCache(entity, type)
    local callback = BaseListener.callbacks[SpawnEntityEvent]
    if callback then
        callback(lEntity)
    end
end

function BaseListener.onRemoveEntity(entityId)
    EntityCache:removeEntityCache(entityId)
end

function BaseListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    ---攻击者如果是 观战者拦截攻击
    if hurtFrom and PlayerManager:getWatcherByUserId(hurtFrom:getPlatformUserId()) then
        return false
    end

    local callback = BaseListener.callbacks[PlayerAttackedEvent]
    if callback ~= nil then
        return callback(hurtPlayer, hurtFrom, damageType, hurtValue)
    end
    return true
end

function BaseListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    ---死亡者如果是 观战者必须死
    if diePlayer and PlayerManager:getWatcherByUserId(diePlayer:getPlatformUserId()) then
        return true
    end

    local callback = BaseListener.callbacks[PlayerDieEvent]
    if callback ~= nil then
        return callback(diePlayer, iskillByPlayer, killPlayer)
    end
    return true
end

function BaseListener.onPlayerDiedTip(diePlayer, killPlayer)
    ---死亡者如果是 观战者 不提示
    if diePlayer and PlayerManager:getWatcherByUserId(diePlayer:getPlatformUserId()) then
        return false
    end

    local callback = BaseListener.callbacks[PlayerDieTipEvent]
    if callback ~= nil then
        return callback(diePlayer, killPlayer)
    end
    return true
end

function BaseListener.onPlayerReconnectSuccess(clientPeer)

end

function BaseListener.onSavePlayerDBData(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player ~= nil then
        ---玩家被踢退出游戏，保存一次数据
        player.isKickOut = true
        DBManager:callSavePlayerData(player)
    end
end

function BaseListener.onPlayerDynamicAttr(attr)
    if not Platform.isWindow() then
        if Server.Instance():getEnableRoom() then
            LuaTimer:schedule(function(userId, requestId)
                local player = PlayerManager:getPlayerByUserId(userId)
                local watcher = PlayerManager:getWatcherByUserId(userId)
                if not player and not watcher then
                    GameServer:getRoomManager():sendUserOut(userId, requestId)
                end
            end, 90000, nil, attr.userId, attr.requestId)
        end
    end
    if attr.followEnterType == 2 then
        return
    end
    local callback = BaseListener.callbacks[PlayerDynamicAttrEvent]
    if callback then
        callback(attr)
    end
end

function BaseListener.onGameStatusChange(status)
    PlayerManager:onGameStatusChange(status)
end

return BaseListener