require "messages.Messages"
require "manager.TigerLotteryManager"
require "manager.SceneManager"
GameMatch = {}

GameMatch.gameType = "g1047"

GameMatch.curTick = 0
GameMatch.curSecondTime = 0
GameMatch.isDecorationChange = false
GameMatch.isManorAreaChange = false
GameMatch.manorAreaInfo = {}
GameMatch.lastWeekRankUserIds = {}

function GameMatch:initMatch()
    GameTimeTask:start()
    TigerLotteryManager:Init()
    ShopHouseManager:init()
    self.curSecondTime = tonumber(os.date("%S", os.time()))
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:checkPlayerRaceResult()
    self:addPlayerScore()
    self:checkVehicle()
    self:kickOutErrorPlayer()
    SceneManager:onTick(ticks)
    self:updateDecorationArea()
    self:updateManorArea()
    self:checkPlayersWingsTimeEnd()
    self:checkPlayersAttribute()
    self.curSecondTime = self.curSecondTime + 1
    self:updateADRefreshTime()
    self:checkRaceVehicle()
    self:checkAndResetRaceData()
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

function GameMatch:onPlayerQuit(player)
    player:reward()
    player:onQuit()
    DBManager:removeCache(player.userId, {DbUtil.TAG_PLAYER})
end

function GameMatch:checkVehicle()
	if self.curTick == 0 or self.curTick % 600 ~= 0 then
		return 
	end
	VehicleConfig:checkAndResetPosition()
end

function GameMatch:kickOutErrorPlayer()
    for _, errorTarget in pairs(SceneManager.errorTarget) do
        local callOnTargets = SceneManager:getCallOnTargets()
        for playerId, target in pairs(callOnTargets) do
            if tostring(target.userId) == tostring(errorTarget.userId) then
                local player = PlayerManager:getPlayerByUserId(playerId)
                if player ~= nil then
                    print("player[", tostring(playerId),"] has been tick out")
                    HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
                end
            end
        end
    end
end

function GameMatch:updateDecorationArea()
    if not self.isDecorationChange then
        return
    end

    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        -- if player.manor then
        --     player.manor:pushMailBoxAreaInfo()
        -- end

        for _, decoration in pairs(SceneManager.userDecorationInfo) do
            decoration:pushDecorationAreaInfo()
        end

        AreaInfoManager:SyncAreaInfo(player.rakssid)
    end

    self.isDecorationChange = false
end

function GameMatch:pushManorArea(manorIndex, userId)
    local startPos, endPos = ManorConfig:getManorPosByIndex(manorIndex, false)
    if not startPos or not endPos then
        return
    end

    local area = BlockCityManorArea.new()
    area.userId = tonumber(tostring(userId))
    area.startPos = startPos
    area.endPos = endPos
    self.manorAreaInfo[tostring(userId)] = area
    self.isManorAreaChange = true
end

function GameMatch:removeManorArea(userId)
    self.manorAreaInfo[tostring(userId)] = nil
    self.isManorAreaChange = true
end

function GameMatch:updateManorArea()
    if not self.isManorAreaChange then
        return
    end

    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:setManorArea()
    end

    self.isManorAreaChange = false
end

function GameMatch:weekRankReward(player)
    local rankWeek = tostring(player:getRankWeek())
    local thisWeek = tostring(DateUtil.getYearWeek())
    if rankWeek == "0" then
        player:setRankWeek(thisWeek)
        player.lastWeekRank = 0
        return
    end

    local rankYear = string.sub(rankWeek, 1, 4)
    local thisYear = string.sub(thisWeek, 1, 4)
    if tonumber(rankYear) == tonumber(thisYear) then
        if tonumber(thisWeek) - tonumber(rankWeek) ~= 1 then
            player:setRankWeek(thisWeek)
            player.lastWeekRank = 0
            return
        end
    end

    if next(self.lastWeekRankUserIds) then
        for rank, userId in pairs(self.lastWeekRankUserIds) do
            if userId == tostring(player.userId) then
                local rewards = WeekRewardConfig:getRewardsByRank(rank)
                for _, reward in pairs(rewards) do
                    player:getRewardItem(reward)
                end
            end
        end
        player:setRankWeek(thisWeek)
        player.lastWeekRank = 0
        player:setPlayerInfo()
    end
end

function GameMatch:getLastWeekRank()
    WebService:GetAllAppRank("week",0, 30, 1, function(data)
        for _, v in pairs(data.data) do
            GameMatch.lastWeekRankUserIds[v.rank] = tostring(v.userId)
        end
    end)
end

function GameMatch:checkPlayersWingsTimeEnd()
    if self.curTick == 0 or self.curTick % 30 ~= 0 then
        return
    end

    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:checkWingsTimeEnd()
    end
end

function GameMatch:checkPlayersAttribute()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:checkCDRewardTimeEnd()
    end
end

function GameMatch:updateADRefreshTime()
    if self.curSecondTime % 60 ~= 0 then
        return
    end

    self.curSecondTime = tonumber(os.date("%S", os.time()))
    local time = os.date("%H:%M", os.time())
    for _, v in pairs(GameConfig.ADRefreshTime) do
        if v == time then
            local players = PlayerManager:getPlayers()
            for i, player in pairs(players) do
                VideoAdHelper:checkShowAd(player)
            end
        end
    end
end

function GameMatch:checkRaceVehicle()
    if self.curTick == 0 or self.curTick % 60 ~= 0 then
        return
    end
    VehicleConfig:checkAndRemoveRaceVehicle()
end

function GameMatch:checkPlayerRaceResult()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.race and player.race.isRacing then
            player:checkPlayerRaceWin()
        end
    end
end

function GameMatch:checkAndResetRaceData()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.race
                and player.race.competitorUId
                and not player.race.isRacing
                and os.time() - player.race.inviteTime >= GameConfig.inviteShowTime then
            player.race = nil
        end
    end
end

return GameMatch