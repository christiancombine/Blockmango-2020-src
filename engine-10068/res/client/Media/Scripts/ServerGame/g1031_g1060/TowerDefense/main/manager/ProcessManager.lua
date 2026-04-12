ProcessManager = {}

ProcessManager.checkPointConfig = {}
ProcessManager.ProcessList = {}

function ProcessManager:init()
    self.checkPointConfigs = CheckPointConfig:getAllCheckPointConfig()
    RewardManager:startGame()
end

function ProcessManager:reset(checkPointId, processId, teamId, teammateNum, playerList)
    --TODO
    self.ProcessList[processId]:onDestroy()

    self.ProcessList[processId] = GameProcess.new(checkPointId, processId, teamId, teammateNum)
    for _, userId in pairs(playerList) do
        local player = PlayerManager:getPlayerByUserId(userId)
        if player then
            GamePacketSender:sendTowerDefensePlayAgain(player.rakssid)
            self.ProcessList[processId]:addPlayer(player)
        end
    end
end

function ProcessManager:addPlayer(player)
    if not player.hallInfo.checkPointId or not player.hallInfo.teamId or not player.hallInfo.teammateNum then
        --默认一个人开启第一个关卡
        player.hallInfo.checkPointId = 1
        player.hallInfo.teamId = 1
        player.hallInfo.teammateNum = 1
        --player:overGame()
        --return
    end

    for _, process in pairs(self.ProcessList) do
        if process and process:addPlayer(player) then
            return
        end
    end

    local roomConfig = RoomConfig:getAllRoomConfig()
    for _, room in pairs(roomConfig) do
        if self.ProcessList[room[1].roomId] == nil then
            self.ProcessList[room[1].roomId] = GameProcess.new(player.hallInfo.checkPointId, room[1].roomId, player.hallInfo.teamId, player.hallInfo.teammateNum)
            self.ProcessList[room[1].roomId]:addPlayer(player)
            return
        end
    end
end

function ProcessManager:subPlayer(player)
    --找到对应进程
    for _, process in pairs(self.ProcessList) do
        process:subPlayer(player)
    end
end

function ProcessManager:addWatcher(player)
    for _, process in pairs(self.ProcessList) do
        if process and process:addWatcher(player) then
            return true
        end
    end
    return false
end

function ProcessManager:subWatcher(player)
    for _, process in pairs(self.ProcessList) do
        process:subWatcher(player)
    end
end

function ProcessManager:onTick(tick)
    for _, process in pairs(self.ProcessList) do
        process:onTick(tick)
    end
end

function ProcessManager:getProcessByProcessId(processId)
    for _, process in pairs(self.ProcessList) do
        if process.processId == processId then
            return process
        end
    end
end

function ProcessManager:closeProcess(processId)
    self.ProcessList[processId] = nil
end

