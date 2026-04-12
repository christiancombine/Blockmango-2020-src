--Match.lua

GameMatch = {}

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.GameTime = 3
GameMatch.Status.GameOver = 4

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.gameType = "g1012"
GameMatch.startWait = false
GameMatch.hasStartGame = false
GameMatch.hasEndGame = false
GameMatch.curDeaths = 0

GameMatch.Teams = {}

GameMatch.snowPos = {}
GameMatch.dierList = {}

GameMatch.lastKiller = nil
GameMatch.lastKillNum = 0
GameMatch.lastKillTime = 0

function GameMatch:prepareMatch()
    MsgSender.sendMsg(IMessages:msgReadyHint(GameConfig.prepareTime))
    GamePrepareTask:start()
end

function GameMatch:startMatch()
    self.hasStartGame = true
    RewardManager:startGame()
    if (GameMatch:getLifeTeams() <= 1) then
        MsgSender.sendMsg(IMessages:msgGameOverByLittle())
        self:endMatch()
    else
        MsgSender.sendMsg(IMessages:msgGameStart())
        GameTimeTask:start()
    end
end

function GameMatch:isGameStart()
    return self.hasStartGame
end

function GameMatch:getLifeTeams()
    local lifeTeam = 0
    for i, v in pairs(self.Teams) do
        if v:isDeath() == false then
            lifeTeam = lifeTeam + 1
        end
    end
    return lifeTeam
end

function GameMatch:getWinTeam()
    local winTeam
    local team1 = self.Teams[1]
    local team2 = self.Teams[2]
    if team1 and team2 then
        if team1:getKillNum() > team2:getKillNum() then
            winTeam = (team1:isDeath() and { team2 } or { team1 })[1]
        elseif team1:getKillNum() < team2:getKillNum() then
            winTeam = (team2:isDeath() and { team1 } or { team2 })[1]
        end
    end

    return winTeam
end

function GameMatch:endMatch()

    GameTimeTask:pureStop()

    local winTeam = self:getWinTeam()
    local winner = 0
    local players = PlayerManager:getPlayers()
    if winTeam ~= nil then
        MsgSender.sendMsg(IMessages:msgWinnerInfo(winTeam.name))
        winner = winTeam.id
        for i, v in pairs(players) do
            if v.team.id == winner then
                v:onWin()
            end
        end

        self:doReward(winner)

        for i, v in pairs(players) do
            v:onGameEnd(v.team.id == winTeam.id)
        end
    else
        if self:getLifeTeams() > 1 then
            MsgSender.sendMsg(IMessages:msgNoWinner(1))
        end

        self:doReward(winner)

        for i, v in pairs(players) do
            v:onGameEnd(false)
        end
    end

    self.hasEndGame = true
    GameOverTask:start()
end

function GameMatch:doReward(winner)
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        UserExpManager:addUserExp(player.userId, player.team.id == winner, 2)
    end
    RewardManager:getQueueReward(function(data, winner)
        self:sendGameSettlement(winner)
    end, winner)
end

function GameMatch:sendGameSettlement(winner)
    local Players = self:sortPlayerRank()

    local players = {}

    for rank, player in pairs(Players) do
        players[rank] = self:newSettlementItem(player, rank, winner)
    end

    for rank, player in pairs(Players) do
        local result = {}
        result.players = players
        result.own = self:newSettlementItem(player, rank, winner)
        HostApi.sendGameSettlement(player.rakssid, json.encode(result), true)
    end
end

function GameMatch:newSettlementItem(player, rank, winner)
    local item = {}
    item.name = player.name
    item.rank = rank
    item.points = player.scorePoints
    item.gold = player.gold
    item.available = player.available
    item.hasGet = player.hasGet
    item.vip = player.vip
    item.kills = player.kills
    if player.team.id == winner then
        item.isWin = 1
    else
        item.isWin = 0
    end
    return item
end

function GameMatch:sortPlayerRank()
    local players = PlayerManager:copyPlayers()

    table.sort(players, function(a, b)
        if a.score ~= b.score then
            return a.score > b.score
        end
        if a.gold ~= b.gold then
            return a.gold > b.gold
        end
        return a.userId > b.userId
    end)

    return players
end

function GameMatch:getPlayerRank(player)
    local players = self:sortPlayerRank();
    return RankManager:getPlayerRank(players, player)
end

function GameMatch:cleanPlayer()
    self:doReport()
    local isWin = self:getLifeTeams() == 1
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:overGame(false, isWin)
    end
end

function GameMatch:doReport()
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        ReportManager:reportUserData(player.userId, player.kills, rank, 1)
    end
    GameServer:closeServer()
end

function GameMatch:ifGameOver()
    local lifeTeam = self:getLifeTeams()
    local winTeam
    for k, team in pairs(self.Teams) do
        if team:getKillNum() >= GameConfig.winCondition then
            winTeam = team
            break
        end
    end
    if lifeTeam <= 1 or winTeam ~= nil then
        GamePrepareTask:pureStop()
        GameTimeTask:pureStop()
        self:endMatch()
    end
end

-- player died or logout
function GameMatch:onPlayerQuit(player)
    player:onLogout()
    player:onQuit()
    if self.hasStartGame == true and self.hasEndGame == false then
        self:ifGameOver()
    end
end

function GameMatch:addSnowPos(vec3)
    local newSnowPos = {}
    newSnowPos.pos = {}
    newSnowPos.pos.x = vec3.x
    newSnowPos.pos.y = vec3.y
    newSnowPos.pos.z = vec3.z
    newSnowPos.delay = math.random(15, 20)
    self.snowPos[VectorUtil.hashVector3(vec3)] = newSnowPos
end

function GameMatch:setSnowBlock()
    for k, v in pairs(self.snowPos) do
        if v.delay > 0 then
            v.delay = v.delay - 1
        else
            EngineWorld:setBlock(VectorUtil.newVector3i(v.pos.x, v.pos.y, v.pos.z), 78)
            self.snowPos[k] = nil
        end
    end
end

function GameMatch:prepareSnow()
    for i, team in pairs(TeamConfig.teams) do
        local area = team.area
        for x = tonumber(area.minX), tonumber(area.maxX), 1 do
            for z = tonumber(area.minZ), tonumber(area.maxZ), 1 do
                for y = tonumber(area.maxY), tonumber(area.minY), -1 do
                    local id = EngineWorld:getBlockId(VectorUtil.newVector3i(x, y, z))
                    if id ~= 0 and id ~= 78 then
                        if GameConfig.noSnowIds[id] == nil then
                            if id ~= 0 then
                                if GameConfig.changeSnowIds[id] then
                                    EngineWorld:setBlock(VectorUtil.newVector3i(x, y, z), 78)
                                else
                                    EngineWorld:setBlock(VectorUtil.newVector3i(x, y + 1, z), 78)
                                end
                            end
                        end
                        break
                    end
                end
            end
        end
    end
end

return GameMatch

--endregion