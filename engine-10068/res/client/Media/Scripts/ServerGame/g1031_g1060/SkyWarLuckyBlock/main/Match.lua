require "messages.Messages"
require "util.DbUtil"
require "manager.SkillManager"
require "manager.DeBuffManager"
require "manager.BoxManager"
require "manager.BlockGoodsManager"
require "manager.EntityManager"
require "config.Define"
require "config.BlockGoodsPool"
require "data.GameTeam"
require "config.GameTip"
require "util.ReportUtil"
require "manager.CollisionManager"
require "util.BackHallManager"
require "config.HonourConfig"

GameMatch = {}

GameMatch.gameType = "g1054"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.Waiting = 1 --等待玩家
GameMatch.Status.Prepare = 2 --准备阶段
GameMatch.Status.Running = 3 --游戏阶段
GameMatch.Status.GameOver = 4

GameMatch.curStatus = 0
GameMatch.teams = {}
GameMatch.winner = 0
GameMatch.savePlayerRank = {}
GameMatch.DragonKnightName = ""

GameMatch.IsWaitingEnd = false
GameMatch.WaitingTick = 0
GameMatch.PrepareTick = 0
GameMatch.RunningTick = 0

GameMatch.GameOverTick = 0

GameMatch.curTick = 0

GameMatch.index = 1
GameMatch.time = 0      --倒计时

GameMatch.TeamSign = {
    "①  ",
    "②  ",
    "③  ",
    "④  ",
    "⑤  ",
    "⑥  ",
    "⑦  ",
    "⑧  "
}

function GameMatch:initMatch()
    GameTimeTask:start()
    self.MovementArea = Area.new(GameConfig.areaStartPos, GameConfig.areaEndPos, true)
    local TickTask = ITask.new(2, 100)
    function TickTask:onTick()
        if not GameMatch:isGameRunning() then
            return
        end
        EntityManager:onTick()
        CollisionManager:checkCollision()
    end
    TickTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:gameProcess(ticks)
end

function GameMatch:gameProcess(ticks)
    if self.curStatus == self.Status.Waiting then
        self:ifWaitingEnd()
    end

    if self.curStatus == self.Status.Prepare then
        self:ifPrepareEnd()
    end

    if self.curStatus == self.Status.Running then
        self:ifRunningEnd()
        SkillManager:onTick(ticks)
        DeBuffManager:onTick(ticks)
        self:checkBloodReturn()
        self:checkInArea()
        self:ifDoReport(ticks)
        self:gameTip()
        self:gameEvent()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifGameOverEnd()
    end
    BackHallManager:onTick()
end

function GameMatch:ifDoReport(ticks)
    if TableUtil.include(ReportUtil.TimeStage, ticks - self.RunningTick) then
        self:doReport(ticks - self.RunningTick)
    end
end

function GameMatch:doReport(ticks)
    ReportUtil:reportBreakBlock(ticks)
    ReportUtil:reportPlaceBlock(ticks)
    ReportUtil:reportPlayerDie(ticks)
end

function GameMatch:ifWaitingEnd()
    if PlayerManager:getReadyPlayerCount() >= GameConfig.startPlayers then
        if not self.IsWaitingEnd then
            self.WaitingTick = self.curTick
            self.IsWaitingEnd = true
        end

        local seconds = GameConfig.waitingPlayerTime - (self.curTick - self.WaitingTick)
        if seconds < 0 then
            self:prepareGame()
        end
    else
        if self.curTick % 30 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
            MsgSender.sendMsg(IMessages:longWaitingHint())
        end
    end
end

function GameMatch:prepareGame()
    self.curStatus = self.Status.Prepare
    self.PrepareTick = self.curTick
    BoxManager:initBox()
end

function GameMatch:teleGamePos()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if next(player.team) == nil then
            player:initTeam()
        end
        if player.team ~= nil then
            player.team:getTeleportPos(player)
        end
    end
end

function GameMatch:ifPrepareEnd()
    local seconds = GameConfig.gameStartCountDown - (self.curTick - self.PrepareTick)
    if seconds == 45 or seconds == 30 then
        MsgSender.sendMsg(IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
    end
    --游戏开始倒计时,提示可配置
    if seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds == 9 then
        HostApi.sendGameStatus(0, 1)
        HostApi.sendStartGame(PlayerManager:getReadyPlayerCount())
    end

    if seconds <= 0 then
        if PlayerManager:getReadyPlayerCount() >= GameConfig.startPlayers then
            self:doGameStart()
        else
            self.curStatus = self.Status.Waiting
            self.IsWaitingEnd = false
        end
    end
end

function GameMatch:doGameStart()
    self:teleGamePos()
    RewardManager:startGame()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        GamePacketSender:sendHideJobButton(player.rakssid, true)
        GamePacketSender:sendLuckySkyEffectLimit(player.rakssid, 9000)
        player.GameJob:updateJobUIData(player.cur_jobId, -1)
        player:addInitItems()
        player:addInitProps()
        ReportUtil:addUseJob(player.cur_jobId)
        ReportUtil:addChoiceJob(player.choiceJob)
        table.insert(GameMatch.savePlayerRank, player)
    end
    self:sendAllPlayerTeamInfo()
    self:updatePlayerInfo(0)
    self:updateGameInfo()
    self.curStatus = self.Status.Running
    self.RunningTick = self.curTick
    ReportUtil:reportChoiceJob()
    ReportUtil:reportUseJob()
end

function GameMatch:ifRunningEnd()
    local ticks = self:getRunningTime()

    local seconds = GameConfig.gameStageTime - ticks

    if seconds % 60 == 0 and seconds / 60 > 0 then
        MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    end
    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameEndTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds <= 0 then
        self:doDraw()
        self:doGameEnd(0)
    end
end

function GameMatch:doDraw()
    local lifeTeam = self:getLifeTeams()
    if lifeTeam > 0 then
        local winner = 0
        for _, team in pairs(self.teams) do
            if team:isDeath() == false then
                winner = team.teamId
            end
        end
        for _, v in pairs(GameMatch.savePlayerRank) do
            if v.surviveTime == -1 then
                v.surviveTime = GameMatch.curTick - GameMatch.RunningTick
            end
            if v.team.teamId == winner then
                v.winner = 2
            end
        end
    end
end

function GameMatch:doGameEnd(winTeam)
    print("GameMatch:doGameEnd")
    self:doReward(winTeam)
    self:doReport(self.curTick)
    LuaTimer:cancelAll()
    ReportUtil:reportDeadType()
    self.curStatus = self.Status.GameOver
    self.GameOverTick = self.curTick
end

function GameMatch:sortPlayerRank()
    table.sort(GameMatch.savePlayerRank, function(a, b)
        if a.surviveTime ~= b.surviveTime then
            return a.surviveTime > b.surviveTime
        end

        if a.kills ~= b.kills then
            return a.kills > b.kills
        end

        if a.winner ~= b.winner then
            return a.winner > b.winner
        end

    end)
end

function GameMatch:doReward(winTeam)
    self:sortPlayerRank()

    for rank, player in pairs(GameMatch.savePlayerRank) do
        player.result_add_integral = HonourConfig:getHonourByRank(rank)
        SeasonManager:addUserHonor(player.userId, player.result_add_integral, player.level)
        RewardManager:addRewardQueue(player.userId, rank)
    end
    RewardManager:getQueueReward(function(data, winTeam)
        self:sendGameSettlement(winTeam)
    end, winTeam)
end

function GameMatch:sendGameSettlement(winTeam)

    local players_data = {}
    for rank, player in pairs(GameMatch.savePlayerRank) do
        table.insert(players_data, self:newSettlement(player, rank, winTeam))
    end

    for rank, player in pairs(GameMatch.savePlayerRank) do
        local data = {}
        player.rank = rank
        data.players = players_data
        data.own = self:newSettlement(player, rank, winTeam)
        GamePacketSender:sendGameSettlement(player.rakssid, json.encode(data))
    end
end

function GameMatch:newSettlement(player, rank, winTeam)
    local settlement = {}
    settlement.name = player.name
    settlement.rank = rank
    settlement.gold = player.gold
    settlement.kills = player.kills
    settlement.honorId = player.honorId
    settlement.result_add_integral = player.result_add_integral
    if winTeam == 0 then
        if player.team:isDeath() then
            settlement.isWin = GameResult.WIN
        else
            settlement.isWin = GameResult.DRAW --平局
        end
    else
        if player.team.teamId == winTeam then
            settlement.isWin = GameResult.LOSE --输
        else
            settlement.isWin = GameResult.WIN --赢
        end
    end
    return settlement
end

function GameMatch:ifGameOverEnd()
    local seconds = GameConfig.gameOverTime - (self.curTick - self.GameOverTick);
    if seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgCloseServerTimeHint(seconds, IMessages.UNIT_SECOND, false))
    end
    if seconds <= 0 then
        self:doGameOverEnd()
    end
end

function GameMatch:doGameOverEnd()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        HostApi.sendGameover(player.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
    end
    GameTimeTask:pureStop()
    GameServer:closeServer()
end

function GameMatch:onPlayerDie(dier)
    dier:clearEffects()
    dier:removeHotPotato()
    dier:removeTimer()
    dier:resetStatus()
    ReportUtil:addPlayerDie()
    if self:getLifeTeams() <= 2 then
        if self:getLifeTeams() == 2 and dier.team:getTeamCount() >= 2 then
            dier:showBuyRespawn()
            self:ifGameEnd()
            return
        end
        self:upDatePlayerRank(dier)
        dier:onDie()
    else
        dier:showBuyRespawn()
    end
    self:ifGameEnd()
end

function GameMatch:ifGameEnd()
    local lifeTeam = self:getLifeTeams()
    if lifeTeam <= 1 then
        local winner = 0
        for _, team in pairs(self.teams) do
            if team:isDeath() == false then
                winner = team.teamId
            end
        end
        -- local players = PlayerManager:getPlayers()
        for _, v in pairs(GameMatch.savePlayerRank) do
            if v.surviveTime == -1 then
                v.surviveTime = GameMatch.curTick - GameMatch.RunningTick
            end
            if v.team.teamId == winner then
                v:onWin()
                v.winner = 1
                v.surviveTime = v.surviveTime + 3
            end
        end
        self:doGameEnd(winner)
    end
end

function GameMatch:getLifeTeams()
    local lifeTeam = 0
    for i, team in pairs(self.teams) do
        if team:isDeath() == false then
            lifeTeam = lifeTeam + 1
        end
    end
    return lifeTeam
end

function GameMatch:getRunningTime()
    return self.curTick - self.RunningTick
end

function GameMatch:gameEvent()
    local seconds = self:getRunningTime()
    for _, event in pairs(EventTriggerConfig.Event) do
        if event.attackTime == seconds then
            EventConfig:onTriggered(event.eventId)
        end
    end
end

function GameMatch:onPlayerQuit(player)
    player:onLogout()
    self:upDatePlayerRank(player)
    if self:isGameRunning() then
        self:updatePlayerInfo(0)
        self:ifGameEnd()
    end
    self:updateGameInfo()
end

function GameMatch:nameAndTeamId(player)
    local disName = GameMatch.TeamSign[player.teamId] .. player.name
    return disName
end

function GameMatch:buildShowName(player, same)
    if player.teamId == 0 then
        return player.name
    end
    local nameList = {}
    local jobName = "      <" .. "##" .. player.jobName .. "##" .. TextFormat.colorWrite .. ">"
    table.insert(nameList, jobName)
    local disName = player.name
    if same then
        disName = TextFormat.colorBlue .. GameMatch.TeamSign[player.teamId] .. disName
    else
        disName = TextFormat.colorRed .. GameMatch.TeamSign[player.teamId] .. disName
    end
    PlayerIdentityCache:buildNameList(player.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GameMatch:isGameRunning()
    if self.curStatus == self.Status.Running then
        return true
    end
    return false
end

function GameMatch:isGameOver()
    return self.curStatus > self.Status.Running
end

function GameMatch:checkBloodReturn()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if next(player.bloodReturn) ~= nil then
            player:checkBloodReturn()
        end
    end
end

function GameMatch:randomPlayer(teamId, checkMount)
    checkMount = checkMount or false
    local players = PlayerManager:copyPlayers()
    local pool = {}
    for _, player in pairs(players) do
        if player.isLife and player:getTeamId() ~= teamId then
            if checkMount then
                if not player:ifOnMount() then
                    table.insert(pool, player)
                end
            else
                table.insert(pool, player)
            end
        end
    end
    if #pool < 1 then
        return 0
    end
    local target = pool[HostApi.random("RandomPool", 1, #pool + 1)]
    return target.rakssid
end

function GameMatch:randomPlayers(num, rakssid, teamId)
    local players = PlayerManager:copyPlayers()
    local pool = {}
    for _, player in pairs(players) do
        if player.rakssid ~= rakssid and
                player.isLife == true and
                player:getTeamId() ~= teamId then
            table.insert(pool, player)
        end
    end
    if num >= #pool then
        return pool
    end
    local result = {}
    while num > 0 do
        local index = HostApi.random("RandomPool", 1, #pool + 1)
        table.insert(result, pool[index])
        table.remove(pool, index)
        num = num - 1
    end
    return result
end

function GameMatch:getLifePlayerCount()
    local players = PlayerManager:copyPlayers()
    local num = 0
    for _, player in pairs(players) do
        if player.isLife == true then
            num = num + 1
        end
    end
    return num
end

function GameMatch:sendAllPlayerTeamInfo()
    local result = {}
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v:getTeamId() ~= 0 then
            local item = {}
            item.userId = tonumber(tostring(v.userId))
            item.teamId = v:getTeamId()
            table.insert(result, item)
        end
    end
    HostApi.sendPlayerTeamInfo(json.encode(result))
end

function GameMatch:ifPlayerCanLogin()
    return self.curStatus < self.Status.Running
end

function GameMatch:gameTip()
    local seconds = self:getRunningTime()
    if seconds > self.time then
        local tip = GameTip:getTipById(self.index)
        if self.index <= #GameTip.Tips then
            self:sendGameCountDown(tip.gameTip, tip.time)
            self.time = self.time + tip.time
            self.index = self.index + 1
        end
    end
end

function GameMatch:sendGameCountDown(text, time)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        GamePacketSender:sendLuckySkyCountDown(player.rakssid, text, time)
    end
end

function GameMatch:checkInArea()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife and not self.MovementArea:isInArea(player:getPosition()) then
            MsgSender.sendBottomTipsToTarget(player.rakssid, 1, Messages:msgOutOfArea())
            if player:onMountHunt(GameConfig.outOfAreaHurt) then
                return
            end
            player:subHealth(GameConfig.outOfAreaHurt)
        end
    end
end

function GameMatch:upDatePlayerRank(player)
    for _, v in pairs(GameMatch.savePlayerRank) do
        if tostring(v.userId) == tostring(player.userId) then
            if v.surviveTime == -1 then
                v.surviveTime = GameMatch.curTick - GameMatch.RunningTick
            end
            v.kills = player.kills
        end
    end
end

function GameMatch:getMostKiller()
    local players = PlayerManager:getPlayers()
    local killNum = 0
    local killer = 0
    for _, player in pairs(players) do
        if player.kills > killNum and player.isLife then
            killer = player.userId
            killNum = player.kills
        end
    end
    return killer
end

function GameMatch:updatePlayerInfo(rakssid)
    local info = {
        killMost = "",
        survivorNum = self:getLifePlayerCount(),
        surviveTeamNum = self:getLifeTeams(),
    }
    if string.len(self.DragonKnightName) > 0 then
        info.killMost = self.DragonKnightName
    else
        local userId = GameMatch:getMostKiller()
        local mostKiller = PlayerManager:getPlayerByUserId(userId)
        if mostKiller ~= nil then
            info.killMost = mostKiller.name
        end
    end
    if rakssid ~= 0 then
        local player = PlayerManager:getPlayerByRakssid(rakssid)
        if player == nil then
            return
        end
        info.killNum = player.kills
        GamePacketSender:sendLuckySkyPlayerInfo(rakssid, info)
        return
    end
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        info.killNum = player.kills
        GamePacketSender:sendLuckySkyPlayerInfo(player.rakssid, info)
    end
end

function GameMatch:updateGameInfo()
    local info = {
        survivorNum = self:getLifePlayerCount(),
        surviveTeamNum = self:getLifeTeams(),
    }
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        GamePacketSender:sendLuckySkyGameInfo(player.rakssid, info)
    end
end

--中途排名得到玩家的排名
function GameMatch:getPlayerRank()
    local rank = 1
    for i, v in ipairs(self.savePlayerRank) do
        if v.isLife then
            rank = rank + 1
        end
    end
    return rank
end

function GameMatch:setName()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.cur_jobId ~= 0 then
            player.jobName = JobConfig:getNameById(player.cur_jobId)
            player:sendNameToOtherPlayers()
        end
    end
end

function GameMatch:lookingPlayer(time, islandId, msg, arg)
    local players = PlayerManager:getPlayers()
    for _, Pplayer in pairs(players) do
        if IslandConfig:inWhichIsland(Pplayer:getPosition()) == islandId then
            MsgSender.sendBottomTipsToTarget(Pplayer.rakssid, time, msg, arg)
        end
    end
end

function GameMatch:msgFailed(name)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        GamePacketSender:sendGeneralDeathTip(player.rakssid, name, "gui.failed", TextFormat.colorAqua)
    end
end

function GameMatch:msgOutOfWorld(name)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        GamePacketSender:sendGeneralDeathTip(player.rakssid, name, "gui.out.of.world.message", TextFormat.colorAqua)
    end
end

function GameMatch:sendDieMsgWithoutKill(isOutOfWorld, disName)
    if isOutOfWorld then
        MsgSender.sendMsg(Messages:msgOutOfWorldTip(disName))
        GameMatch:msgOutOfWorld(disName)
    else
        MsgSender.sendMsg(Messages:msgFailedTip(disName))
        GameMatch:msgFailed(disName)
    end
end

return GameMatch