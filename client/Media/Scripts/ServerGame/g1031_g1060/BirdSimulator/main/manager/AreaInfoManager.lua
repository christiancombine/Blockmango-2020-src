AreaInfoManager = {}

AreaType = {
    House = 1,
    Task = 2,
    DrawLucky = 3,
    Nest = 4,
    Chest = 5,
    Cannon = 6,
    Rank = 7,
    EggChest = 8
}

-- house  task
function AreaInfoManager:push(id, type, startPos, endPos)
    if self.areaInfoList == nil then
        self.areaInfoList = {}
    end
    local score = BirdScope.new()
    score.id = id
    score.type = type
    score.startPos = VectorUtil.newVector3i(startPos.x, startPos.y, startPos.z)
    score.endPos = VectorUtil.newVector3i(endPos.x, endPos.y, endPos.z)
    table.insert(self.areaInfoList, score)
end

function AreaInfoManager:SyncAreaInfo(player)
    local birdSimulator = player:getBirdSimulator()
    if birdSimulator ~= nil then
        local temp = {}
        for _, v in pairs(self.areaInfoList) do
            table.insert(temp, v)
        end

        local score = BirdScope.new()
        score.id = player.userNest.nestIndex
        score.type = AreaType.Nest
        local nestPos = NestConfig:getNestAreaPos(player.userNest.nestIndex)
        score.startPos = VectorUtil.newVector3i(nestPos.minX, nestPos.minY, nestPos.minZ)
        score.endPos = VectorUtil.newVector3i(nestPos.maxX, nestPos.maxY, nestPos.maxZ)
        table.insert(temp, score)

        for i, v in pairs(CannonConfig.cannons) do
            local data = BirdScope.new()
            data.id = v.entityId
            data.type = AreaType.Cannon
            data.startPos = VectorUtil.newVector3i(v.x - 3, v.y, v.z - 3)
            data.endPos = VectorUtil.newVector3i(v.x + 3, v.y + 2, v.z + 3)
            table.insert(temp, data)
        end

        for i, v in pairs(RankConfig.ranks) do
            local data = BirdScope.new()
            data.id = v.entityId
            data.type = AreaType.Rank
            data.startPos = v.startPos
            data.endPos = v.endPos
            table.insert(temp, data)
        end

        birdSimulator:setScopes(temp)
    end
end
