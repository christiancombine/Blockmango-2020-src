BaseGridListManager = {}

local gridListGroup = {}

function BaseGridListManager:addGridList(gridList, processId)
    gridListGroup[processId] = gridList
end

function BaseGridListManager:addNewGrid(position, progress, processId)
    if gridListGroup[processId] then
        gridListGroup[processId]:addNewGrid(position, progress)
    end
end

function BaseGridListManager:addTower(tower)
    if gridListGroup[tower.processId] then
        gridListGroup[tower.processId]:addTower(tower)
    end
end

function BaseGridListManager:addAura(processId, aura)
    if gridListGroup[processId] then
        gridListGroup[processId]:addAura(aura)
    end
end

function BaseGridListManager:subTower(tower)
    if gridListGroup[tower.processId] then
        gridListGroup[tower.processId]:subTower(tower)
    end
end

function BaseGridListManager:intoGrid(monster, posKey, oldPosKey, processId)
    if gridListGroup[processId] then
        return gridListGroup[processId]:intoGrid(monster, posKey, oldPosKey)
    end
    return false
end

function BaseGridListManager:canPlace(pos, processId)
    if gridListGroup[processId] then
        return gridListGroup[processId]:canPlace(pos)
    end
    return false
end

function BaseGridListManager:initPathGrid(id, processId)
    if gridListGroup[processId] then
        gridListGroup[processId]:initPathGrid(id)
    end
end

function BaseGridListManager:GetPathMaxProgress(id, processId)
    if gridListGroup[processId] then
        return gridListGroup[processId]:GetPathMaxProgress(id)
    end
    return nil
end

function BaseGridListManager:resetGridList(processId)
    if gridListGroup[processId] then
        gridListGroup[processId]:resetGridList()
    end
end

function BaseGridListManager:getGridByPosition(pos, processId)
    if gridListGroup[processId] then
        return gridListGroup[processId]:getGridByPosition(pos)
    end
    return nil
end