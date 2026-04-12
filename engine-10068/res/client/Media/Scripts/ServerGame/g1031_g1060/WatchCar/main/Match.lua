---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"
require "config.XAppShopPrivilege"
require "config.XTeamConfig"
require "data.XGameTeam"
require "data.XNpcShop"
require "util.GameManager"
require "util.RewardUtil"

GameMatch = {}

GameMatch.gameType = "g1036"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.Waiting = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.Running = 3
GameMatch.Status.GameOver = 4

GameMatch.PrepareTick = 0
GameMatch.RunningTick = 0
GameMatch.GameOverTick = 0

GameMatch.hasStartGame = false
GameMatch.curTick = 0

GameMatch.Teams = {}
GameMatch.BindMerchants = {}
GameMatch.IsGameOver = false
GameMatch.TimerList = {}

function GameMatch:initMatch()
    GameTimeTask:start()
    self:InitTeams()
    self:InitMerchants()
    self:InitAppShop()
    self.IsGameOver = false
    GameManager:prepareCars()
end

function GameMatch:InitTeams()
    local teams = XTeamConfig:getTeams()
    for _, c_team in pairs(teams) do
        local team = XGameTeam.new(c_team)
        self.Teams[team:getTeamId()] = team
    end
end

function GameMatch:StartTimer(name, time, callback)
    if time == 0 and type(callback) == 'function' then
        callback()
    end
    if self.TimerList[name] == nil then
        self.TimerList[name] = { _time = time, _callback = callback }
    end
end

function GameMatch:InvokeTimer()
    for key, timer in pairs(self.TimerList) do
        if timer._time > 0 then
            timer._time = timer._time - 1
            if timer._time <= 0 and type(timer._callback) == 'function' then
                timer._callback()
                self.TimerList[key] = nil
                break
            end
        end
    end
end

function GameMatch:InitMerchants()
    local configs = XNpcShopConfig:getConfigs()
    for _, config in pairs(configs) do
        local merchant = XNpcShop.new(config)
        table.insert(self.BindMerchants, merchant)
    end
    local coinMaps = {}
    table.insert(coinMaps, { coinId = 1, itemId = 10000 })
    WalletUtils:addCoinMappings(coinMaps)
end

function GameMatch:InitAppShop()
    local appShopConfigs = XAppShopConfig:getConfigs()
    for _, config in pairs(appShopConfigs) do
        if config ~= nil then
            EngineWorld:addGoodsGroupToShop(config.ShopType, config.ShopIcon, config.ShopName)
            local shopConfigs = XAppShopPrivilege:getGoodsByShopId(config.ShopType)
            for _, goods in pairs(shopConfigs) do
                EngineWorld:addGoodsToShop(config.ShopType, goods.intance)
            end
        end
    end
end

function GameMatch:getTeamByTeamId(teamId)
    if teamId ~= nil then
        return self.Teams[teamId]
    end
    return nil
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:InvokeTimer()

    if self.curStatus == self.Status.Waiting then
        self:ifWaitingEnd()
    end

    if self.curStatus == self.Status.Prepare then
        self:ifPrepareEnd()
    end

    if self.curStatus == self.Status.Running then
        self:ifRunningEnd()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifGameOverEnd()
    end
end

function GameMatch:ifWaitingEnd()

    if PlayerManager:getPlayerCount() >= GameConfig.startPlayers then
        self:doWaitingEnd()
    else
        if self.curTick % 30 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
            MsgSender.sendMsg(IMessages:longWaitingHint())
        end
    end
end

function GameMatch:ifPrepareEnd()
    local seconds = GameConfig.prepareTime - (self.curTick - self.PrepareTick)

    if seconds == 45 or seconds == 30 then
        MsgSender.sendMsg(IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
    end

    if seconds == GameConfig.prestartTime and seconds <= 15 then
        HostApi.sendStartGame(PlayerManager:getPlayerCount())
    end

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        self:doPrepareEnd()
    end
end

function GameMatch:ifRunningEnd()
    local ticks = self.curTick - self.RunningTick

    local seconds = GameConfig.gameTime - ticks
    if seconds <= 60 then
        MsgSender.sendBottomTips(1, 1036008, string.format(" %s ", seconds))
    end

    if seconds % 60 == 0 and seconds / 60 > 0 then
        MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    end

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameEndTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        self:doRunningEnd()
    end

    GameManager:onTick(ticks)
end

function GameMatch:ifGameOverEnd()
    local seconds = GameConfig.closeServerTime - (self.curTick - self.GameOverTick)
    if seconds <= 0 then
        self:doGameOverEnd()
    end
end

function GameMatch:doWaitingEnd()
    self.curStatus = self.Status.Prepare
    self.PrepareTick = self.curTick
end

function GameMatch:doPrepareEnd()
    self.hasStartGame = true
    self.curStatus = self.Status.Running

    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    HostApi.sendGameStatus(0, 1)
    GameManager:prepareGame()
    RewardManager:startGame()
end

function GameMatch:doRunningEnd(winTeamId)
    self.curStatus = self.Status.GameOver
    self.GameOverTick = self.curTick
    self:onGameOver(winTeamId)
    self.IsGameOver = true

end

function GameMatch:onGameOver(winTeamId)
    if type(winTeamId) == 'number' then
        local name = XTeamConfig:GetTeamNameById(winTeamId)
        if name ~= nil then
            local color = ""
            if winTeamId == 1 then
                color = TextFormat.colorBlack
            elseif winTeamId == 2 then
                color = TextFormat.colorWrite
            end
            XGameTips:sendTips(1036006, color .. name)
        end
    end
    self:StartTimer("actionTimer", GameConfig.playerActionTime, function()
        local players = RewardUtil:sortPlayerRank(winTeamId)
        if #players == 0 then
            return
        end
        RewardUtil:doReport(players, winTeamId)
        RewardUtil:doReward(players, winTeamId)
    end)
end

function GameMatch:doGameOverEnd()
    GameServer:closeServer()
end

function GameMatch:onPlayerQuit(player)

end

function GameMatch:ifGameRunning()
    return self.curStatus == self.Status.Running
end

return GameMatch