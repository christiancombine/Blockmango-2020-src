--Match.lua
require "messages.Messages"

GameMatch = {}

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.AssignRole = 2
GameMatch.Status.Running = 3
GameMatch.Status.GameOver = 4
GameMatch.Status.WaitingReset = 5

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.gameType = "g1013"

GameMatch.gameWaitTick = 0
GameMatch.gameAssignRoleTick = 0
GameMatch.gameStartTick = 0
GameMatch.gameOverTick = 0
GameMatch.gameTelportTick = 0

GameMatch.curGameScene = nil

GameMatch.curTick = 0
GameMatch.isFirstReady = true
GameMatch.isTeleScene = false

GameMatch.teamKills = {}

GameMatch.isConvertToHunter = false
GameMatch.convertToHunterTimeStart = os.time()

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:startAssignRole()
    HostApi.sendGameStatus(0, 1)
    HostApi.sendStartGame(PlayerManager:getPlayerCount())
    self.gameAssignRoleTick = self.curTick
    self.curStatus = self.Status.AssignRole

    self.curGameScene = SceneConfig:randomScene()
    GameConfig.gameTime = self.curGameScene.gameTime

    MsgSender.sendMsg(IMessages:msgGameStart())
    MsgSender.sendMsg(Messages:assignRoleTimeHint(GameConfig.assignRoleTime))
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:teleportScene()
    end
    self.isTeleScene = true
end

function GameMatch:getCurGameScene()
    if self.curGameScene == nil then
        self.curGameScene = SceneConfig:randomScene()
    end
    return self.curGameScene
end

function GameMatch:startGame()
    RewardManager:startGame()
    if self:assignRole() then
        self.gameStartTick = self.curTick
        self.curStatus = self.Status.Running
        self:resetRecord()
        MsgSender.sendMsg(IMessages:msgGameStart())
        HostApi.sendPlaySound(0, 10008)
    else
        self:resetGame(false)
    end
end

function GameMatch:assignRole()
    local pendings_count = self:getPlayerNumForRole(GamePlayer.ROLE_PENDING)
    local zombies_count = self:getCurGameScene():getZombieCount(pendings_count)
    if zombies_count + 1 > pendings_count then
        return false
    end
    local players = PlayerManager:getPlayers()
    while pendings_count > 0 do
        for i, v in pairs(players) do
            if v.role == GamePlayer.ROLE_PENDING then
                if zombies_count > 0 then
                    local odds = zombies_count / pendings_count
                    local random = math.random(1, 100) / 100
                    if random <= odds then
                        if v.multiZombie < 3 then
                            if v:becomeRole(GamePlayer.ROLE_ZOMBIE) then
                                zombies_count = zombies_count - 1
                                pendings_count = pendings_count - 1
                            end
                        else
                            if math.random(1, 100) <= 50 then
                                if v:becomeRole(GamePlayer.ROLE_ZOMBIE) then
                                    zombies_count = zombies_count - 1
                                    pendings_count = pendings_count - 1
                                end
                            end
                        end
                    end
                else
                    if v:becomeRole(GamePlayer.ROLE_SOLDIER) then
                        pendings_count = pendings_count - 1
                    end
                end
            end
        end
    end
    for i, v in pairs(players) do
        v:teleRolePos()
    end
    self:sendAllPlayerTeamInfo()
    return true
end

function GameMatch:sendAllPlayerTeamInfo()
    local result = {}
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isInGame then
            local item = {}
            item.userId = tonumber(tostring(v.userId))
            item.teamId = v.role
            table.insert(result, item)
        end
    end
    HostApi.sendPlayerTeamInfo(json.encode(result))
end

function GameMatch:doReward(role)
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        UserExpManager:addUserExp(player.userId, self.isSameTeam(player.role, role), 2)
    end
    RewardManager:getQueueReward(function(data, role)
        self:sendGameSettlement(role)
    end, role)
    players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        player:onGameEnd(self.isSameTeam(role, player.role))
    end
    self:doReport()
end

function GameMatch:doReport()
    local players = self:sortPlayerRank()
    for rank, player in pairs(players) do
        if player.isLife then
            ReportManager:reportUserData(player.userId, player.kills, rank, 1)
            player.kills = 0
        end
    end
end

function GameMatch.isSameTeam(role1, role2)
    return role1 == role2
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
        HostApi.sendGameSettlement(player.rakssid, json.encode(result), false)
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
    if self.isSameTeam(player.role, winner) then
        item.isWin = 1
    else
        item.isWin = 0
    end
    return item
end

function GameMatch:sortPlayerRank()
    local players = {}
    local c_players = PlayerManager:getPlayers()
    for i, player in pairs(c_players) do
        if player.role ~= GamePlayer.ROLE_PENDING then
            table.insert(players, player)
        end
    end

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

function GameMatch:resetPlayer(report)
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:reset(report)
    end
end

function GameMatch:resetRecord()
    self.recordNum = 1
    self.record = {}
end

function GameMatch:resetMap()
    HostApi:resetMap()
end

function GameMatch:resetGame(report)
    self:resetPlayer(report)
    self:resetMap()
    self:getCurGameScene().produceConfig:reset()
    self:getCurGameScene().inventoryConfig:reset()
    self.isConvertToHunter = false
    self.gameTelportTick = self.curTick
    self.curStatus = self.Status.WaitingReset
    self.isTeleScene = false
    HostApi.sendGameStatus(0, 2)
    HostApi.sendResetGame(PlayerManager:getPlayerCount())
    MsgSender.sendMsg(Messages:gameResetTimeHint())
    MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.prepareTime, IMessages.UNIT_SECOND, false))
end

function GameMatch:ifStartAssignRole()
    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
        if self.isFirstReady then
            MsgSender.sendMsg(IMessages:msgCanStartGame())
            self.gameWaitTick = self.curTick
            self.isFirstReady = false
        end
        if self.curTick - self.gameWaitTick > GameConfig.prepareTime then
            self:startAssignRole()
        else
            local seconds = GameConfig.prepareTime - (self.curTick - self.gameWaitTick)
            if seconds <= 10 and seconds > 0 then
                MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
                if seconds <= 3 then
                    HostApi.sendPlaySound(0, 12);
                else
                    HostApi.sendPlaySound(0, 11);
                end
            end
        end
    else
        if self.curTick % 30 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
        end
        self.isFirstReady = true
    end
end

function GameMatch:ifGameStart()
    local seconds = GameConfig.assignRoleTime - (self.curTick - self.gameAssignRoleTick);

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, Messages:assignRoleTimeHint(seconds))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds <= 0 then
        self:startGame()
    end
end

function GameMatch:ifGameOver()

    local ticks = self.curTick - self.gameStartTick

    local seconds = self:getCurGameScene().gameTime - ticks

    self:showGameData()

    self:addRandomPropToWorld(ticks)

    self:zombieStandRecover()

    self:calculateDistance()

    self:convertToHunter()

    self:poisonAttackBySkill()

    self:zombieSkillDefense()

    self:zombieSkillSprint()

    self:onLifePointTick(ticks)

    if seconds % 60 == 0 and seconds / 60 > 0 then
        MsgSender.sendMsg(IMessages:msgGameEndTimeHint(seconds / 60, IMessages.UNIT_MINUTE, false))
        if seconds <= 10 and seconds > 0 then
            MsgSender.sendBottomTips(3, IMessages:msgGameEndTimeHint(seconds, IMessages.UNIT_SECOND, false))
            if seconds <= 3 then
                HostApi.sendPlaySound(0, 12);
            else
                HostApi.sendPlaySound(0, 11);
            end
        end
    end

    if seconds <= 0 then
        self:doSoldierWin()
    end

end

function GameMatch:showGameData()
    if self.curTick % 10 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player.isInGame == false then
            MsgSender.sendBottomTipsToTarget(player.rakssid, 12, IMessages:msgGameRunning())
        end
    end
end

function GameMatch:addRandomPropToWorld(ticks)
    self:getCurGameScene().produceConfig:addPropToWorld(ticks)
end

function GameMatch:ifTelportEnd()

    local seconds = GameConfig.gameOverTime - (self.curTick - self.gameOverTick);

    MsgSender.sendBottomTips(3, Messages:gameOverHint(seconds))

    if seconds <= 0 then
        self:resetGame()
    end
end

function GameMatch:ifResetEnd()
    self.curStatus = self.Status.WaitingPlayer
    self.isFirstReady = true
    self:ifStartAssignRole()
end

function GameMatch:onTick(ticks)

    self.curTick = ticks

    if self.curStatus == self.Status.WaitingPlayer then
        self:ifStartAssignRole()
    end

    if self.curStatus == self.Status.AssignRole then
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

function GameMatch:getLifeTeams()
    local lifeTeam = 0
    if self:getPlayerNumForRole(GamePlayer.ROLE_SOLDIER) ~= 0 then
        lifeTeam = lifeTeam + 1
    end
    if self:getPlayerNumForRole(GamePlayer.ROLE_ZOMBIE) ~= 0 then
        lifeTeam = lifeTeam + 1
    end
    return lifeTeam
end

function GameMatch:ifGameOverByPlayer()
    local soldierNum = self:getPlayerNumForRole(GamePlayer.ROLE_SOLDIER)
    local zombieNum = self:getPlayerNumForRole(GamePlayer.ROLE_ZOMBIE)

    if soldierNum == 0 then
        self:doZombieWin()
        return true
    end
    if zombieNum == 0 then
        self:doSoldierWin()
        return true
    end
    return false
end

function GameMatch:doZombieWin()
    self.gameOverTick = self.curTick
    self.curStatus = self.Status.GameOver
    MsgSender.sendMsg(IMessages:msgWinnerInfo(Messages:getRoleName(GamePlayer.ROLE_ZOMBIE)))
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_ZOMBIE then
            v:onWin()
        end
    end
    self:doReward(GamePlayer.ROLE_ZOMBIE)
end

function GameMatch:doSoldierWin()
    self.gameOverTick = self.curTick
    self.curStatus = self.Status.GameOver
    MsgSender.sendMsg(IMessages:msgWinnerInfo(Messages:getRoleName(GamePlayer.ROLE_SOLDIER)))
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == GamePlayer.ROLE_SOLDIER then
            v:onWin()
        end
    end
    self:doReward(GamePlayer.ROLE_SOLDIER)
end

function GameMatch:isGameStart()
    return self.curStatus == self.Status.Running
end

function GameMatch:isGameOver()
    return self.curStatus == self.Status.GameOver
end

function GameMatch:getInGamePlayerNum()
    local num = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isInGame then
            num = num + 1
        end
    end
    return num
end

function GameMatch:getPlayerNumForRole(role)
    local num = 0
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.role == role and v.isInGame then
            num = num + 1
        end
    end
    return num
end

function GameMatch:getLastSoldier()
    local soldierNum = self:getPlayerNumForRole(GamePlayer.ROLE_SOLDIER)
    if soldierNum == 1 then
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            if v.role == GamePlayer.ROLE_SOLDIER then
                return v
            end
        end
    end
    return nil
end

function GameMatch:addTeamKills(role)
    if role == GamePlayer.ROLE_SOLDIER or role == GamePlayer.ROLE_ZOMBIE then
        if self.teamKills[role] == nil then
            self.teamKills[role] = 0
        end
        self.teamKills[role] = self.teamKills[role] + 1
    end
end

function GameMatch:sendPlayerChangeTeam(dier)
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player.isInGame then
            HostApi.changePlayerTeam(player.rakssid, dier.userId, GamePlayer.ROLE_ZOMBIE)
        end
    end
end

--僵尸站立回血
function GameMatch:zombieStandRecover()
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        if player.role == GamePlayer.ROLE_ZOMBIE and player.entityPlayerMP ~= nil then
            local curPos = player.entityPlayerMP:getBottomPos()
            if player.lastPos ~= nil and VectorUtil.equal(player.lastPos, curPos) then
                local recover = self:getCurGameScene():getZbStandRecConfig(player.zombieId)
                if recover then
                    if os.time() - player.standTime >= recover.standTime then
                        player.entityPlayerMP:heal(recover.hp)
                        player.standTime = os.time()
                    end
                end
            else
                player.standTime = os.time()
                player.lastPos = {}
                player.lastPos.x = curPos.x
                player.lastPos.y = curPos.y
                player.lastPos.z = curPos.z
            end
        end
    end
end

function GameMatch:getPlayersFromRole(role)
    local players = {}
    local c_players = PlayerManager:getPlayers()
    for i, v in pairs(c_players) do
        if v.role == role then
            players[i] = v
        end
    end
    return players
end

function GameMatch:calculateDistance()
    if self.curTick % 5 ~= 0 then
        return
    end
    local zombies = self:getPlayersFromRole(GamePlayer.ROLE_ZOMBIE)
    local soldiers = self:getPlayersFromRole(GamePlayer.ROLE_SOLDIER)
    for i, soldier in pairs(soldiers) do
        local pos1 = soldier:getPosition()
        if pos1 ~= nil then
            for j, zombie in pairs(zombies) do
                local pos2 = zombie:getPosition()
                if pos2 ~= nil then
                    local x, y, z = math.abs(pos1.x - pos2.x), math.abs(pos1.y - pos2.y), math.abs(pos1.z - pos2.z)
                    if math.sqrt(x * x + y * y + z * z) <= 15 then
                        HostApi.sendPlaySound(soldier.rakssid, 128)
                    end
                end
            end
        end
    end
end

function GameMatch:poisonRangeAttack(userId, pos, range, duration, damage, poisonOnTime)
    local soldiers = self:getPlayersFromRole(GamePlayer.ROLE_SOLDIER)
    for i, soldier in pairs(soldiers) do
        local pos1 = soldier:getPosition()
        if pos1 ~= nil then
            if pos1.x >= pos.x - range and pos1.x <= pos.x + range
                    and pos1.y >= pos.y - range and pos1.y <= pos.y + range
                    and pos1.z >= pos.z - range and pos1.z <= pos.z + range then
                soldier.isPoisonAttack = true
                soldier.poisonAttackStartTime = os.time()
                soldier.poisonLastAttackTime = os.time()
                soldier.poisonAttackOnTime = poisonOnTime
                soldier.poisonDuration = duration
                soldier.poisonDamage = damage
                soldier.poisonZombieUserId = userId
            end
        end
    end
end

function GameMatch:convertToHunter()
    local lastSoldier = self:getLastSoldier()
    local players = PlayerManager:getPlayers()
    if lastSoldier ~= nil then
        if self.isConvertToHunter == false and lastSoldier.isHunter == false then
            self.convertToHunterTimeStart = os.time()
            self.isConvertToHunter = true
            MsgSender.sendCenterTipsToTarget(lastSoldier.rakssid, 3, Messages:convertHunterMsgToSoliderStart())
            for i, v in pairs(players) do
                if v.role == GamePlayer.ROLE_ZOMBIE then
                    MsgSender.sendCenterTipsToTarget(v.rakssid, 3, Messages:convertHunterMsgToZombieStart())
                end
            end
        end

        if os.time() - self.convertToHunterTimeStart >= HunterConfig.transformTime and self.isConvertToHunter == true then
            lastSoldier:convertToHunter()
            MsgSender.sendCenterTipsToTarget(lastSoldier.rakssid, 3, Messages:convertHunterMsgToSoliderEnd())
            for i, v in pairs(players) do
                if v.role == GamePlayer.ROLE_ZOMBIE then
                    MsgSender.sendCenterTipsToTarget(v.rakssid, 3, Messages:convertHunterMsgToZombieEnd())
                end
            end
            self.isConvertToHunter = false
        end
    end
end

function GameMatch:poisonAttackBySkill()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isPoisonAttack == true then
            if os.time() - v.poisonLastAttackTime >= v.poisonAttackOnTime then
                v:onPoison(1)
                v.entityPlayerMP:heal(-v.poisonDamage)
                v.poisonLastAttackTime = os.time()
            end

            if os.time() - v.poisonAttackStartTime > v.poisonDuration then
                v.isPoisonAttack = false
                v.poisonAttackOnTime = 0
                v.poisonDuration = 0
                v.poisonDamage = 0
            end
        end
    end
end

function GameMatch:zombieSkillDefense()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isDefenseSkill then
            if os.time() - v.defenseStartTime >= v.skillDurationTime then
                v.isDefenseSkill = false
                v.defenseCoefficient = 1
                HostApi.setPlayerKnockBackCoefficient(v.rakssid, 1)
            end
        end
    end
end

function GameMatch:zombieSkillSprint()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        if v.isSprintSkill then
            if os.time() - v.sprintStartTime >= v.skillDurationTime then
                v.isSprintSkill = false
                HostApi.setPlayerKnockBackCoefficient(v.rakssid, 1)
                if v.sprintLastBuff ~= nil then
                    for j, buff in pairs(v.sprintLastBuff) do
                        if buff.id == PotionID.MOVE_SPEED then
                            v.entityPlayerMP:removeEffect(buff.id)
                            local gameTime = GameMatch:getCurGameScene().gameTime
                            v.entityPlayerMP:addEffect(buff.id, gameTime, buff.lv)
                        end
                    end
                end
            end
        end
    end
end

function GameMatch:onPlayerQuit(player)
    if self:isGameStart() then
        self:ifGameOverByPlayer()
    end
end

return GameMatch

--endregion
