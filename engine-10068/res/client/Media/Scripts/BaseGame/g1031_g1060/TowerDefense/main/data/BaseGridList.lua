BaseGridList = class("BaseGridList")

function BaseGridList:ctor()
    self.gridList = {}
    self.pathProgress = {}
end

function BaseGridList:addNewGrid(position, progress)
    self.gridList[tostring(position.x) .. "#" .. tostring(position.z)] = BaseGrid.new(progress)
    return self.gridList[tostring(position.x) .. "#" .. tostring(position.z)]
end

function BaseGridList:addTower(tower)
    --遍历格子
    local maxRange = tower.config.AttackRange
    maxRange = math.floor(maxRange + 1)
    local towerPos = VectorUtil.toBlockVector3(math.floor(tower.pos.x), 0, math.floor(tower.pos.z))
    for x = towerPos.x - maxRange, towerPos.x + maxRange do
        for z = towerPos.z - maxRange, towerPos.z + maxRange do
            local key = tostring(x) .. "#" .. tostring(z)
            if not self.gridList[key] then
                self.gridList[key] = BaseGrid.new()
            end
            local distance = math.sqrt(math.pow(x - towerPos.x, 2) + math.pow(z - towerPos.z, 2))
            --如果格子在塔攻击范围内则把塔id加入到对应格子的Tower列表
            if distance <= tower.config.AttackRange then
                self.gridList[key]:addAttackerToList(tower)
            end
        end
    end
    self.gridList[tostring(towerPos.x) .. "#" .. tostring(towerPos.z)]:setTower(tower)
end

function BaseGridList:addAura(aura)
    --遍历格子
    local maxRange = aura.config.AuraRange
    maxRange = math.floor(maxRange + 1)
    local auraPos = VectorUtil.toBlockVector3(math.floor(aura.pos.x), 0, math.floor(aura.pos.z))
    for x = auraPos.x - maxRange, auraPos.x + maxRange do
        for z = auraPos.z - maxRange, auraPos.z + maxRange do
            local key = tostring(x) .. "#" .. tostring(z)
            if not self.gridList[key] then
                self.gridList[key] = BaseGrid.new()
            end
            local distance = math.sqrt(math.pow(x - auraPos.x, 2) + math.pow(z - auraPos.z, 2))
            --如果格子在光环范围内则把aura对象放到对应的格子的AuraBuff列表
            if aura.config.AuraRange and distance <= aura.config.AuraRange then
                self.gridList[key]:addAuraToList(aura)
                aura:addGridToList(self.gridList[key])
            end
        end
    end
end

function BaseGridList:subTower(tower)
    --遍历格子
    local maxRange = math.max(tower.config.AttackRange, tower.auraRange or 0)
    maxRange = math.floor(maxRange + 1)
    local towerPos = VectorUtil.newVector3(math.floor(tower.pos.x), 0, math.floor(tower.pos.z))
    local gird = self.gridList[tostring(towerPos.x) .. "#" .. tostring(towerPos.z)]
    if gird then
        gird:setTower()
    else
        ---TODO 需要检查一下为什么格子会为空 @yanzhuolin
        LogUtil.log("BaseGridList:subTower gird = nil, towerPos=" .. VectorUtil.hashVector3(towerPos), LogUtil.LogLevel.Error)
    end
    for x = towerPos.x - maxRange, towerPos.x + maxRange do
        for z = towerPos.z - maxRange, towerPos.z + maxRange do
            local key = tostring(x) .. "#" .. tostring(z)
            if self.gridList[key] then
                self.gridList[key]:updateGrid()
            end
        end
    end
end

function BaseGridList:intoGrid(monster, posKey, oldPosKey)
    --怪物进入新的格子，检查坐标，如果有格子则调用那个格子的进入方法并返回true，没有则返回false
    if oldPosKey and self.gridList[oldPosKey] ~= nil then
        self.gridList[oldPosKey]:onMonsterLevel(monster)
    end

    if self.gridList[posKey] ~= nil then
        self.gridList[posKey]:onMonsterInto(monster)
        return true
    end

    return false
end

function BaseGridList:canPlace(pos)
    local key = tostring(pos.x) .. "#" .. tostring(pos.z)
    if self.gridList[key] and self.gridList[key].tower then
        return false
    end
    return true
end

function BaseGridList:initPathGrid(id)
    if not self.pathProgress[id] then
        local config = PathConfig:getConfigById(id)
        self.pathProgress[id] = self:onFindRoute(config.startPos)
    end
end

function BaseGridList:GetPathMaxProgress(id)
    return self.pathProgress[id]
end

function BaseGridList:onFindRoute(startPos)
    local position = VectorUtil.newVector3i(startPos.x, startPos.y, startPos.z)
    local finished = false
    local progress = 0
    local enterPos = VectorUtil.newVector3(startPos.x + 0.5, startPos.y, startPos.z + 0.5)
    while not finished do
        local find = false
        local blockId = EngineWorld:getBlockId(position)
        if self:isRailBlock(blockId) then
            progress = progress + 1
            local grid = self:addNewGrid(position, progress)
            grid.enterPos = enterPos
        end
        if VectorUtil.equal(position, self.endPos) then
            finished = true
        end
        if finished then
            break
        end
        if not find then
            local posX = VectorUtil.newVector3i(position.x + 1, position.y, position.z)
            blockId = EngineWorld:getBlockId(posX)
            if self:isRailBlock(blockId) and self.gridList[tostring(posX.x) .. "#" .. tostring(posX.z)] == nil then
                find = true
                position = posX
                enterPos = VectorUtil.newVector3(posX.x, posX.y, posX.z + 0.5)
            end
        end
        if not find then
            local negX = VectorUtil.newVector3i(position.x - 1, position.y, position.z)
            blockId = EngineWorld:getBlockId(negX)
            if self:isRailBlock(blockId) and self.gridList[tostring(negX.x) .. "#" .. tostring(negX.z)] == nil then
                find = true
                position = negX
                enterPos = VectorUtil.newVector3(negX.x + 1, negX.y, negX.z + 0.5)
            end
        end
        if not find then
            local posZ = VectorUtil.newVector3i(position.x, position.y, position.z + 1)
            blockId = EngineWorld:getBlockId(posZ)
            if self:isRailBlock(blockId) and self.gridList[tostring(posZ.x) .. "#" .. tostring(posZ.z)] == nil then
                find = true
                position = posZ
                enterPos = VectorUtil.newVector3(posZ.x + 0.5, posZ.y, posZ.z)
            end
        end
        if not find then
            local negZ = VectorUtil.newVector3i(position.x, position.y, position.z - 1)
            blockId = EngineWorld:getBlockId(negZ)
            if self:isRailBlock(blockId) and self.gridList[tostring(negZ.x) .. "#" .. tostring(negZ.z)] == nil then
                find = true
                position = negZ
                enterPos = VectorUtil.newVector3(negZ.x + 0.5, negZ.y, negZ.z + 1)
            end
        end
        if not find then
            finished = true
        end
    end
    return progress
end

function BaseGridList:isRailBlock(blockId)
    if blockId == BlockID.RAIL or
            blockId == BlockID.RAIL_POWERED or
            blockId == BlockID.RAIL_DETECTOR or
            blockId == BlockID.RAIL_ACTIVATOR or
            blockId == BlockID.RAIL_RECEIVE then
        return true
    end
    return false
end

function BaseGridList:resetGridList()
    for _, grid in pairs(self.gridList) do
        grid:resetGrid()
    end
end

function BaseGridList:getGridByPosition(pos)
    return self.gridList[tostring(pos.x) .. "#" .. tostring(pos.z)]
end