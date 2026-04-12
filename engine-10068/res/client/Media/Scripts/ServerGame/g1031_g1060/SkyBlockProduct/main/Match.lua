--Match.lua
require "util.DbUtil"
require "util.BackHallManager"
require "util.RechargeUtil"

GameMatch = {}

GameMatch.gameType = "g1050"
GameMatch.curTick = 0
GameMatch.userManorInfo = {}
GameMatch.callOnTarget = {}
GameMatch.errorTarget = {}

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    -- self:updateCallOnTargetTime()
    self:updateErrorTargetTime()
    -- self:updateManorTime()
    self:addPlayerScore()
    self:kickOutErrorPlayer()
    BackHallManager:onTick()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onTick()
    end
end

function GameMatch:onPlayerQuit(player)
    local manor = GameMatch.userManorInfo[tostring(player.userId)]
    player:reward()
    if manor ~= nil then
    	manor:masterLogout()
    end
    player.off_lineTime = os.time()
    for k , entity in pairs(player.world_products) do
        EngineWorld:removeEntity(entity)
    end
    for i, v in pairs(player.products) do
        if v.Time == -1 then
            v.Time = -1
        else
            if v.Time ~= 0 then
                if player.unlockTime ~= 0 then
                    if v.First == false then
                        v.Time = v.Time - (player.off_lineTime - player.unlockTime)
                    else
                        v.Time = v.Time - (player.off_lineTime - player.update_Time)
                        v.First = false
                    end
                else
                    v.Time = v.Time - (player.off_lineTime - player.on_lineTime)
                end
            end
            if v.Time <= 0 then
                v.Time = 0
                v.pre_product_list = {}
                v.had_product_list = {}
            end
        end
    end
    player:unInitSelfArea()
    player:unInitSelfNpc()
    GameAnalytics:design(player.userId, 0, { "g1050StayTime", player.stay_time} )
    DBManager:removeCache(player.userId, {DbUtil.TAG_SHAREDATA})
    DBManager:removeCache(player.userId, {DbUtil.TAG_MIAN_LINE})
    self:releaseManor(player.userId)
    self:deleteCallOnTarget(player.userId)
end

function GameMatch:initUserManorInfo(userId, manorInfo)
    GameMatch.userManorInfo[tostring(userId)] = manorInfo
    HostApi.log("==== LuaLog: GameMatch:initUserManorInfo : [".. tostring(userId) .. "] 's information init")
end

function GameMatch:initCallOnTarget(userId, targetUserId)
    self.callOnTarget[tostring(userId)] = {}
    self.callOnTarget[tostring(userId)].userId = targetUserId
    self.callOnTarget[tostring(userId)].time = 0
end

function GameMatch:initErrorTarget(userId)
    self.errorTarget[tostring(userId)] = {}
    self.errorTarget[tostring(userId)].userId = userId
    self.errorTarget[tostring(userId)].time = 0
end

function GameMatch:deleteManorInfo(userId)
    GameMatch.userManorInfo[tostring(userId)] = nil
    HostApi.log("==== LuaLog: GameMatch:deleteManorInfo : [".. tostring(userId) .. "] 's information delete")
end

function GameMatch:deleteCallOnTarget(userId)
    self.callOnTarget[tostring(userId)] = nil
end

function GameMatch:deleteErrorTarget(userId)
    self.errorTarget[tostring(userId)] = nil
end

function GameMatch:getUserManorInfo(userId)
    return self.userManorInfo[tostring(userId)]
end

function GameMatch:getTargetByUid(userId)
    if self.callOnTarget[tostring(userId)] ~= nil then
        return self.callOnTarget[tostring(userId)].userId
    end

    return nil
end

function GameMatch:getCallOnTargets()
    return self.callOnTarget
end

function GameMatch:getErrorTarget(userId)
    if self.errorTarget[tostring(userId)] ~= nil then
        return self.errorTarget[tostring(userId)].userId
    end

    return nil
end

function GameMatch:getErrorTargets()
    return self.errorTarget
end

function GameMatch:releaseManor(userId)
    self:deleteManorInfo(userId)
    HostApi.sendReleaseManor(userId)
    DBManager:removeCache(userId)
end

function GameMatch:kickOutErrorPlayer()
    for _, errorTarget in pairs(self.errorTarget) do
        local callOnTargets = self.callOnTarget
        for playerId, target in pairs(callOnTargets) do
            if tostring(target.userId) == tostring(errorTarget.userId) then
                local player = PlayerManager:getPlayerByUserId(playerId)
                if player ~= nil then
                    HostApi.log("player[", tostring(playerId),"] has been tick out")
                    HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
                end
            end
        end
    end
end

function GameMatch:updateManorTime()
    for i, manor in pairs(self.userManorInfo) do
        if manor:getMasterState() then
            manor:refreshTime()
        else
            manor:checkVisitor()

            if manor:isEmptyVisitor() then
                manor:addTime(1)
            else
                manor:refreshTime()
            end

            if manor:getTime() >= 120 then
                GameMatch:releaseManor(manor.userId)

                local player = PlayerManager:getPlayerByUserId(manor.userId)
                if player then
                    HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
                end
            end
        end
    end
end

function GameMatch:updateCallOnTargetTime()
    for userId, v in pairs(self.callOnTarget) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            v.time = v.time + 1
        else
            v.time = 0
        end

        if v.time > 120 then
            self:deleteCallOnTarget(userId)
            -- DBManager:removeCache(userId, {DbUtil.TAG_PLAYER})
            if player then
                HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
            end
        end
    end
end

function GameMatch:updateErrorTargetTime()
    for userId, v in pairs(self.errorTarget) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            -- ExpConfig("errorTarget : userId[".. tostring(userId) .."]  --  time[".. v.time .."] ")
            v.time = v.time + 1
        else
            v.time = 0
        end
        if v.time > 10 then
            self:deleteErrorTarget(userId)
        end
    end
end

function GameMatch:getPlayerByPos(vec3)
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player:isIn(vec3) then
            return player
        end
    end
    return nil
end

function GameMatch:addPlayerScore()
    if self.curTick == 0 or self.curTick % 60 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:addScore(10)
    end
end

function GameMatch:checkIfChristmasFinish()
    local activityTime = os.time() - GameConfig.christmasFinishTime
    if activityTime < 0 then
        return true
    else
        return false
    end
end

return GameMatch