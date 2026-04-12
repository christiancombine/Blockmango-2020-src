
GameMatch = {}

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.GameTime = 3
GameMatch.Status.GameOver = 4

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.startWait = false
GameMatch.hasStartGame = false
GameMatch.hasEndGame = false

GameMatch.Teams = {}

GameMatch.allowPvp = false

GameMatch.startGameTime = os.time()
GameMatch.startGamePlayer = 0

GameMatch.cannotPlaceBlock = {}

function GameMatch:init()
    ---init teams
    local teams = TeamConfig:getTeams()
    for _, config in pairs(teams) do
        local team = GameTeam.new(config, TeamConfig:getTeamColor(config.id))
        GameMatch.Teams[config.id] = team
    end
    WaitingPlayerTask:start()
    TreasureChallenge:setSeasonRefreshTimer()
end

function GameMatch:prepareMatch()
    self.hasStartGame = true
    GamePrepareTask:start()
    GamePrepareTask:onTick(1)
end

function GameMatch:startMatch()
    self.allowPvp = true
    self.startGameTime = os.time()
    self.startGamePlayer = PlayerManager:getPlayerCount()

    ---初始化数据站 数据. 包括队伍创建
    GameInfoManager.Instance():initInfo()
    GainsConfig:onGameStart()

    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:setShowName(player:buildShowName())
        ---回满血
        player:setHealth(player:getMaxHealth())
        if player:checkReady() then
            player:onSpawnInGame()
            CommonPacketSender:sendGameStart(player.rakssid)
        end
    end

    RewardManager:startGame()
    if GameMatch:getLifeTeams() <= 1 then
        MessagesManage:sendSystemTipsToAll(IMessages:msgGameOverByLittle())
        self:endMatch()
    else
        MessagesManage:sendSystemTipsToAll(IMessages:msgGameStart())
        GameTimeTask:start()
    end

    PropBlockConfig:createBedProtect()

    LuaTimer:scheduleTimer(function()
        if AIManager:getAICount() == PlayerManager:getPlayerCount() then
            GameServer:closeServer()
        end
    end, 5000)

end

function GameMatch:endMatch()

    if GameConfig and GameConfig.gameTime then
        HostApi.log("endMatch " .. GameConfig.gameTime)
    end

    GamePrepareTask:pureStop()
    GameTimeTask:pureStop()

    local finalWin = true
    local winner = 0

    if (GameMatch:getLifeTeams() > 1) then
        finalWin = false
    end

    local winTeam

    for _, team in pairs(self.Teams) do
        if finalWin and team:isDeath() == false then
            winTeam = team
            winner = team.id
        end
    end
    SettlementManager:updateFinalSettlement(winner)

    if finalWin and winTeam ~= nil then
        local players = PlayerManager:getPlayers()
        for _, _player in pairs(players) do
            MessagesManage:sendSystemTipsToPlayer(_player.rakssid, IMessages:msgWinnerInfo(winTeam:getDisplayName(_player:getLanguage())))
        end
    else
        MessagesManage:sendSystemTipsToAll(IMessages:msgNoWinner(1))
    end
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.surviveTime == -1 then
            player.surviveTime = os.time() - GameMatch.startGameTime
        end
    end

    ReportUtil.reportAIGameEndTime()

    AIManager:onGameOver()

    HostApi.sendPlaySound(0, 10003)
    self.hasEndGame = true
    GameOverTask:start()
end

function GameMatch:cleanPlayer()
    self:doReport()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:overGame()
    end
end

function GameMatch:doReport()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onQuit()
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
        if v.team and v.team.id and v.team.id ~= 0 and (v.team.id == teamId) then
            if v.realLife then
                life = life + 1
            end
        end
    end
    return life
end

function GameMatch:ifGameOver()
    if self.allowPvp == false or self.hasEndGame then
        return
    end
    local lifeTeam = self:getLifeTeams()
    if lifeTeam <= 1 then
        self:endMatch()
    end
end

function GameMatch:onPlayerDie(player)
    if player.team == nil then
        return
    end
    if player.team.isHaveBed then
        RespawnPrivilege:onShowBuyKeepItem(player)
    else
        player.realLife = false
        local life = self:getRealLifePlayerByTeamId(player.team.id)
        if life == 0 then
            ---没有队友存活，直接死亡
            local players = PlayerManager:getPlayers()
            for _, _player in pairs(players) do
                if _player.realLife == false then
                    _player:onDie()
                end
            end
        else
            ---有队友存活，弹出购买复活弹窗
            local ai = AIManager:getAIByRakssid(player.rakssid)
            if ai ~= nil then
                ---AI不选择复活
                ai.luaPlayer:onDie()
            else
                RespawnPrivilege:onShowBuyRespawn(player)
            end
        end
        GameMatch:ifGameOver()
    end
end

function GameMatch:isGameStart()
    return self.hasStartGame
end

-- player died or logout
function GameMatch:onPlayerQuit(player)
    player:onLogout()
    player:onQuit()
    TreasureChallenge:onPlayerLogout(player)
    GameLicense:onPlayerLogout(player)
    if self.allowPvp == true and not player.alreadyReward then
        SettlementManager:onPlayerLose(player)
        if player.isPartyUser then
            self.appIntegral = 0
        elseif not AIManager:isAI(player.rakssid) then
            if not player.alreadyHonor then
                SeasonManager:addUserHonor(player.userId, GameConfig.quitSubHonor, player.level)
                player.alreadyHonor = true
            end
        end
    end
    self:ifGameOver()
    if self.hasStartGame then
        ReportUtil.reportLotteryRefreshTimes(player)
        if AIManager:getRealPlayerCount() == 0 then
            ReportUtil.reportTotalPacketInfo(player)
        end
    end
    local ai = AIManager:getAIByRakssid(player.rakssid)
    if ai then
        ai:onLogout()
    end
end

function GameMatch:getOnePlayerUserId()
    local players = PlayerManager:getPlayers()
    for k, v in pairs(players) do
        return v.userId
    end
    return nil
end

function GameMatch:checkInArea()
    if not GameConfig.MovementArea then
        return
    end
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if not player:isDead() and not GameConfig.MovementArea:inArea(player:getPosition()) then
            ---提示
            MessagesManage:sendSystemTipsToPlayer(player.rakssid, Messages:msgOutOfArea())
            player:subHealth(GameConfig.outOfAreaHurt)
        end
    end
end

return GameMatch

--endregion
