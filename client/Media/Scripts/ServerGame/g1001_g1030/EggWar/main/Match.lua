--Match.lua

GameMatch = {}

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.GameTime = 3
GameMatch.Status.GameOver = 4

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.gameType = "g1018"
GameMatch.startWait = false
GameMatch.hasStartGame = false
GameMatch.hasEndGame = false
GameMatch.firstKill = false

GameMatch.Teams = {}
GameMatch.Merchants = {}

GameMatch.allowPvp = false
GameMatch.allowEnchantment = false

GameMatch.money = {}
GameMatch.egg = {}

function GameMatch:prepareMatch()
    self.hasStartGame = true
    GamePrepareTask:start()
end

function GameMatch:startMatch()
    self.allowPvp = true
    RewardManager:startGame()
    if (GameMatch:getLifeTeams() <= 1) then
        MsgSender.sendMsg(IMessages:msgGameOverByLittle())
        self:endMatch()
    else
        MsgSender.sendMsg(IMessages:msgGameStart())
        GameTimeTask:start()
    end
    self:sendAllPlayerTeamInfo()
end

function GameMatch:sendAllPlayerTeamInfo()
    local result = {}
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.team and player.team.id and player.team.id ~= 0 then
            local item = {}
            item.userId = tonumber(tostring(player.userId))
            item.teamId = player.team.id
            table.insert(result, item)
        end
    end
    print(json.encode(result))
    HostApi.sendPlayerTeamInfo(json.encode(result))
end

function GameMatch:upgradeResource(id)
    local level = self.money[id].mineral.level
    if level >= GameConfig.maxLevel then
        return
    end
    level = level + 1
    local money = MoneyConfig:getMoneyConfigByIdAndLevel(id, level)
    self.money[id].mineral = money
end

function GameMatch:getMoneyConfigByEntityId(entityId)
    for i, v in pairs(self.money) do
        if entityId == v.entityId then
            return v
        end
    end
    return nil
end

function GameMatch:getEggConfigByEntityId(entityId)
    for i, v in pairs(self.egg) do
        if entityId == v.entityId then
            return v
        end
    end
end

function GameMatch:endMatch()

    GamePrepareTask:pureStop()
    GameTimeTask:pureStop()

    local finalWin = true
    local winner = 0

    if (GameMatch:getLifeTeams() > 1) then
        finalWin = false
    end

    local winPlayerName

    for i, v in pairs(self.Teams) do
        if finalWin and v:isDeath() == false then
            winPlayerName = v:getDisplayName()
            winner = v.id
        end
    end

    if finalWin and winPlayerName ~= nil then
        MsgSender.sendMsg(IMessages:msgWinnerInfo(winPlayerName))
    else
        MsgSender.sendMsg(IMessages:msgNoWinner(1))
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.team.id == winner then
            v:onWin()
        end
    end

    self:doReward(winner)

    for i, v in pairs(players) do
        if v.isLife then
            v:onGameEnd(finalWin)
        end
    end

    HostApi.sendPlaySound(0, 10003)
    self.hasEndGame = true
    GameOverTask:start()
end

function GameMatch:doReward(winner)
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        UserExpManager:addUserExp(player.userId, winner == player:getTeamId(), 4)
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
    item.adSwitch = player.adSwitch or 0
    if item.gold <= 0 then--or player.isSingleReward then
        item.adSwitch = 0
    end
    if player.team.id == winner then
        item.isWin = 1
    else
        if winner == 0 then
            if player.isLife then
                item.isWin = 2
            else
                item.isWin = 0
            end
        else
            item.isWin = 0
        end
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
    local players = self:sortPlayerRank()
    return RankManager:getPlayerRank(players, player)
end

function GameMatch:cleanPlayer()
    self:doReport()
    local isWin = GameMatch:getLifeTeams() == 1
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:overGame(isWin)
    end
end

function GameMatch:doReport()
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        ReportManager:reportUserData(player.userId, player.kills, rank, 1)
    end
    GameServer:closeServer()
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

function GameMatch:getRealLifeTeams()
    local lifeTeam = 0
    for i, v in pairs(self.Teams) do
        if v.realLife then
            lifeTeam = lifeTeam + 1
        end
    end
    return lifeTeam
end

function GameMatch:getRealLifePlayerByTeamId(teamId)
    local life = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.team.id == teamId then
            if v.realLife then
                life = life + 1
            end
        end
    end
    return life
end

function GameMatch:ifGameOver()
    local lifeTeam = self:getLifeTeams()
    if lifeTeam <= 1 then
        self:endMatch()
    end
end

function GameMatch:onPlayerDie(player)
    if player.team.isHaveBed then
        player.isKeepItem = false
        player.keepItemSource = 0
        local Config = KeepItemConfig:getKeepItemByTimes(player.keepItemTimes)
        player.KeepItemSource = 0
        HostApi.sendKeepItemTip(player.rakssid, Config.coinId, Config.price, Config.tipTime)
    else
        if self:getRealLifeTeams() <= 2 then
            player.realLife = false
            local life = self:getRealLifePlayerByTeamId(player.team.id)
            if life == 0 then
                local players = PlayerManager:getPlayers()
                for i, v in pairs(players) do
                    if v.realLife == false then
                        v:onDie()
                    end
                end
                GameMatch:ifGameOver()
                return
            end
        end
        player:showBuyRespawn()
    end
end

function GameMatch:isGameStart()
    return self.hasStartGame
end

-- player died or logout
function GameMatch:onPlayerQuit(player)
    player:onLogout()
    player:onQuit()
    if self.hasStartGame == true and self.hasEndGame == false then
        self:ifGameOver()
    end
end

return GameMatch

--endregion
