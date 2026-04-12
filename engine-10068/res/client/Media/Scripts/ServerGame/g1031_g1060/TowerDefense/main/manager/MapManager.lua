MapManager = class("MapManager", BaseMapManager)

function MapManager:start(roundId)
    for _, path in pairs(self.pathList) do
        if #path.protectUserId > 0 then
            path:start(self.roundConfig[roundId])
        end
    end
end

function MapManager:getMapHp()
    local damage = 0
    for _, path in pairs(self.pathList) do
        damage = damage + path.pathMissNum
    end
    return self.hp + self.extraHp - damage
end

function MapManager:getMapInitHp()
    return self.hp
end

function MapManager:addHp(count)
    self.extraHp = self.extraHp + count
    local process = ProcessManager:getProcessByProcessId(self.processId)
    if process then
        process:broadHomeHp()
    end
end

function MapManager:addPlayer(player, seq)
    if self.pathList[self.pathIds[seq]] == nil then
        self:addPath(self.pathIds[seq])
    end
    self.pathList[self.pathIds[seq]]:addPlayer(player)
    ManagerPacketSender:sendMapManagerCreate(player.rakssid, self.mapConfig, self.pathIds, self.pathIds[seq], self.processId)
end

function MapManager:addWatcher(player, seq)
    ManagerPacketSender:sendMapManagerCreate(player.rakssid, self.mapConfig, self.pathIds, self.pathIds[seq], self.processId)
    ---进入观战模式
end

function MapManager:checkStop()
    local isStop = true
    for _, path in pairs(self.pathList) do
        if path.isRunning then
            isStop = false
            break
        end
    end
    return isStop
end

function MapManager:stopCreateMonster()
    for _, Path in pairs(self.pathList) do
        Path.count = 0
    end
end