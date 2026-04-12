---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"
require "config.AirSupportConfig"
require "config.Area"
require "config.MapConfig"
require "config.ChestConfig"
require "config.VehicleConfig"
require "config.AirplaneConfig"

GameMatch = {}
GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.Running = 2
GameMatch.Status.GameOver = 3
GameMatch.Status.EndGame = 4

GameMatch.gameType = "g1016"

GameMatch.gameWaitTick = 0
GameMatch.gameStartTick = 0
GameMatch.gameOverTick = 0
GameMatch.endGameTick = 0

GameMatch.safeAreaStartTick1 = 0
GameMatch.safeAreaStartTick2 = 0
GameMatch.safeAreaStartTick3 = 0
GameMatch.safeAreaStartTick4 = 0
GameMatch.safeAreaStartTick5 = 0

GameMatch.isPoisonAreaFinish1 = false
GameMatch.isPoisonAreaFinish2 = false
GameMatch.isPoisonAreaFinish3 = false
GameMatch.isPoisonAreaFinish4 = false

GameMatch.airSupportTick1 = 0
GameMatch.airSupportTick2 = 0
GameMatch.airSupportTick3 = 0
GameMatch.airSupportTick4 = 0

GameMatch.curTick = 0
GameMatch.isFirstReady = true
GameMatch.isEndGame = false
GameMatch.isReward = false

GameMatch.hasStartGame = false
GameMatch.isAllLanding = false
GameMatch.isCanLanding = false
GameMatch.isPrepareItems = false
GameMatch.isInitPoisonArea = false
GameMatch.map = {}
GameMatch.safeAreaId = 0
GameMatch.poisonAreaId = 0
GameMatch.safeArea = {}
GameMatch.poisonArea = {}

GameMatch.poisonMoveMinX = 0
GameMatch.poisonMoveMaxX = 0
GameMatch.poisonMoveMinZ = 0
GameMatch.poisonMoveMaxZ = 0

GameMatch.prestarted = false

function GameMatch:initMatch()
    GameTimeTask:start()
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
        self:ifCloseServer()
    end

end

function GameMatch:ifGameStart()
    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
        if self.isFirstReady then
            MsgSender.sendMsg(IMessages:msgCanStartGame())
            self.gameWaitTick = self.curTick
            self.isFirstReady = false
        end
        if self.curTick - self.gameWaitTick > GameConfig.waitingPlayerTime or PlayerManager:isPlayerFull() then
            self:GameStart()
        else
            local seconds = GameConfig.waitingPlayerTime - (self.curTick - self.gameWaitTick)
            if seconds == 5 then
                self.prestarted = true
                HostApi.sendStartGame(PlayerManager:getPlayerCount())
            end
            if seconds <= 60 and seconds > 0 then
                MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
                if seconds <= 3 then
                    HostApi.sendPlaySound(0, 12)
                else
                    HostApi.sendPlaySound(0, 11)
                end
            end
        end
    else
        if self.curTick % 30 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
        end
        self.isFirstReady = true
        if self.prestarted then
            self.prestarted = false
            HostApi.sendResetGame(PlayerManager:getPlayerCount())
        end
    end
end

function GameMatch:ifGameOver()
    local ticks = self.curTick - self.gameStartTick
    local seconds = GameConfig.gameTime - ticks
    self:prepareItems()
    self:prepareArea()
    self:poisonAttack()
    self:onLifeTime()
    self:onLifePointTick(ticks)

    if seconds <= 0 then
        self.isEndGame = true
        self:ifGameOverByPlayer()
        self.curStatus = self.Status.GameOver
        self.gameOverTick = self.curTick
    end
end

function GameMatch:ifGameOverByPlayer()

    local winner = 0
    local winPlayerName

    local life = self:getLifePlayer()
    if life <= 1 and self.isReward == false then
        self.isReward = true
        self.gameOverTick = self.curTick
        self.curStatus = self.Status.GameOver
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            if v.isLife then
                winPlayerName = v.name
                winner = v.rakssid
                v:onWin()
            end
        end
        MsgSender.sendMsg(IMessages:msgWinnerInfo(winPlayerName))
        self:doReward(winner)
    end

    if self.isEndGame then
        MsgSender.sendMsg(IMessages:msgNoWinner(0))
        self:doReward(winner)
    end
end

function GameMatch:isGameOver()
    return self.curStatus == self.Status.GameOver
end

function GameMatch:ifCloseServer()
    local ticks = self.curTick - self.gameOverTick
    local seconds = GameConfig.gameOverTime - ticks

    if seconds >= 60 and seconds % 60 == 0 then
        MsgSender.sendMsg(IMessages:msgCloseServerTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
    elseif seconds >= 60 and seconds % 30 == 0 then
        if seconds % 60 == 0 then
            MsgSender.sendMsg(IMessages:msgCloseServerTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
        else
            MsgSender.sendMsg(IMessages:msgCloseServerTimeHint(seconds / 60, IMessages.UNIT_MINUTE, true))
        end
    elseif seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgCloseServerTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12)
        else
            HostApi.sendPlaySound(0, 11)
        end
    end

    if seconds <= 0 then
        GameMatch:cleanPlayer()
        GameTimeTask:pureStop()
    end
end

function GameMatch:GameStart()
    self.hasStartGame = true
    RewardManager:startGame()
    self:onAirPlane()
    self.gameStartTick = self.curTick
    HostApi.sendGameStatus(0, 1)
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    self.curStatus = self.Status.Running
end

function GameMatch:onAirPlane()
    local startPoint = AirplaneConfig:randomStartPoint(AirplaneConfig.area.startPoint)
    local centerPoint = AirplaneConfig:randomCenterPoint(AirplaneConfig.area.centerPoint)
    local endPoint = AirplaneConfig:getEndPoint(startPoint, centerPoint)

    local airPlane = AirplaneConfig:getAirplaneById(1)
    EngineWorld:getWorld():addAircraft(startPoint, 0)
    EngineWorld:getWorld():tryAllPlayerTakeAircraft(startPoint, endPoint, airPlane.speed)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:setShowName(" ")
        v:resetFood()
        HostApi.updateMemberLeftAndKill(v.rakssid, self:getLifePlayer(), v.kills)
    end
    HostApi.setFallingSpeed(GameConfig.horizontalSpeed, GameConfig.verticalSpeed)
end

function GameMatch:prepareItems()
    if self.isPrepareItems == true then
        return
    end

    ChestConfig:prepareChest()
    VehicleConfig:prepareVehicle()
    self.isPrepareItems = true
end

function GameMatch:prepareArea()
    self:prepareSafeArea()
    self:preparePoisonArea1()
    self:preparePoisonArea2()
    self:preparePoisonArea3()
    self:preparePoisonArea4()
    self:preparePoisonArea5()
    self:poisonAreaMove()
end

function GameMatch:prepareSafeArea()
    local safeAreaTick = self.curTick - self.gameStartTick
    local tipTime = 30
    if safeAreaTick == GameConfig.safeAreaTime - tipTime then
        MsgSender.sendCenterTips(3, Messages:safeAreaTimeTip(tipTime))
    end

    if safeAreaTick == GameConfig.safeAreaTime then
        self:showSafeArea()
        MsgSender.sendCenterTips(3, Messages:prepareSafeAreaTip())
        self.safeAreaStartTick1 = self.curTick
    end
end

function GameMatch:preparePoisonArea1()
    if self.safeAreaStartTick1 == 0 then
        return
    end

    local poisonAreaTick = self.curTick - self.safeAreaStartTick1
    local tipTime = 30
    if poisonAreaTick == GameConfig.poisonAreaTime1 - tipTime then
        MsgSender.sendCenterTips(3, Messages:poisonTimeTip(tipTime))
    end

    if poisonAreaTick == GameConfig.poisonAreaTime1 then
        self:poisonAreaBegin()
        MsgSender.sendCenterTips(3, Messages:preparePoisonAreaTip())
    end
end

function GameMatch:preparePoisonArea2()
    if self.safeAreaStartTick2 == 0 then
        return
    end

    local poisonAreaTick = self.curTick - self.safeAreaStartTick2
    local tipTime = 30
    if poisonAreaTick == GameConfig.poisonAreaTime2 - tipTime then
        MsgSender.sendCenterTips(3, Messages:poisonTimeTip(tipTime))
    end

    if poisonAreaTick == GameConfig.poisonAreaTime2 then
        self:poisonAreaBegin()
        MsgSender.sendCenterTips(3, Messages:preparePoisonAreaTip())
    end
end

function GameMatch:preparePoisonArea3()
    if self.safeAreaStartTick3 == 0 then
        return
    end

    local poisonAreaTick = self.curTick - self.safeAreaStartTick3
    local tipTime = 30
    if poisonAreaTick == GameConfig.poisonAreaTime3 - tipTime then
        MsgSender.sendCenterTips(3, Messages:poisonTimeTip(tipTime))
    end

    if poisonAreaTick == GameConfig.poisonAreaTime3 then
        self:poisonAreaBegin()
        MsgSender.sendCenterTips(3, Messages:preparePoisonAreaTip())
    end
end

function GameMatch:preparePoisonArea4()
    if self.safeAreaStartTick4 == 0 then
        return
    end

    local poisonAreaTick = self.curTick - self.safeAreaStartTick4
    local tipTime = 30
    if poisonAreaTick == GameConfig.poisonAreaTime4 - tipTime then
        MsgSender.sendCenterTips(3, Messages:poisonTimeTip(tipTime))
    end

    if poisonAreaTick == GameConfig.poisonAreaTime4 then
        self:poisonAreaBegin()
        MsgSender.sendCenterTips(3, Messages:preparePoisonAreaTip())
    end
end

function GameMatch:preparePoisonArea5()
    if self.safeAreaStartTick5 == 0 then
        return
    end

    local poisonAreaTick = self.curTick - self.safeAreaStartTick5
    local tipTime = 30
    if poisonAreaTick == GameConfig.poisonAreaTime5 - tipTime then
        MsgSender.sendCenterTips(3, Messages:poisonTimeTip(tipTime))
    end

    if poisonAreaTick == GameConfig.poisonAreaTime5 then
        self:poisonAreaBegin()
        MsgSender.sendCenterTips(3, Messages:preparePoisonAreaTip())
    end
end

function GameMatch:getAirSupportPosition()
    local x = AirSupportConfig.airSupportPosition.x
    local z = AirSupportConfig.airSupportPosition.z
    return VectorUtil.toBlockVector3(x, 0, z)
end

function GameMatch:showSafeArea()
    self.safeAreaId = self.safeAreaId + 1
    MapConfig:getSafeArea(self.safeAreaId)

    self.safeArea.minX = MapConfig.safeAreaSize.x
    self.safeArea.minY = MapConfig.safeAreaSize.y
    self.safeArea.minZ = MapConfig.safeAreaSize.z

    local size = MapConfig.safeAreaSize.size

    self.safeArea.maxX = self.safeArea.minX + size
    self.safeArea.maxY = self.safeArea.minY + 150
    self.safeArea.maxZ = self.safeArea.minZ + size

    if self.isInitPoisonArea == false then
        EngineWorld:getWorld():setPoisonCircleRange(
                VectorUtil.newVector3(self.safeArea.minX, self.safeArea.minY, self.safeArea.minZ),
                VectorUtil.newVector3(self.safeArea.maxX, self.safeArea.maxY, self.safeArea.maxZ),
                VectorUtil.newVector3(MapConfig.mapArea.minX, MapConfig.mapArea.minY, MapConfig.mapArea.minZ),
                VectorUtil.newVector3(MapConfig.mapArea.maxX, MapConfig.mapArea.maxY, MapConfig.mapArea.maxZ), 0)
    else
        EngineWorld:getWorld():setPoisonCircleRange(
                VectorUtil.newVector3(self.safeArea.minX, self.safeArea.minY, self.safeArea.minZ),
                VectorUtil.newVector3(self.safeArea.maxX, self.safeArea.maxY, self.safeArea.maxZ),
                VectorUtil.newVector3(self.poisonArea.minX, self.poisonArea.minY, self.poisonArea.minZ),
                VectorUtil.newVector3(self.poisonArea.maxX, self.poisonArea.maxY, self.poisonArea.maxZ), 0)
    end

end

function GameMatch:poisonAreaBegin()
    self.poisonAreaId = self.poisonAreaId + 1

    if self.isInitPoisonArea == false then
        local location = {}
        location[1] = {}
        location[1][1] = MapConfig.mapArea.minX
        location[1][2] = MapConfig.mapArea.minY
        location[1][3] = MapConfig.mapArea.minZ

        location[2] = {}
        location[2][1] = MapConfig.mapArea.maxX
        location[2][2] = MapConfig.mapArea.maxY
        location[2][3] = MapConfig.mapArea.maxZ
        self.poisonArea = Area.new(location)
        self.isInitPoisonArea = true
    end

    self.poisonMoveMinX = self.safeArea.minX
    self.poisonMoveMaxX = self.safeArea.maxX
    self.poisonMoveMinZ = self.safeArea.minZ
    self.poisonMoveMaxZ = self.safeArea.maxZ

    local poison = MapConfig:getPoisonArea(self.poisonAreaId)
    EngineWorld:getWorld():setPoisonCircleRange(
            VectorUtil.newVector3(self.safeArea.minX, self.safeArea.minY, self.safeArea.minZ),
            VectorUtil.newVector3(self.safeArea.maxX, self.safeArea.maxY, self.safeArea.maxZ),
            VectorUtil.newVector3(self.poisonArea.minX, self.poisonArea.minY, self.poisonArea.minZ),
            VectorUtil.newVector3(self.poisonArea.maxX, self.poisonArea.maxY, self.poisonArea.maxZ), poison.moveSpeed)
end

function GameMatch:poisonAreaMove()
    if self.poisonAreaId == 0 then
        return
    end

    local minX = self.poisonMoveMinX
    local maxX = self.poisonMoveMaxX
    local minZ = self.poisonMoveMinZ
    local maxZ = self.poisonMoveMaxZ

    local poison = MapConfig:getPoisonArea(self.poisonAreaId)

    if self.poisonArea.minX < minX then
        self.poisonArea.minX = self.poisonArea.minX + poison.moveSpeed
    end

    if self.poisonArea.maxX > maxX then
        self.poisonArea.maxX = self.poisonArea.maxX - poison.moveSpeed
    end

    if self.poisonArea.minZ < minZ then
        self.poisonArea.minZ = self.poisonArea.minZ + poison.moveSpeed
    end

    if self.poisonArea.maxZ > maxZ then
        self.poisonArea.maxZ = self.poisonArea.maxZ - poison.moveSpeed
    end

    if self.poisonArea.minX >= minX then
        self.poisonArea.minX = minX
    end

    if self.poisonArea.maxX <= maxX then
        self.poisonArea.maxX = maxX
    end

    if self.poisonArea.minZ >= minZ then
        self.poisonArea.minZ = minZ
    end

    if self.poisonArea.maxZ <= maxZ then
        self.poisonArea.maxZ = maxZ
    end

    if self.poisonAreaId == 1 then
        if self.poisonArea.minX == minX and self.poisonArea.maxX == maxX and self.poisonArea.minZ == minZ and self.poisonArea.maxZ == maxZ and self.isPoisonAreaFinish1 == false then
            self.isPoisonAreaFinish1 = true
            self:showSafeArea()
            MsgSender.sendCenterTips(3, Messages:prepareSafeAreaTip())
            self.safeAreaStartTick2 = self.curTick
            self.airSupportTick1 = self.curTick
            self.safeAreaStartTick1 = 0
        end
    end

    if self.poisonAreaId == 2 then
        if self.poisonArea.minX == minX and self.poisonArea.maxX == maxX and self.poisonArea.minZ == minZ and self.poisonArea.maxZ == maxZ and self.isPoisonAreaFinish2 == false then
            self.isPoisonAreaFinish2 = true
            self:showSafeArea()
            MsgSender.sendCenterTips(3, Messages:prepareSafeAreaTip())
            self.safeAreaStartTick3 = self.curTick
            self.airSupportTick2 = self.curTick
            self.safeAreaStartTick2 = 0
        end
    end

    if self.poisonAreaId == 3 then
        if self.poisonArea.minX == minX and self.poisonArea.maxX == maxX and self.poisonArea.minZ == minZ and self.poisonArea.maxZ == maxZ and self.isPoisonAreaFinish3 == false then
            self.isPoisonAreaFinish3 = true
            self:showSafeArea()
            MsgSender.sendCenterTips(3, Messages:prepareSafeAreaTip())
            self.safeAreaStartTick4 = self.curTick
            self.airSupportTick3 = self.curTick
            self.safeAreaStartTick3 = 0
        end
    end

    if self.poisonAreaId == 4 then
        if self.poisonArea.minX == minX and self.poisonArea.maxX == maxX and self.poisonArea.minZ == minZ and self.poisonArea.maxZ == maxZ and self.isPoisonAreaFinish4 == false then
            self.isPoisonAreaFinish4 = true
            self:showSafeArea()
            MsgSender.sendCenterTips(3, Messages:prepareSafeAreaTip())
            self.safeAreaStartTick5 = self.curTick
            self.airSupportTick4 = self.curTick
            self.safeAreaStartTick4 = 0
        end
    end

    if self.poisonAreaId == 5 then
        if self.poisonArea.minX == minX and self.poisonArea.maxX == maxX and self.poisonArea.minZ == minZ and self.poisonArea.maxZ == maxZ and self.isPoisonAreaFinish5 == false then
            self.isPoisonAreaFinish5 = true
            self:showSafeArea()
            MsgSender.sendCenterTips(3, Messages:prepareSafeAreaTip())
            self.safeAreaStartTick5 = 0
        end
    end

    if self.curTick - self.airSupportTick1 == GameConfig.airSupportTime
            or self.curTick - self.airSupportTick2 == GameConfig.airSupportTime
            or self.curTick - self.airSupportTick3 == GameConfig.airSupportTime
            or self.curTick - self.airSupportTick4 == GameConfig.airSupportTime then
        local vec3 = self:getAirSupportPosition()
        local pos = AirSupportConfig:prepareAirSupport(vec3)
        if pos ~= nil then
            EngineWorld:getWorld():addAirDrop(tonumber(pos.x), tonumber(pos.y), tonumber(pos.z))
            MsgSender.sendCenterTips(3, Messages:prepareAirSupportTip())
        end
    end

end

function GameMatch:poisonAttack()
    if self.poisonAreaId == 0 then
        return
    end

    local poison = MapConfig:getPoisonArea(self.poisonAreaId)
    local damage = poison.damage
    local damageOnTime = poison.damageOnTime
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if self.poisonArea:inArea(v:getPosition()) == false then
            if os.time() - v.standTime >= damageOnTime then
                v.entityPlayerMP:heal(-damage)
                v.standTime = os.time()
            end
            if v.isLife then
                MsgSender.sendBottomTipsToTarget(v.rakssid, 1, Messages:poisonAttackTip())
            end
        else
            v.standTime = os.time()
        end
    end
end

function GameMatch:onLifeTime()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isLife then
            v.lifeTime = v.lifeTime + 1
        end
    end
end

function GameMatch:onLifePointTick(ticks)
    if self.curTick % 10 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isLife then
            v:onLifeTick(ticks)
        end
    end
end

function GameMatch:sortPlayerRank()
    local players = PlayerManager:copyPlayers()

    table.sort(players, function(a, b)
        if a.lifeTime ~= b.lifeTime then
            return a.lifeTime > b.lifeTime
        end
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

function GameMatch:doReward(winner)
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        UserExpManager:addUserExp(player.userId, winner == player.rakssid)
    end
    RewardManager:getQueueReward(function(data, winner)
        self:sendGameSettlement(winner)
    end, winner)
    players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        player:onGameEnd(player.isLife)
    end
end

function GameMatch:sendGameSettlement(winner)
    local Players = self:sortPlayerRank()

    local players = {}

    for rank, player in pairs(Players) do
        players[rank] = self:newSettlementItem(player, rank, winner)
    end

    for rank, player in pairs(Players) do
        local result = {}
        player.rank = rank
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
    if item.gold <= 0 then --or player.isSingleReward then
        item.adSwitch = 0
    end
    if player.rakssid == winner then
        item.isWin = 1
    else
        item.isWin = 0
    end
    return item
end

function GameMatch:onPlayerQuit(player)
    player:onQuit()
    if self.hasStartGame then
        self:ifGameOverByPlayer()
    end
end

function GameMatch:cleanPlayer()
    self:doReport()
    local isWin = PlayerManager:getPlayerCount() == 1
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

function GameMatch:getLifePlayer()
    local life = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isLife then
            life = life + 1
        end
    end
    return life
end

return GameMatch
