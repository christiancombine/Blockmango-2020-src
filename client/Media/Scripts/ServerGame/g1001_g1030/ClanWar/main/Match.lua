--Match.lua
require "messages.Messages"

GameMatch = {}

GameMatch.SCORE_KILL = 10
GameMatch.SCORE_FLAG = 40
GameMatch.SCORE_DIE = -3

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.Running = 2
GameMatch.Status.GameOver = 3
GameMatch.Status.WaitingReset = 4

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.gameType = "g1005"

GameMatch.gameFirstTick = 0
GameMatch.gameStartTick = 0
GameMatch.gameOverTick = 0
GameMatch.gameTelportTick = 0

GameMatch.firstKill = false
GameMatch.lastKiller = ""
GameMatch.lastKillerKills = 0

GameMatch.Teams = {}

GameMatch.allowPvp = false

GameMatch.record = {}
GameMatch.recordNum = 1

GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
    self:initTeams()
end

function GameMatch:initTeams()
    self.Teams[1] = GameTeam.new(1)
    self.Teams[2] = GameTeam.new(2)
end

function GameMatch:startGame()
    self.allowPvp = true
    self.gameStartTick = self.curTick
    self.curStatus = self.Status.Running
    MsgSender.sendMsg(IMessages:msgGameStart())
    MsgSender.sendMsg(Messages:gameStartWelcome())
    HostApi.sendGameStatus(0, 1)
end

function GameMatch:endGame()
    self.allowPvp = false
    self.gameOverTick = self.curTick
    self.curStatus = self.Status.GameOver
    self:doReward()
end

function GameMatch:resetKillRecord()
    self.firstKill = false
    self.lastKiller = ""
    self.lastKillerKills = 0
end

function GameMatch:resetPlayer()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:reset()
    end
end

function GameMatch:resetTeam()
    for i, v in pairs(self.Teams) do
        v:reset()
    end
end

function GameMatch:resetRecord()
    self.recordNum = 1
    self.record = {}
end

function GameMatch:resetMap()
    HostApi:resetMap()
end

function GameMatch:resetGame()
    self:resetKillRecord()
    self:resetPlayer()
    self:resetTeam()
    self:resetMap()
    self.gameTelportTick = self.curTick
    self.curStatus = self.Status.WaitingReset
    HostApi.sendGameStatus(0, 2)
    MsgSender.sendMsg(Messages:gameResetTimeHint())
    MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.resetTime, IMessages.UNIT_SECOND, false))
end

function GameMatch:ifGameStart()
    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
        if self.curTick - self.gameFirstTick > 10 then
            self:startGame()
        else
            local seconds = 60 - (self.curTick - self.gameFirstTick)
            if seconds <= 10 and seconds > 0 then
                MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
                if seconds <= 3 then
                    HostApi.sendPlaySound(0, 12);
                else
                    HostApi.sendPlaySound(0, 11);
                end
            end
        end
    end
end

function GameMatch:ifGameOver()

    if self.curTick % 2 == 0 then
        MsgSender.sendBottomTips(3, Messages:gameData(self.Teams))
    end

    local seconds = GameConfig.gameTime * 60 - (self.curTick - self.gameStartTick);

    if seconds % 60 == 0 and seconds / 60 > 0 then
        MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    end

    if seconds <= 0 then
        self:endGame()
    end

end

function GameMatch:resetSignText(ticks)
    if ticks % 10 ~= 0 then
        return
    end
    for i, v in pairs(TeamConfig.teams) do
        EngineWorld:getWorld():resetSignText(v.signPos, 1, Messages:getFlagHint())
    end
end

function GameMatch:ifTelportEnd()

    local seconds = GameConfig.gameOverTime - (self.curTick - self.gameOverTick);

    MsgSender.sendBottomTips(3, Messages:gameOverHint(seconds))

    if seconds <= 0 then
        self:resetGame()
    end
end

function GameMatch:ifResetEnd()

    local seconds = GameConfig.resetTime - (self.curTick - self.gameTelportTick);

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds <= 0 then
        self.curStatus = self.Status.WaitingPlayer
        self:ifGameStart()
    end

end

function GameMatch:onTick(ticks)

    self.curTick = ticks

    if self.curStatus == self.Status.WaitingPlayer then
        self:ifGameStart()
    end

    if self.curStatus == self.Status.Running then
        self:ifGameOver()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifTelportEnd()
    end

    if self.curStatus == self.Status.WaitingReset then
        self:ifResetEnd()
    end

    self:resetSignText(ticks)

end

function GameMatch:isGameStart()
    return self.curStatus == self.Status.Running
end

function GameMatch:onChestOpen(vec3, inv, rakssid)
    for i, v in pairs(self.Teams) do
        v:onChestOpen(vec3, inv)
    end
end

function GameMatch:getWinTeam()
    local red = self.Teams[1]
    local blue = self.Teams[2]
    local win = red
    if red.score == blue.score then
        if red.flag == blue.flag then
            if red.kills == blue.kills then
                if red.dies == blue.dies then
                    if red.curPlayers == blue.curPlayers then
                        local random = math.random(1, 2)
                        if random == 2 then
                            win = blue
                        end
                    else
                        if red.curPlayers > blue.curPlayers then
                            win = red
                        else
                            win = blue
                        end
                    end
                else
                    if red.dies > blue.dies then
                        win = blue
                    else
                        win = red
                    end
                end
            else
                if red.kills > blue.kills then
                    win = red
                else
                    win = blue
                end
            end
        else
            if red.flag > blue.flag then
                win = red
            else
                win = blue
            end
        end
    else
        if red.score > blue.score then
            win = red
        else
            win = blue
        end
    end
    return win
end

function GameMatch:doReward()
    local win = self:getWinTeam()
    MsgSender.sendMsg(IMessages:msgWinnerInfo(win:getDisplayName()))
    self:sendGameSettlement(win)
    self:addRecords(win)
    ClanWarWeb:GetReward(self:getRewardRequestBody(), function(data)
        if not data then
            return
        end
        self:sendReward(data)
    end)
end

function GameMatch:getRewardRequestBody()
    local body = {}
    for i, v in pairs(self.record) do
        body[i] = {}
        body[i].userId = v.userId
        body[i].clanId = v.clanId
        body[i].score = v.score
        body[i].isWin = v.isWin
        body[i].region = v.region
    end
    return body
end

function GameMatch:sortPlayerRank()
    local players = PlayerManager:copyPlayers()

    table.sort(players, function(a, b)
        if a.score ~= b.score then
            return a.score > b.score
        end
        if a.flags ~= b.flags then
            return a.flags > b.flags
        end
        if a.kills ~= b.kills then
            return a.kills > b.kills
        end
        if a.dies ~= b.dies then
            return a.dies < b.dies
        end
        return a.userId > b.userId
    end)

    return players
end

function GameMatch:sendGameSettlement(win)

    local Players = self:sortPlayerRank()

    local players = {}

    for rank, player in pairs(Players) do
        players[rank] = self:newSettlementItem(player, rank, win)
    end

    for rank, player in pairs(Players) do
        local result = {}
        result.players = players
        result.own = self:newSettlementItem(player, rank, win)
        HostApi.sendGameSettlement(player.rakssid, json.encode(result), false)
    end
end

function GameMatch:sendReward(data)

    local reward1, reward2, reward3, reward4, reward5 = data[1], data[2], data[3], data[4], data[5]
    local entrys_1_5 = {}
    local index = 1

    if reward1 ~= nil then
        local record1 = self:getRecord(reward1.userId)
        if record1 ~= nil then
            entrys_1_5[index] = self:newResultItem(record1, reward1)
            index = index + 1
        end
    end

    if reward2 ~= nil then
        local record2 = self:getRecord(reward2.userId)
        if record2 ~= nil then
            entrys_1_5[index] = self:newResultItem(record2, reward2)
            index = index + 1
        end
    end

    if reward3 ~= nil then
        local record3 = self:getRecord(reward3.userId)
        if record3 ~= nil then
            entrys_1_5[index] = self:newResultItem(record3, reward3)
            index = index + 1
        end
    end

    if reward4 ~= nil then
        local record4 = self:getRecord(reward4.userId)
        if record4 ~= nil then
            entrys_1_5[index] = self:newResultItem(record4, reward4)
            index = index + 1
        end
    end

    if reward5 ~= nil then
        local record5 = self:getRecord(reward5.userId)
        if record5 ~= nil then
            entrys_1_5[index] = self:newResultItem(record5, reward5)
            index = index + 1
        end
    end

    for i, v in pairs(data) do
        local reward = v
        local record = self:getRecord(v.userId)
        if reward ~= nil and record ~= nil then
            local player = PlayerManager:getPlayerByUserId(v.userId)
            if player ~= nil then
                local entrys = {}
                for a, e in pairs(entrys_1_5) do
                    entrys[a] = e
                end
                entrys[index] = self:newResultItem(record, reward)

                if entrys[index] and entrys[index].isWin then
                    HostApi.sendPlaySound(player.rakssid, 10023)
                else
                    HostApi.sendPlaySound(player.rakssid, 10024)
                end

                HostApi.sendSettlement(player.rakssid, GameMatch.gameType, json.encode(entrys))
            end
        end
    end
end

function GameMatch:getRecord(userId)
    for i, v in pairs(self.record) do
        if tonumber(v.userId) == tonumber(userId) then
            return v
        end
    end
    return nil
end

function GameMatch:newResultItem(record, reward)
    if record ~= nil and reward ~= nil then
        local item = {}
        item.userId = reward.userId
        item.name = record.name
        item.clanName = reward.clanName
        if record.isWin then
            item.isWin = 1
        else
            item.isWin = 0
        end
        item.rank = reward.orderno
        item.killanddie = record.kills .. "/" .. record.dies
        item.flags = record.flags
        item.exp = reward.exp
        item.energy = reward.energy
        item.money = reward.money
        item.score = string.sub(reward.score, 0, #reward.score - 2) .. "%%)"
        return item
    end
    return nil
end

function GameMatch:newSettlementItem(player, rank, win)
    local item = {}
    item.name = player.name
    item.rank = rank
    item.gold = player.gold
    item.available = player.available
    item.hasGet = player.hasGet
    item.points = player.scorePoints
    item.vip = player.vip
    item.kills = player.kills
    if player.team.id == win.id then
        item.isWin = 1
    else
        item.isWin = 0
    end
    return item
end

function GameMatch:addRecords(win)
    self:resetRecord()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        self:addRecord(v, win)
    end
end

function GameMatch:addRecord(player, win)
    self.record[self.recordNum] = {}
    self.record[self.recordNum].userId = tostring(player.userId)
    self.record[self.recordNum].clanId = player.staticAttr.clanId
    self.record[self.recordNum].score = player.score
    self.record[self.recordNum].name = player.name
    self.record[self.recordNum].flags = player.flags
    self.record[self.recordNum].kills = player.kills
    self.record[self.recordNum].dies = player.dies
    if player.team.id == win.id then
        self.record[self.recordNum].isWin = true
    else
        self.record[self.recordNum].isWin = false
    end
    self.record[self.recordNum].region = 1001
    self.recordNum = self.recordNum + 1
end

function GameMatch:onPlayerQuit(player)
    player:onLogout()
end

return GameMatch

--endregion
