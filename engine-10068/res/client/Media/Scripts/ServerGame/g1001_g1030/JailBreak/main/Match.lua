---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "config.ProduceConfig"
require "config.BlockConfig"
require "config.ScoreConfig"
require "config.PoliceConfig"
require "config.CriminalConfig"
require "config.VehicleConfig"
require "messages.Messages"

GameMatch = {}

GameMatch.gameType = "g1014"

GameMatch.curTick = 0

GameMatch.selectRoleTrigger = {}
GameMatch.selectPoliceRoleDataTrigger = {}
GameMatch.selectCriminalRoleDataTrigger = {}
GameMatch.selectPoliceRoleDataCache = {}
GameMatch.selectCriminalRoleDataCache = {}

GameMatch.RankData = {}
GameMatch.RankData.day = {}
GameMatch.RankData.week = {}

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:showGameTip()
    self:addPlayerScore()
    self:addPoliceWage()
    self:saveAllPlayerRankData()
    self:checkVehicle()
    self:ifUpdateRank()
    BlockConfig:onTick()
    ProduceConfig:addPropToWorld(ticks)
end

function GameMatch:addPoliceWage()
    if os.time() % PoliceConfig.wageTime ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_POLICE then
            v:addWage()
        end
    end
end

function GameMatch:checkVehicle()
    if self.curTick == 0 or self.curTick % 600 ~= 0 then
        return
    end
    VehicleConfig:checkAndResetPosition()
end

function GameMatch:showGameTip()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if os.time() - v.enterLocationTime < GameConfig.gameTipShowTime then
            return
        end
        v:showGameData()
    end
end

function GameMatch:addPlayerScore()
    if self.curTick == 0 or self.curTick % 60 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:addScore(ScoreConfig.ticket)
    end
end

function GameMatch:onPlayerQuit(player)
    self:savePlayerRankScore(player)
    self:subRankData(player)
    HostApi.sendResetGame(PlayerManager:getPlayerCount())
    player:reward()
    self:sendAllPlayerTeamInfo()
    self:sendRankDataToPlayers()
end

function GameMatch:ifUpdateRank()
    if self.curTick % GameConfig.rankUpdateTime == 0 then
        GameMatch.RankData.day = {}
        GameMatch.RankData.week = {}
        GameMatch:getRankPlayersList()
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isCacheRankData == false then
            return
        end
    end

    self:sendRankDataToPlayers()

    for i, v in pairs(players) do
        v.isCacheRankData = false
    end
end

function GameMatch:sendAllPlayerTeamInfo()
    local result = {}
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        local item = {}
        item.userId = tonumber(tostring(v.userId))
        item.teamId = v.role
        table.insert(result, item)
    end
    HostApi.sendPlayerTeamInfo(json.encode(result))
end

function GameMatch:savePlayerData(player, immediate)
    if player == nil or player.role < 1 then
        return
    end
    local data = GameMatch:getPlayerSaveData(player)
    if player.role == GamePlayer.ROLE_CRIMINAL or player.role == GamePlayer.ROLE_PRISONER then
        DBManager:savePlayerData(player.userId, GamePlayer.ROLE_CRIMINAL, data, immediate)
    else
        DBManager:savePlayerData(player.userId, GamePlayer.ROLE_POLICE, data, immediate)
    end
    local adData = {}
    adData.dayKey = player.dayKey
    adData.respawnAdTimes = player.respawnAdTimes
    adData.itemAdRecords = player.itemAdRecords
    DBManager:savePlayerData(player.userId, GamePlayer.VIDEO_AD_DATA, adData, immediate)
end

function GameMatch:getPlayerSaveData(player)
    local data = {}
    local inventory = InventoryUtil:getPlayerInventoryInfo(player)
    local enderInv = InventoryUtil:getPlayerEnderInventoryInfo(player)
    data.inventory = inventory
    data.enderInv = enderInv
    data.money = player.money
    data.hp = player:getHealth()
    data.food = player:getFoodLevel()
    local pos = player:getPosition()
    data.position = {}
    data.position.x = pos.x
    data.position.y = pos.y
    data.position.z = pos.z
    if player.role == GamePlayer.ROLE_POLICE then
        data.merit = player.merit
        data.todayWage = player.todayWage
        data.addWageDate = os.date("%Y-%m-%d", player.lastAddWage)
    end
    if player.role == GamePlayer.ROLE_CRIMINAL or player.role == GamePlayer.ROLE_PRISONER then
        data.threat = player.threat
    end
    data.vehicles = player.vehicles
    return data
end

function GameMatch:getPlayerNumForRole(role)
    local num = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == role then
            num = num + 1
        end
    end
    return num
end

function GameMatch:getDBDataRequire(player)
    GameMatch.selectPoliceRoleDataTrigger[tostring(player.userId)] = true
    DBManager:getPlayerData(player.userId, GamePlayer.ROLE_POLICE)

    GameMatch.selectCriminalRoleDataTrigger[tostring(player.userId)] = true
    DBManager:getPlayerData(player.userId, GamePlayer.ROLE_CRIMINAL)
end

function GameMatch:policeRoleDataCache(player, role, policeData)
    local data = {}
    data.police = {}

    local policeConfig = PoliceConfig:getDataByLevel(1)
    data.police.actor = PoliceConfig.actor
    if not policeData.__first then
        local config = PoliceConfig:getDataByMerit(policeData.merit or 0)
        data.police.lv = config.lv or 1
        data.police.money = policeData.money or 0
        data.police.merit = policeData.merit or 0
        data.police.name = config.nameKey or ""
    else
        data.police.lv = policeConfig.lv
        data.police.money = 0
        data.police.merit = 0
        data.police.name = policeConfig.nameKey
    end

    player.DBData.police = {}
    player.DBData.police = data.police
    GameMatch.selectPoliceRoleDataCache[tostring(player.userId)] = true
end

function GameMatch:criminalRoleDataCache(player, role, criminalData)
    local data = {}
    data.criminal = {}

    local criminalConfig = CriminalConfig:getDataByLevel(1)
    data.criminal.actor = CriminalConfig.actor

    if not criminalData.__first then
        local config = CriminalConfig:getDataByThreat(criminalData.threat or 0)
        data.criminal.lv = config.lv or 1
        data.criminal.money = criminalData.money or 0
        data.criminal.threat = criminalData.threat or 0
        data.criminal.name = config.nameKey or ""
    else
        data.criminal.lv = criminalConfig.lv
        data.criminal.money = 0
        data.criminal.threat = 0
        data.criminal.name = criminalConfig.nameKey
    end

    player.DBData.criminal = {}
    player.DBData.criminal = data.criminal
    GameMatch.selectCriminalRoleDataCache[tostring(player.userId)] = true

end

function GameMatch:sendSelectRoleData(player)
    local polices = self:getPlayerNumForRole(GamePlayer.ROLE_POLICE)
    local criminals = self:getPlayerNumForRole(GamePlayer.ROLE_CRIMINAL) + self:getPlayerNumForRole(GamePlayer.ROLE_PRISONER)
    local all = polices + criminals
    if all == 0 then
        player.DBData.police.canSelect = true
        player.DBData.criminal.canSelect = true
    else
        if polices / all >= 0.5 then
            player.DBData.police.canSelect = false
        else
            player.DBData.police.canSelect = true
        end
        if criminals / all >= 0.8 then
            player.DBData.criminal.canSelect = false
        else
            player.DBData.criminal.canSelect = true
        end
    end

    HostApi.sendSelectRoleData(player.rakssid, json.encode(player.DBData))
    player.DBData.police = {}
    player.DBData.criminal = {}
end

function GameMatch:saveAllPlayerRankData()
    if self.curTick == 0 or self.curTick % 30 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        self:savePlayerRankScore(player)
    end
end

function GameMatch:savePlayerRankScore(player)
    local key = ""
    if player.role == GamePlayer.ROLE_POLICE then
        key = self:getPoliceKey()
    elseif player.role == GamePlayer.ROLE_CRIMINAL or player.role == GamePlayer.ROLE_PRISONER then
        key = self:getCriminalKey()
    end
    local uid = tostring(player.userId)
    HostApi.ZIncrBy(key, uid, player.gameScore)
    player.gameScore = 0
end

function GameMatch:getPoliceKey()
    return GameMatch.gameType .. ".police"
end

function GameMatch:getCriminalKey()
    return GameMatch.gameType .. ".criminal"
end

function GameMatch:addPoliceRank(rank, player)
    self.RankData.day[tostring(player.userId)] = rank
end

function GameMatch:addCriminalRank(rank, player)
    self.RankData.week[tostring(player.userId)] = rank
end

function GameMatch:getPlayerRankData(player)
    local key = ""
    if player.role == GamePlayer.ROLE_POLICE then
        key = self:getPoliceKey()
    elseif player.role == GamePlayer.ROLE_CRIMINAL or player.role == GamePlayer.ROLE_PRISONER then
        key = self:getCriminalKey()
    end
    local uid = tostring(player.userId)
    HostApi.ZScore(key, uid)
end

function GameMatch:sendRankDataToPlayers()

    local dayRanks = self:sortRankData(self.RankData.day)
    local weekRanks = self:sortRankData(self.RankData.week)

    local data = {}
    data.ranks = {}
    data.ranks.day = dayRanks
    data.ranks.week = weekRanks
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        local dayRankSort = 1
        for j, rank in pairs(dayRanks) do
            rank.rank = dayRankSort
            dayRankSort = dayRankSort + 1
            if tonumber(v.userId) == rank.userId then
                v.RankData.day.rank = rank.rank
            end
            data.own = v.RankData
        end

        local weekRankSort = 1
        for j, rank in pairs(weekRanks) do
            rank.rank = weekRankSort
            weekRankSort = weekRankSort + 1
            if tonumber(v.userId) == rank.userId then
                v.RankData.week.rank = rank.rank
            end
            data.own = v.RankData
        end
        --HostApi.log("GameMatch:sendRankDataToPlayers: " .. json.encode(data))
        RankNpcConfig:sendRankData(v.rakssid, json.encode(data))
    end
end

function GameMatch:sortRankData(rankData)
    local ranks = {}
    local index = 1

    for i, v in pairs(rankData) do
        ranks[index] = v
        index = index + 1
    end

    table.sort(ranks, function(a, b)
        if a.score ~= b.score then
            return a.score > b.score
        end

        if a.rank ~= b.rank then
            return a.rank > b.rank
        end

        return a.userId > b.userId
    end)

    return ranks
end

function GameMatch:subRankData(player)
    if player.role == GamePlayer.ROLE_POLICE then
        self.RankData.day[tostring(player.userId)] = nil
    end

    if player.role == GamePlayer.ROLE_CRIMINAL or player.role == GamePlayer.ROLE_PRISONER then
        self.RankData.week[tostring(player.userId)] = nil
    end
end

function GameMatch:getRankPlayersList()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        self:getPlayerRankData(v)
    end
end

return GameMatch