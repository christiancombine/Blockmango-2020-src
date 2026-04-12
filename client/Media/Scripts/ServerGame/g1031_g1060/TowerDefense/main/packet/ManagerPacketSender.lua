ManagerPacketSender = {}

function ManagerPacketSender:sendMapManagerCreate(rakssid, mapConfig, pathIds, curPathId, processId)
    PacketSender:sendLuaCommonData(rakssid, "MapManagerCreate", json.encode({ mapConfig = mapConfig, pathIds = pathIds, curId = curPathId, processId = processId }))
end

function ManagerPacketSender:sendMonsterCreate(entityId, config, path, processId)
    local data = {
        Id = entityId,
        configId = config.Id,
        pathId = path.Id,
        processId = processId
    }
    local player = PlayerManager:getPlayerByUserId(path.protectUserId[1])
    if player and player.processId then
        ManagerPacketSender:broadcastEntity(player.processId, "MonsterCreate", json.encode(data))
    end
end

function ManagerPacketSender:sendMonsterDead(entityId, processId)
    ManagerPacketSender:broadcastEntity(processId, "MonsterDead", entityId)
end

function ManagerPacketSender:sendTowerCreate(entityId, pos, config, userId, seed)
    local data = {
        Id = entityId,
        configId = config.Id,
        userId = userId,
        seed = seed
    }
    local builder = DataBuilder.new():fromTable(data):addVector3Param("pos", pos):getData()
    local player = PlayerManager:getPlayerByUserId(userId)
    if player and player.processId then
        ManagerPacketSender:broadcastEntity(player.processId, "TowerCreate", builder)
    end
end

function ManagerPacketSender:syncExistingTower(rakssid, entityId, pos, config, userId, seed)
    local data = {
        Id = entityId,
        configId = config.Id,
        userId = userId,
        seed = seed
    }
    local data = DataBuilder.new():fromTable(data):addVector3Param("pos", pos):getData()
    PacketSender:sendLuaCommonData(rakssid, "TowerCreate", data)
end

function ManagerPacketSender:sendTowerDead(entityId, userId)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player and player.processId then
        ManagerPacketSender:broadcastEntity(player.processId, "TowerDead", entityId)
    end
end

function ManagerPacketSender:broadcastEntity(processId, key, data)
    local process = ProcessManager:getProcessByProcessId(processId)
    if process then
        for _, userId in pairs(process.playerList) do
            local cPlayer = PlayerManager:getPlayerByUserId(userId)
            if cPlayer then
                PacketSender:sendLuaCommonData(cPlayer.rakssid, key, data)
            end
        end

        for _, userId in pairs(process.watcherList) do
            local cPlayer = PlayerManager:getWatcherByUserId(userId)
            if cPlayer then
                PacketSender:sendLuaCommonData(cPlayer.rakssid, key, data)
            end
        end
    end
end

function ManagerPacketSender:sendMapManagerDestroy(rakssid)
    PacketSender:sendLuaCommonData(rakssid, "MapManagerDestroy")
end

