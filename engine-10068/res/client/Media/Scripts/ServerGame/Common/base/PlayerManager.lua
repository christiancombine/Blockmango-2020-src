---
--- Created by Jimmy.
--- DateTime: 2018/10/9 0009 10:47
---

local RoomGameStatus = {
    RUNNING = 2,
    SERVER_QUITTING = 5,
    PREPARING = 7,
    STARTED = 8,
    STARTED_CAN_ENTER = 10,
}

local Players = {}
local Watchers = {}
local MaxPlayer = 8
local RoomMaxPlayer = 8
local Status = "normal"
local StatusChange = RoomGameStatus.PREPARING
local WatchMode = 0 ---观战模式，默认为0，1为只观战

PlayerManager = {}

--是否屏蔽消息
PlayerManager.IsMessages = true

local function updateCanJoin()
    local newStatus = "normal"
    if WatchMode == 1 then
        newStatus = "onlyWatch"
    elseif GameServer:isPrivateParty() then
        newStatus = "privateParty"
    elseif RoomMaxPlayer <= #Players then
        newStatus = "full"
    elseif StatusChange == RoomGameStatus.STARTED then
        newStatus = "gameStart"
    end
    if Status ~= newStatus then
        Status = newStatus
        for _, watcher in pairs(Watchers) do
            PacketSender:sendLuaCommonData(watcher.rakssid, "JoinGameStatus", Status)
        end
    end
end

function PlayerManager:setMaxPlayer(maxPlayer)
    MaxPlayer = maxPlayer
end

function PlayerManager:setRoomMaxPlayer(maxPlayer)
    RoomMaxPlayer = maxPlayer
end

function PlayerManager:getMaxPlayer()
    return MaxPlayer
end

function PlayerManager:isPlayerFull()
    return self:getPlayerCount() >= MaxPlayer
end

function PlayerManager:isPlayerEmpty()
    return self:getPlayerCount() <= 0
end

function PlayerManager:isPlayerEnough(players)
    return self:getPlayerCount() >= (players or 1)
end

function PlayerManager:isPlayerExists()
    return self:getPlayerCount() > 0
end

function PlayerManager:addPlayer(player)
    table.insert(Players, player)
    if self.IsMessages then
        MsgSender.sendMsg(IMessages:msgPlayerCount(self:getPlayerCount()))
    end
    HostApi.log(string.format("PlayerManager:addPlayer(PlayerCount=%d)", self:getPlayerCount()))
    updateCanJoin()
end

function PlayerManager:addWatcher(player)
    table.insert(Watchers, player)
end

function PlayerManager:subWatcher(player)
    for pos, c_player in pairs(Watchers) do
        if tostring(player.userId) == tostring(c_player.userId) then
            if BaseMain:getGameType() == "g1056" then
                ProcessManager:subWatcher(player)
            end
            table.remove(Watchers, pos)
            break
        end
    end
end

function PlayerManager:getWatcherByUserId(userId)
    for _, player in pairs(Watchers) do
        if tostring(player.userId) == tostring(userId) then
            return player
        end
    end
    return nil
end

function PlayerManager:getWatcherByRakssid(rakssid)
    for _, player in pairs(Watchers) do
        if player.rakssid == rakssid then
            return player
        end
    end
    return nil
end

function PlayerManager:subPlayer(player)
    RankManager:removePlayerRank(player.userId)
    UserExpManager:removeExpCache(player.userId)
    local sendMsg = false
    for pos, c_player in pairs(Players) do
        if tostring(player.userId) == tostring(c_player.userId) then
            table.remove(Players, pos)
            sendMsg = true
            break
        end
    end
    if sendMsg then
        if self.IsMessages then
            MsgSender.sendMsg(IMessages:msgPlayerQuit(player:getDisplayName()))
            MsgSender.sendMsg(IMessages:msgPlayerLeft(self:getPlayerCount()))
        end
        HostApi.log(string.format("PlayerManager:subPlayer(PlayerCount=%d)", self:getPlayerCount()))
    end
    ---找出跟随该玩家的观战者,并踢出游戏
    for _, watcher in pairs(Watchers) do
        if tostring(watcher.dynamicAttr.targetUserId) == tostring(player.userId) then
            HostApi.sendGameoverByPlatformUserId(watcher.userId, "user_follow_target_logout", GameOverCode.GameOver)
        end
    end
    updateCanJoin()
end

function PlayerManager:getPlayerCount()
    return #Players
end

function PlayerManager:getReadyPlayerCount()
    local count = 0
    for _, player in pairs(Players) do
        if player.isReady then
            count = count + 1
        end
    end
    return count
end

function PlayerManager:getPlayerByRakssid(rakssid)
    for _, player in pairs(Players) do
        if player.rakssid == rakssid then
            return player
        end
    end
    return nil
end

function PlayerManager:getPlayerByUserId(userId)
    for _, player in pairs(Players) do
        if tostring(player.userId) == tostring(userId) then
            return player
        end
    end
    return nil
end

function PlayerManager:getPlayerByEntityId(entityId)
    for _, player in pairs(Players) do
        if player:getEntityId() == entityId then
            return player
        end
    end
    return nil
end

function PlayerManager:getPlayerByPlayerMP(playerMP)
    if playerMP == nil then
        return nil
    end
    return self:getPlayerByRakssid(playerMP:getRaknetID())
end

function PlayerManager:getPlayers()
    return Players
end

function PlayerManager:copyPlayers()
    local players = {}
    for _, player in pairs(Players) do
        table.insert(players, player)
    end
    return players
end

function PlayerManager:onTick(ticks)
    for _, player in pairs(Players) do
        player:onTick(ticks)
    end
end

function PlayerManager:isAIPlayer(player)
    return tonumber(tostring(player.rakssid)) < 100
end

function PlayerManager:onGameStatusChange(status)
    StatusChange = status
    updateCanJoin()
end

function PlayerManager:getJoinGameStatus()
    return Status
end

function PlayerManager:setWatchMode(watchMode)
    WatchMode = watchMode
end

function PlayerManager:getRoomMaxPlayer()
    return RoomMaxPlayer
end

return PlayerManager
