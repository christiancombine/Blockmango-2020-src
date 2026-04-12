
require "data.GamePlayer"
require "data.GameFuncNpc"
require "Match"
require "data.GameJobPond"

PlayerListener = {}

function PlayerListener:init()

    BaseListener.registerCallBack(PlayerLoginEvent, self.onPlayerLogin)
    BaseListener.registerCallBack(PlayerLogoutEvent, self.onPlayerLogout)
    BaseListener.registerCallBack(PlayerReadyEvent, self.onPlayerReady)
    BaseListener.registerCallBack(PlayerAttackedEvent, self.onPlayerHurt)
    BaseListener.registerCallBack(PlayerDieEvent, self.onPlayerDied)

    PlayerMoveEvent.registerCallBack(self.onPlayerMove)
    PlayerCurrencyChangeEvent.registerCallBack(self.onPlayerCurrencyChange)
    PlayerDropItemEvent.registerCallBack(function()
        return false
    end)
    PlayerAttackSessionNpcEvent.registerCallBack(self.onPlayerAttackSessionNpc)
    PlayerStartGameEvent:registerCallBack(self.onPlayerStartGame)
    WatchAdvertFinishedEvent.registerCallBack(self.onWatchAdvertFinished)

    PlayerSelectJobEvent:registerCallBack(self.onSelectJob)
    PlayerRefreshJobListInfoEvent:registerCallBack(self.RefreshJobListInfo)
    PlayerExtractJobEvent:registerCallBack(self.onPlayerExtractJob)  --玩家抽取职业的事件
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
    DbUtil:getPlayerData(player)
    HostApi.sendPlaySound(player.rakssid, 10039)
    return 43200
end

function PlayerListener.onPlayerHurt(hurtPlayer, hurtFrom, damageType, hurtValue)
    local hurter = PlayerManager:getPlayerByPlayerMP(hurtPlayer)
    if damageType == "outOfWorld" then
        hurter:teleInitPos()
    end
    return false
end

function PlayerListener.onPlayerDied(diePlayer, iskillByPlayer, killPlayer)
    return true
end

function PlayerListener.onPlayerMove(movePlayer, x, y, z)
    if x == 0 and y == 0 and z == 0 then
        return true
    end
    local player = PlayerManager:getPlayerByPlayerMP(movePlayer)
    if player == nil then
        return true
    end
    FuncNpcConfig:onPlayerMove(player, x, y, z)
    player:onMove(x, y, z)
    return true
end

function PlayerListener.onPlayerCurrencyChange(rakssid, currency)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        player:onMoneyChange()
    end
end

function PlayerListener.onPlayerStartGame(userId)
    print("PlayerListener.onPlayerStartGame")
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    EngineUtil.sendEnterOtherGame(player.rakssid, "g1054", player.userId, player.cur_map_id)
end

function PlayerListener.onPlayerAttackSessionNpc(rakssid, npcEntityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    local npc = FuncNpcConfig.FuncNpcMap[tostring(npcEntityId)]
    npc:onPlayerTriggered(player)
    HostApi.log("onPlayerAttackSessionNpc player.rakssid is :" .. tostring(rakssid) .. " the npcEntityId is : " .. npcEntityId)
end

function PlayerListener.onWatchAdvertFinished(rakssid, type, params, code)
    LogUtil.log("PlayerListener.onWatchAdvertFinished  " .. code)
    if code ~= 1 then
        return
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end

    if type == Define.AdType.AD_CARD then
        AdvertConfig:doCardAdReward(player)
        return
    end
    if type == Define.AdType.AD_PROP then
        AdvertConfig:doPropAdReward(player)
        return
    end
end

--当玩家选择职业的
function PlayerListener.onSelectJob(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local jobId = DataBuilder.new():fromData(data):getNumberParam("JobId")
    player:setCurJobId(jobId)
    --GameMatch:setName()
end
--当玩家刷新职业
function PlayerListener.RefreshJobListInfo(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local price = DataBuilder.new():fromData(data):getNumberParam("price")
    PayHelper.payDiamonds(userId, 100, price, function(player)
        player.Jobs_Pond = {}
        GameJobPond:randomAllJobs(player)
    end, "")
end

--玩家抽取职业的事件
function PlayerListener.onPlayerExtractJob(userId, data)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    --价格
    local price = DataBuilder.new():fromData(data):getNumberParam("price")
    PayHelper.payDiamonds(userId, 100, price, function(player)
        player.choiceJob = player.choiceJob + 1
        GameJobPond:onPlayerExtractJob(player)
    end, "")
end

return PlayerListener
--endregion
