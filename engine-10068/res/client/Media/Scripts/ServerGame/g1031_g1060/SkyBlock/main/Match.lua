--Match.lua
require "util.DbUtil"
require "util.BackHallManager"
require "data.GameParty"
require "data.GameMultiBuild"
require "util.RechargeUtil"
require "util.SaveDBTaskQueue"

GameMatch = {}

GameMatch.gameType = "g1048"
GameMatch.curTick = 0
GameMatch.userManorInfo = {}
GameMatch.callOnTarget = {}
GameMatch.errorTarget = {}
GameMatch.treeTime = {}
GameMatch.dirtTime = {}

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:updateCallOnTargetTime()
    self:updateErrorTargetTime()
    self:updateManorTime()
    self:kickOutErrorPlayer()
    self:updateAllPlayer()
    self:ifSaveRankData()
    self:ifUpdateRank()
    self:addPlayerScore()
    self:ifUpdateParty()
    BackHallManager:onTick()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onTick()
    end
    self:updateWeather()
end

function GameMatch:updateAllPlayer()
    for k, v in pairs(self.userManorInfo) do
        v:update()
        v:updateAttachBlock()
        v:updateSignPostText()
    end

    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:update()
    end
end

function GameMatch:onPlayerQuit(player)
    GameParty:partySubPlayer(player.userId)
    player:quitAskMemeberChangeShow()
    GameAnalytics:design(player.userId, 0, { "g1048StayTime", player.stay_time} )
    RankConfig:savePlayerRankScore(player)
    DBManager:removeCache(player.userId, {DbUtil.TAG_PLAYER})
    DBManager:removeCache(player.userId, {DbUtil.TAG_SHAREDATA})
    DBManager:removeCache(player.userId, {DbUtil.TAG_CHEST})
    DBManager:removeCache(player.userId, {DbUtil.TAG_MIAN_LINE})
    DBManager:removeCache(player.userId, {DbUtil.DIALOG_TIPS})

    if GameMatch.treeTime[tostring(player.userId)] then
        print("onPlayerQuit tree " .. tostring(player.userId))
        for _, timer in pairs(GameMatch.treeTime[tostring(player.userId)]) do
            LuaTimer:cancel(timer)
            timer = nil
        end
        GameMatch.treeTime[tostring(player.userId)] = nil
    end

    if GameMatch.dirtTime[tostring(player.userId)] then
        print("onPlayerQuit dirt " .. tostring(player.userId))
        for _, timer in pairs(GameMatch.dirtTime[tostring(player.userId)]) do
            LuaTimer:cancel(timer)
            timer = nil
        end
        GameMatch.dirtTime[tostring(player.userId)] = nil
    end

    local manor = GameMatch.userManorInfo[tostring(player.userId)]
    player:reward()
    if manor ~= nil then
        manor:masterLogout()
        manor:clearCache()
    end
    player:designStayTime()
    GameMultiBuild:onPlayerLogout(player)
end

function GameMatch:initUserManorInfo(userId, manorInfo)
    GameMatch.userManorInfo[tostring(userId)] = manorInfo
    HostApi.log("==== LuaLog: GameMatch:initUserManorInfo : [".. tostring(userId) .. "] 's information init")
end

function GameMatch:hasErrorDataInManor(player_userId)
    local manorIndex = 0
    local hasManor = false
    if GameMatch.userManorInfo then
        for userId, manor in pairs(GameMatch.userManorInfo) do
            if tostring(userId) == tostring(player_userId) then
                manorIndex = manor.manorIndex
                hasManor = true
                break
            end
        end
    end
    local repeatManorIndex = false

    if manorIndex > 0 and hasManor == true then
        for userId, manor in pairs(GameMatch.userManorInfo) do
            if manorIndex == manor.manorIndex and tostring(userId) ~= tostring(player_userId) then
                repeatManorIndex = true
                break
            end
        end
    end

    return repeatManorIndex
end

function GameMatch:isRepeatManorIndex(index)
    if GameMatch.userManorInfo then
        for _, manor in pairs(GameMatch.userManorInfo) do
            if manor.manorIndex == index then
                return true, manor
            end
        end
    end
    return false, nil
end

function GameMatch:isMaster(userId)
    if self.callOnTarget[tostring(userId)] then
        return tostring(self.callOnTarget[tostring(userId)].userId) == tostring(userId)
    end
    return false
end

function GameMatch:initCallOnTarget(userId, targetUserId)
    self.callOnTarget[tostring(userId)] = {}
    self.callOnTarget[tostring(userId)].userId = targetUserId
    self.callOnTarget[tostring(userId)].time = 0
end

function GameMatch:getCallonUserId(targetUserId)
    for userId, callon in pairs(self.callOnTarget) do
        if callon.userId == tostring(targetUserId) then
            return userId
        end
    end
    return 0
end

function GameMatch:getCallonTargetId(userId)
    for userId, callon in pairs(self.callOnTarget) do
        if userId == tostring(userId) then
            return callon.userId
        end
    end
    return 0
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

function GameMatch:getUserManorInfoByIndex(manorIndex)
    for key, manor in pairs(self.userManorInfo) do
        if manor.manorIndex == manorIndex then
            return manor
        end
    end
    return nil
end

function GameMatch:getTargetByUid(userId)
    if self.callOnTarget[tostring(userId)] ~= nil then
        return self.callOnTarget[tostring(userId)].userId
    end

    return nil
end

function GameMatch:getTargetManorByUid(userId)
    local targetUserId = GameMatch:getTargetByUid(userId)
    if targetUserId ~= nil then
        local targetManorInfo = GameMatch:getUserManorInfo(targetUserId)
        return targetManorInfo
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
    local manorInfo = GameMatch:getUserManorInfo(userId)
    if manorInfo ~= nil then
        manorInfo:clearBlockDb()
        manorInfo:clearCrops()
        manorInfo:clearActors()
    end

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

            if manor:getTime() >= Define.startReleaseTime then
                HostApi.log("GameMatch:releaseManor " .. tostring(manor.userId))

                if manor.release_status == Define.manorRelese.Not then
                    GameMatch:releaseManor(manor.userId)
                    local player = PlayerManager:getPlayerByUserId(manor.userId)
                    if player then
                        HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
                    end
                elseif manor.release_status == Define.manorRelese.End then
                    HostApi.log("sendReleaseManor " .. tostring(manor.userId))
                    manor:clearMem()
                    DBManager:removeCache(manor.userId)
                    GameMatch:deleteManorInfo(manor.userId)
                    HostApi.sendReleaseManor(manor.userId)
                    local player = PlayerManager:getPlayerByUserId(manor.userId)
                    if player then
                        HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
                    end
                else

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

        if v.time >= Define.startReleaseTime then
            HostApi.log("GameMatch:deleteCallOnTarget " .. tostring(userId))
            self:deleteCallOnTarget(userId)
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

function GameMatch:getMasterByPos(vec3)
    for k, v in pairs(self.userManorInfo) do
        if v:isIn(vec3) then
            return PlayerManager:getPlayerByUserId(k)
        end
    end
    return nil
end

function GameMatch:getManorByPos(vec3)
    for k, v in pairs(self.userManorInfo) do
        if v:isIn(vec3) then
            return v
        end
    end
    return nil
end

function GameMatch:ifSaveRankData()
    if self.curTick == 0 or self.curTick % 30 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        RankConfig:savePlayerRankScore(player)
    end
end

function GameMatch:ifUpdateRank()
    if self.curTick % 600 == 0 and PlayerManager:isPlayerExists() then
        RankConfig:updateRank()
    end
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

function GameMatch:leaveCurrentManor(userId)
    local visitUserId = GameMatch:getTargetByUid(userId)
    if visitUserId then
        local visitManor = GameMatch:getUserManorInfo(visitUserId)
        if visitManor then
            visitManor:visitorLeaveHouse(userId)
        end
    end
end

function GameMatch:ifUpdateParty()
    if self.curTick % 300 == 0 and PlayerManager:isPlayerExists() then
        GameParty:getAllPartyList()

        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            GameParty:getFriendPartyList(player.userId)
            GameParty:getFreeCreateNum(player.userId)
        end
    end
end

function GameMatch:createPartySuccess(userId)
    -- if not GameMatch:isMaster(userId) then
    --     return
    -- end

    local player = PlayerManager:getPlayerByUserId(userId)
    local commonMsg = "0"
    if player then
        commonMsg = player.msgTips
    end

    local manorInfo = GameMatch:getUserManorInfo(userId)
    if manorInfo then
        for k, v in pairs(manorInfo.visitor) do
            local visitor_player = PlayerManager:getPlayerByUserId(v)
            if visitor_player and tostring(visitor_player.userId) ~= tostring(userId) then
                if GameMultiBuild:checkIsMember(v, userId) then
                    visitor_player.tips = "master_open_party_to_happy"
                    visitor_player.tipsUserId = tostring(userId)
                    -- HostApi.sendCommonTip(visitor_player.rakssid, "master_party_create_success_goparty")
                    MsgSender.sendCenterTipsToTarget(visitor_player.rakssid, 100, Messages:msgGoParty())
                    EngineUtil.sendEnterOtherGame(visitor_player.rakssid, "g1048", userId)
                    visitor_player.isNotInPartyCreateOrOver = false
                    self:partyGameOverToPlayer(v)
                else
                    visitor_player.tips = commonMsg
                    visitor_player.tipsUserId = tostring(visitor_player.userId)
                    -- HostApi.sendCommonTip(visitor_player.rakssid, "master_party_create_success_backhome")
                    if visitor_player:checkBackHome() then
                    	visitor_player.isNotInPartyCreateOrOver = false
                        self:partyGameOverToPlayer(v)
                    end
                end
            end
        end

        if player then
            player.isNotInPartyCreateOrOver = false
        end

        manorInfo.time = Define.startReleaseTime
        manorInfo:masterLogout()
        manorInfo.isNotInPartyCreateOrOver = false
        GameMatch:releaseManor(userId)

        if player then
            EngineUtil.sendEnterOtherGame(player.rakssid, "g1048", player.userId)
            self:sendCreatePartySuccessTip(userId)
            -- HostApi.sendCommonTip(player.rakssid, "party_create_success")
        end

        self:partyGameOverToPlayer(userId)
    end
end

function GameMatch:sendCreatePartySuccessTip(userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local VIP = false
    if PrivilegeConfig:checkPlayerShowPri(player) then
        VIP = true
    end
    local tip = GameConfig.createPartySuccessTip
    if tip then
        local data =
        {
            message = tip,
            top = true,
            isVip = VIP,
            isShowBtn = true,
            targetId = tonumber(tostring(player.userId))
        }

        data.params = player:getDisplayName()
        WebService:SendMsg(nil, json.encode(data), 4, "game", GameMatch.gameType)
        --local players = PlayerManager:getPlayers()
        --for i, player in pairs(players) do
        --    HostApi.addDefaultNotification(player.rakssid, json.encode(data))
        --end
    end
end

function GameMatch:partyGameOverToPlayer(userId)
    LuaTimer:schedule(function(userId)
        local player = PlayerManager:getPlayerByUserId(userId)
        if player == nil then
            return
        end
        HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.GameOver)
    end, 7000, nil, tostring(userId))
end

function GameMatch:checkIfChristmasFinish()
    local activityTime = os.time() - GameConfig.christmasFinishTime
    if activityTime < 0 then
        return true
    else
        return false
    end
end

function GameMatch:updateWeather()
    if self:checkIfChristmasFinish() then
        local tick = tonumber(tostring(EngineWorld:getWorld():getWorldTime()))
        local hour = (tick % 24000 + 8000) / 1000
        if GameConfig:isInSnowTime(hour) then
            HostApi.setGameWeather(2)
        else
            HostApi.setGameWeather(0)
        end
    else
        HostApi.setGameWeather(0)
    end
end

return GameMatch