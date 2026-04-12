PathManager = class("PathManager", BasePathManager)

function PathManager:running(config)
    if config.count == 0 or self.count == 0 then
        LuaTimer:cancel(config.timerKey)
        return
    end
    if not self.isStopMonster then
        local monster = GameMonster.new(MonsterConfig:getConfigById(config.monsterId), self, self.processId)
        monster.supHomeHp = config.subHomeHp
        monster.teamMoney = config.teamMoney
        monster.goldReward = config.killMoney
        config.count = config.count - 1
        self.count = self.count - 1
    end
end

function PathManager:createTimer(config)
    config.timerKey = LuaTimer:scheduleTimer(function(config)
        self:running(config)
    end, config.monsterInterval * 1000, nil, config)
end

function PathManager:start(roundConfig)
    if self.isRunning or not roundConfig then
        return
    end

    local GroupId = roundConfig.monsterGroupId
    self.GroupConfigs = TableUtil.copyTable(MonsterGroupConfig:getConfigsById(GroupId))
    if #self.GroupConfigs == 0 then
        return
    end
    self.count = 0
    for _, config in pairs(self.GroupConfigs) do
        self.count = self.count + config.count
        if config.monsterDelay == 0 then
            self:createTimer(config)
        else
            LuaTimer:schedule(function(config)
                self:createTimer(config)
            end, config.monsterDelay * 1000, nil, config)
        end
    end
    self.isRunning = true
end

function PathManager:onHomeBeAttack(monster)
    self.pathMissNum = self.pathMissNum + monster.supHomeHp
    for _, userId in pairs(self.protectUserId) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            player.missNum = player.missNum + 1
        end
    end
    local process = ProcessManager:getProcessByProcessId(self.processId)
    if process then
        process:broadHomeHp()
    end
end

function PathManager:addPlayer(player)
    self.super.addPlayer(self, player)
    player.canPlaceBlockId = self.canPlaceBlockIds[#self.protectUserId]
    player:addPath(self)
    player:teleportPosWithYaw(self.playerSpawnPos[#self.protectUserId], self.playerSpawnYaw[#self.protectUserId])

    -- 传送后几帧再设置飞行
    LuaTimer:schedule(function()
        player:setFlying(true)
    end, 10)

    GamePacketSender:sendTowerDefenseCanPlaceBlockId(player.rakssid, self.canPlaceBlockIds[#self.protectUserId])
end

function PathManager:addWatcher(player)
    self.super.addPlayer(self, player)
end

function PathManager:onMonsterDie(monster)
    if #self.protectUserId == 0 then
        return false
    end
    local average = math.floor(monster.teamMoney / #self.protectUserId)
    local remainder = monster.teamMoney % #self.protectUserId
    local luckyNum = math.random(1, #self.protectUserId)
    for i, userId in pairs(self.protectUserId) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if i == luckyNum then
            player.Wallet:addMoney(Define.Money.GameGold, average + remainder)
            player.totalMoney = player.totalMoney + average + remainder
        else
            player.Wallet:addMoney(Define.Money.GameGold, average)
            player.totalMoney = player.totalMoney + average
        end
    end
end

function PathManager:equallyDistributedMoney(moneyNum)
    if #self.protectUserId == 0 then
        return false
    end
    local average = math.floor(moneyNum / #self.protectUserId)
    local remainder = moneyNum % #self.protectUserId
    local luckyNum = math.random(1, #self.protectUserId)
    for i, userId in pairs(self.protectUserId) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if i == luckyNum then
            player.Wallet:addMoney(Define.Money.GameGold, average + remainder)
            player.totalMoney = player.totalMoney + average + remainder
        else
            player.Wallet:addMoney(Define.Money.GameGold, average)
            player.totalMoney = player.totalMoney + average
        end
    end
end

function PathManager:stopMonster(duration)
    self.isStopMonster = true
    LuaTimer:cancel(self.stopMonsterKey or 0)
    self.stopMonsterKey = LuaTimer:schedule(function()
        self.isStopMonster = false
    end, duration * 1000)
end

function PathManager:onPlayerLogout(player)
    self.super.subPlayer(self, player)
    self:equallyDistributedMoney(player.Wallet:getMoneyByCoinId(Define.Money.GameGold))
    player.Wallet:setMoneyCount(Define.Money.GameGold, 0)
end
