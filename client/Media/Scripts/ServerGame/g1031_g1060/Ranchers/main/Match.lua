
require "messages.Messages"

GameMatch = {}

GameMatch.gameType = "g1031"

GameMatch.curTick = 0

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:kickErrorTargetPlayers()
    self:updateManorInfoState()
    self:updateBuildingInfoState()
    self:updateTasksInfoState()
    self:ifUpdateRank()
    self:addPlayerScore()
    SceneManager:onTick(ticks)
end

function GameMatch:kickErrorTargetPlayers()
    for _, errorTarget in pairs(SceneManager.errorTarget) do
        -- 踢出相关玩家
        local callOnTargets = SceneManager:getCallOnTarget()
        for playerId, target in pairs(callOnTargets) do
            if tostring(target.userId) == tostring(errorTarget.userId) then
                HostApi.log("              GameMatch:kickErrorTargetPlayers :  playerId[" .. tostring(playerId) .. "]")
                local player = PlayerManager:getPlayerByUserId(playerId)
                if player ~= nil then
                    HostApi.log("              player[" .. tostring(player.userId) .. "] has kick out ...")
                    HostApi.sendGameover(player.rakssid, IMessages:msgGameOverByDBDataError(), GameOverCode.LoadDBDataError)
                end
            end
        end
    end
end

function GameMatch:onPlayerQuit(player)
    player:reward()
    player:onQuit()
end

function GameMatch:addPlayerScore()
    if self.curTick == 0 or self.curTick % 60 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:addScore(10)
    end
end

function GameMatch:updateManorInfoState()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.manor ~= nil then
            for _, area in pairs(player.manor.waitForUnlock) do
                if os.time() - area.startTime >= area.totalTime then
                    player.manor:updateUnlockedAreaStateById(area.id)
                end
            end
        end
    end
end

function GameMatch:updateBuildingInfoState()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.building ~= nil then
            for buildingId, building in pairs(player.building.building) do

                local isInWorld = BuildingConfig:isInWorld(building.entityId)
                if isInWorld == true then
                    if building.isInWorld == false then
                        building.isInWorld = true
                        for queueId, queue in pairs(building.queue) do
                            if queue.startTime ~= 0 then
                                queue.startTime = os.time()
                            end
                        end

                        local ranchBuilding = player.building:getBuildingInfoById(building.id)
                        player.building:initRanchBuildingQueue(ranchBuilding)

                    end
                else
                    if building.isInWorld == true then
                        building.isInWorld = false
                        for queueId, queue in pairs(building.queue) do
                            if queue.startTime ~= 0 then
                                queue.needTime = queue.needTime - (os.time() - queue.startTime)
                            end
                        end

                        local ranchBuilding = player.building:getBuildingInfoById(building.id)
                        player.building:initRanchBuildingQueue(ranchBuilding)
                    end
                end

                for queueId, queue in pairs(building.queue) do
                    if queue.startTime ~= 0 and building.isInWorld == true then
                        if os.time() - queue.startTime >= queue.needTime then
                            player.building:updateQueueStateById(buildingId, queueId, player.userId, false)
                        end
                    end
                end

            end
        end
    end
end

function GameMatch:updateTasksInfoState()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.task ~= nil then
            for taskIndex, task in pairs(player.task.tasks) do
                if task.startTime ~= 0 and task.status ~= 4 then
                    if os.time() - task.startTime >= task.needTime then
                        player.task:updateTasksStateById(taskIndex, player.userId)
                    end
                end
            end
        end
    end
end

function GameMatch:ifUpdateRank()
    if self.curTick == 0 or self.curTick % 14400 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    RanchersRankManager:updateRankData()
    for i, v in pairs(players) do
        v:getPlayerRanks()
    end
end

return GameMatch